//
// Controller.cpp
//
// Test controller
//
// Created by Sandesh Kumar Sodhi, December 2017
// Copyright (c) [2017] Juniper Networks, Inc. All rights reserved.
//
// All rights reserved.
//
// Notice and Disclaimer: This code is licensed to you under the Apache
// License 2.0 (the "License"). You may not use this code except in compliance
// with the License. This code is not an official Juniper product. You can
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Third-Party Code: This code may depend on other components under separate
// copyright notice and license terms. Your use of the source code for those
// components is subject to the terms and conditions of the respective license
// as noted in the Third-Party source code file.
//

#include <grpc++/grpc++.h>

#include <google/rpc/code.pb.h>
#include <p4runtime.grpc.pb.h>
#include <p4/tmp/p4config.grpc.pb.h>

#include <google/protobuf/util/message_differencer.h>

#include <arpa/inet.h>
#include <fstream>
#include <memory>
#include <streambuf>
#include <string>
#include <thread>
#include <cstring>
#include <cassert>
#include <iomanip>
#include <unistd.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "P4InfoUtils.h"
#include "Controller.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using google::protobuf::util::MessageDifferencer;
using namespace std::chrono_literals;

constexpr char test_json[] =
    "/b/sgopinath/code/JP4Agent/test/controller/testdata/afi_switch.json";
constexpr char test_proto_txt[] =
    "/b/sgopinath/code/JP4Agent/test/controller/testdata/simple_router.proto.txt";

// Static configuration for interfaces
constexpr interface_t intfs[] = {
    // ge-0.0.2-vmx1
    {0, {103, 30, 120, 1}, {0xfe, 0x26, 0x0a, 0x2e, 0xaa, 0xf2}},
    // ge-0.0.3-vmx1
    {1, {103, 30, 130, 1}, {0xfe, 0x26, 0x0a, 0x2e, 0xaa, 0xf3}}
};

class StreamChannelSyncClient
{
public:
    using StreamChannelRW =
        ClientReaderWriter<p4::StreamMessageRequest, p4::StreamMessageResponse>;

    StreamChannelSyncClient(std::shared_ptr<Channel> channel,
                            std::chrono::milliseconds timeout)
        : stub_{p4::P4Runtime::NewStub(channel)}
    {
        if (timeout.count() > 0) {
            auto deadline = std::chrono::system_clock::now() + timeout;
            stream_ctxt.set_deadline(deadline);
        }
        stream = stub_->StreamChannel(&stream_ctxt);
    }

    StreamChannelRW *get_stream() { return stream.get(); }

    bool send_pkt_out(std::string &&payload)
    {
        p4::StreamMessageRequest pkt_out;
        pkt_out.mutable_packet()->set_payload(std::move(payload));
        return stream->Write(pkt_out);
    }

private:
    std::unique_ptr<p4::P4Runtime::Stub> stub_;
    ClientContext stream_ctxt;
    std::unique_ptr<StreamChannelRW> stream;
};

bool
ControllerInjectL2Pkt(const std::string &l2_pkt, uint16_t egress_port)
{
    // Create gRPC stub and open the stream channel
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());

    StreamChannelSyncClient sc{channel, 0s};

    // Encapsulate L2 pkt with cpu_header
    cpu_header_t cpu_hdr;
    memset(&cpu_hdr, 0, cpu_hdr_sz);
    cpu_hdr.port = htons(egress_port);

    std::string payload{(char *)&cpu_hdr, cpu_hdr_sz};
    payload.append(l2_pkt);

    // Inject L2 pkt on the stream
    return sc.send_pkt_out(std::move(payload));
}

bool
ControllerPuntPkt(std::string &l2_pkt, uint16_t &ingress_port,
                  std::chrono::milliseconds timeout_ms)
{
    // Create gRPC stub and open the stream channel
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());

    StreamChannelSyncClient sc{channel, timeout_ms};
    auto *stream = sc.get_stream();

    // Listen for L2 pkt on the stream channel
    p4::StreamMessageResponse response;
    bool rdstatus = stream->Read(&response);
    if (!rdstatus) {
        stream->WritesDone();
        Status s = stream->Finish();
        if (s.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
            std::cout << __func__ << ": Read packet timed out.\n";
        }
    }
    if (response.update_case() != p4::StreamMessageResponse::kPacket) {
        return false;
    }
    std::string recvd_pkt = response.packet().payload();

    // Decapsulate cpu_header
    char zero[8]{};
    if ((recvd_pkt.size() <= cpu_hdr_sz) ||
        (memcmp(zero, recvd_pkt.data(), 8) != 0)) {
        return false;
    }

    ingress_port = ntohs(((struct cpu_header_t *)recvd_pkt.data())->port);
    std::cout << __PRETTY_FUNCTION__
              << ": Received L2 pkt (size: " << recvd_pkt.size() - cpu_hdr_sz
              << " bytes) on ingress port " << ingress_port << "\n";

    // Copy L2 pkt payload alone
    l2_pkt.assign(recvd_pkt.begin() + cpu_hdr_sz, recvd_pkt.end());
    return true;
}

uint16_t csum(uint16_t *addr, int len)
{
    int nleft = len;
    int sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *(uint8_t *)(&answer) = *(uint8_t *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

bool
ControllerICMPEcho(std::chrono::milliseconds timeout_ms)
{
    // Create gRPC stub and open the stream channel with 5 second timeout.
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());

    StreamChannelSyncClient sc{channel, timeout_ms};
    auto *stream = sc.get_stream();

    // Listen for L2 pkt on the stream channel
    p4::StreamMessageResponse response;
    bool rdstatus = stream->Read(&response);
    if (!rdstatus) {
        stream->WritesDone();
        Status s = stream->Finish();
        if (s.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
            //std::cout << __func__ << ": Read packet timed out.\n";
        }
    }
    if (response.update_case() != p4::StreamMessageResponse::kPacket) {
        return false;
    }
    std::string recvd_pkt = response.packet().payload();
    
    // Sanity check
    char zero[8]{};
    constexpr auto icmp_pkt_min_len = cpu_hdr_sz + sizeof(struct ether_header) +
                                      sizeof(struct iphdr) +
                                      sizeof(struct icmphdr);
    if ((recvd_pkt.size() < icmp_pkt_min_len) ||
        (memcmp(zero, recvd_pkt.data(), 8) != 0)) {
        std::cout << "Recvd pkt not an ICMP Echo request.\n";
        return false;
    }

    std::cout << "Recvd ICMP Echo request.\n";
    // Use info in received pkt to construct ICMP echo reply
    const auto *cpu_hdr = (struct cpu_header_t *)recvd_pkt.data();
    const auto *eth_hdr = (struct ether_header *)(cpu_hdr + 1);
    const auto *ip_hdr = (struct iphdr *)(eth_hdr + 1);
    const auto *icmp_pkt = (struct icmp *)(ip_hdr + 1);

    if (ip_hdr->protocol != IPPROTO_ICMP && icmp_pkt->icmp_type != ICMP_ECHO) {
        std::cout << "Not ICMP Echo pkt.\n";
        return false;
    }

    std::string send_pkt{recvd_pkt};

    // cpu_hdr copied as is, since ingress & egress ports are the same.
    // Ret ethernet hdr: switch source and dest mac. copy ether_type.
    auto *rep_eth_hdr = (struct ether_header *)(send_pkt.data() + cpu_hdr_sz);
    std::copy_n(eth_hdr->ether_shost, sizeof(struct ether_addr),
                rep_eth_hdr->ether_dhost);
    std::copy_n(eth_hdr->ether_dhost, sizeof(struct ether_addr),
                rep_eth_hdr->ether_shost);

    // Fill in IP hdr.
    auto *rep_ip_hdr = (struct iphdr *)(rep_eth_hdr + 1);
    rep_ip_hdr->check = 0;
    rep_ip_hdr->daddr = ip_hdr->saddr;
    rep_ip_hdr->saddr = ip_hdr->daddr;
    rep_ip_hdr->check = csum((uint16_t *)rep_ip_hdr, sizeof(struct iphdr));

    // Ret ICMP: set ICMP type & calculate ICMP pkt checksum.
    auto *rep_icmp = (struct icmp *)(rep_ip_hdr + 1);
    rep_icmp->icmp_type = ICMP_ECHOREPLY;
    rep_icmp->icmp_code = 0;
    rep_icmp->icmp_cksum = 0;
    const size_t icmp_pkt_len =
        recvd_pkt.size() - icmp_pkt_min_len + sizeof(struct icmphdr);
    rep_icmp->icmp_cksum = csum((uint16_t *)rep_icmp, icmp_pkt_len);

    // Write pkt to stream channel
    sc.send_pkt_out(std::move(send_pkt));

    return true;
}

bool
ControllerHandleArpReq(std::chrono::milliseconds timeout_ms)
{
    // Create gRPC stub and open the stream channel with 5 second timeout.
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());

    StreamChannelSyncClient sc{channel, timeout_ms};
    auto *stream = sc.get_stream();

    // Listen for L2 pkt on the stream channel
    p4::StreamMessageResponse response;
    bool rdstatus = stream->Read(&response);
    if (!rdstatus) {
        stream->WritesDone();
        Status s = stream->Finish();
        if (s.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
            //std::cout << __func__ << ": Read packet timed out.\n";
        }
    }
    if (response.update_case() != p4::StreamMessageResponse::kPacket) {
        return false;
    }
    const std::string recvd_pkt = response.packet().payload();

    // Sanity check.
    char zero[8]{};
    constexpr size_t arp_pkt_len = sizeof(cpu_header_t) +
                                   sizeof(struct ether_header) +
                                   sizeof(struct ether_arp);
    if ((recvd_pkt.size() < arp_pkt_len) ||
        (memcmp(zero, recvd_pkt.data(), 8) != 0)) {
        std::cout << "Recvd pkt not an ARP request.\n";
        return false;
    }

    const auto *recv_eth_hdr = reinterpret_cast<const ether_header *>(
        recvd_pkt.data() + sizeof(cpu_header_t));
    const auto *recv_arp_hdr =
        reinterpret_cast<const ether_arp *>(recv_eth_hdr + 1);

    if ((ETHERTYPE_ARP != ntohs(recv_eth_hdr->ether_type)) ||
        (ARPOP_REQUEST != ntohs(recv_arp_hdr->arp_op))) {
        std::cout << "Not an ARP request packet.\n";
        return false;
    }

    // Print recvd pkt
    std::cout << __func__ << "(): Recvd pkt:" << std::hex << std::uppercase;
    for (size_t i = 0; i < recvd_pkt.size(); i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)recvd_pkt[i];
    }
    std::cout << "\n" << std::dec << std::nouppercase;

    // Construct ARP reply packet
    std::string arp_reply(arp_pkt_len, '\0');

    for (const auto &intf : intfs) {
        if (memcmp(&intf.ip_addr, (uint8_t *)(&recv_arp_hdr->arp_tpa),
                   sizeof(intf.ip_addr)) == 0) {
            auto *cpu_hdr = reinterpret_cast<cpu_header_t *>(&arp_reply[0]);
            cpu_hdr->reason = htons(static_cast<uint16_t>(Cpureason::ArpMsg));
            cpu_hdr->port = htons(intf.port);

            auto *eth_hdr = reinterpret_cast<ether_header *>(cpu_hdr + 1);
            memcpy(eth_hdr->ether_dhost, recv_arp_hdr->arp_sha, ETHER_ADDR_LEN);
            memcpy(eth_hdr->ether_shost, &intf.mac_addr, ETHER_ADDR_LEN);
            eth_hdr->ether_type = htons(ETHERTYPE_ARP);

            auto *arp_hdr = reinterpret_cast<ether_arp *>(eth_hdr + 1);
            arp_hdr->arp_hrd = htons(ARPHRD_ETHER);
            arp_hdr->arp_pro = htons(ETH_P_IP);  // IPv4
            arp_hdr->arp_hln = ETHER_ADDR_LEN;
            arp_hdr->arp_pln = sizeof(in_addr_t);
            arp_hdr->arp_op = htons(ARPOP_REPLY);
            memcpy(arp_hdr->arp_sha, &intf.mac_addr, ETHER_ADDR_LEN);
            memcpy((uint8_t *)&arp_hdr->arp_spa, &intf.ip_addr,
                   sizeof(in_addr_t));
            memcpy(arp_hdr->arp_tha, recv_arp_hdr->arp_sha, ETHER_ADDR_LEN);
            memcpy((uint8_t *)&arp_hdr->arp_tpa,
                   (uint8_t *)&recv_arp_hdr->arp_spa, sizeof(in_addr_t));

            // Write pkt to stream channel
            sc.send_pkt_out(std::move(arp_reply));
            return true;
        }
    }

    std::cout << __func__ << "ARP request not for local interface.\n";
    return false;
}

#ifndef SUD
static int
routeAdd(p4::config::P4Info& p4Info,
         std::unique_ptr<p4::P4Runtime::Stub>& pi_stub_,
         uint32_t dAddr,
         uint16_t pLen,
         uint64_t mac,
         uint16_t oPort)
{
    int dev_id = 0;

    auto set_election_id = [](p4::Uint128 *election_id) {
        election_id->set_high(0);
        election_id->set_low(1);
    };

    std::cout << "Write: Adding route table entry...";

    Utils utils;
    std::string dstAddr  = utils.uint2Str(dAddr);
    std::string macAddr  = utils.uint2Str(mac);
    std::string oPortStr = utils.uint2Str(oPort);

    auto tId  = get_table_id(p4Info, "ipv4_lpm");
    auto mfId = get_mf_id(p4Info, "ipv4_lpm", "ipv4.dstAddr");
    auto aId  = get_action_id(p4Info, "set_nhop");
    auto p0Id = get_param_id(p4Info, "set_nhop", "nhop_ipv4");
    auto p1Id = get_param_id(p4Info, "set_nhop", "port");

    p4::Entity entity;
    auto tableEntry = entity.mutable_table_entry();

    tableEntry->set_table_id(tId);
    auto match = tableEntry->add_match();
    match->set_field_id(mfId);

    auto lpm = match->mutable_lpm();
    lpm->set_value(dstAddr);
    lpm->set_prefix_len(pLen);

    auto tableAction = tableEntry->mutable_action();
    auto action = tableAction->mutable_action();
    action->set_action_id(aId);
    {
        auto param = action->add_params();
        param->set_param_id(p0Id);
        std::string nhAddr;
        param->set_value(nhAddr);
    }
    {
        auto param = action->add_params();
        param->set_param_id(p1Id);
        param->set_value(oPortStr);
    }

    // add entry
    {
        p4::WriteRequest request;
        set_election_id(request.mutable_election_id());
        request.set_device_id(dev_id);
        auto update = request.add_updates();
        update->set_type(p4::Update_Type_INSERT);
        update->set_allocated_entity(&entity);

        ClientContext context;
        p4::WriteResponse rep;

        auto status = pi_stub_->Write(&context, request, &rep);
        assert(status.ok());

        update->release_entity();
    }

    std::cout << "Done.\n";

    return 0;
}
#endif // SUD

int
ControllerAddRouteEntry()
{
    int dev_id = 0;
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());
    std::unique_ptr<p4::P4Runtime::Stub> pi_stub_(
        p4::P4Runtime::NewStub(channel));

    auto p4info = parse_p4info(test_proto_txt);

    auto set_election_id = [](p4::Uint128 *election_id) {
        election_id->set_high(0);
        election_id->set_low(1);
    };

    // Open bidirectional stream and advertise election id.
    ClientContext stream_context;
    auto stream = pi_stub_->StreamChannel(&stream_context);
    {
        p4::StreamMessageRequest request;
        auto arbitration = request.mutable_arbitration();
        arbitration->set_device_id(dev_id);
        set_election_id(arbitration->mutable_election_id());
        stream->Write(request);
        p4::StreamMessageResponse response;
        stream->Read(&response);
        assert(response.update_case() ==
               p4::StreamMessageResponse::kArbitration);
        assert(response.arbitration().status().code() ==
               ::google::rpc::Code::OK);
    }

#ifndef SUD
    // transit routes
    std::cout << "Adding transit routes.." << std::endl;
    routeAdd(p4info, pi_stub_, 0x2c2c2c02, 24, 0x88a25e91c0a9, 9);
    routeAdd(p4info, pi_stub_, 0x37373702, 24, 0x88a25e9175ff, 13);

    // local routes
    std::cout << "Adding local routes.." << std::endl;
    routeAdd(p4info, pi_stub_, 0x2c2c2c01, 32, 0x88a25e91a2a8, 0);
    routeAdd(p4info, pi_stub_, 0x37373701, 32, 0x88a25e91a2a9, 0);
#else // SUD

    std::cout << "Write: Adding route table entry...";
    auto t_id = get_table_id(p4info, "ipv4_lpm");
    auto mf_id = get_mf_id(p4info, "ipv4_lpm", "ipv4.dstAddr");
    auto a_id = get_action_id(p4info, "set_nhop");
    auto p0_id = get_param_id(p4info, "set_nhop", "nhop_ipv4");
    auto p1_id = get_param_id(p4info, "set_nhop", "port");

    p4::Entity entity;
    auto table_entry = entity.mutable_table_entry();

    table_entry->set_table_id(t_id);
    auto match = table_entry->add_match();
    match->set_field_id(mf_id);

    auto lpm = match->mutable_lpm();
    // lpm->set_value(std::string("\x0a\x00\x00\x01", 4));  // 10.0.0.1
    lpm->set_value(std::string("\x67\x1e\x1e\x03", 4));  // 10.0.0.1
    lpm->set_prefix_len(16);

    auto table_action = table_entry->mutable_action();
    auto action = table_action->mutable_action();
    action->set_action_id(a_id);
    {
        auto param = action->add_params();
        param->set_param_id(p0_id);
        param->set_value(std::string("\x0a\x00\x00\x01", 4));  // 10.0.0.1
    }
    {
        auto param = action->add_params();
        param->set_param_id(p1_id);
        // param->set_value(std::string("\x00\x09", 2));
        param->set_value(std::string("\x00\x01", 2));
    }

    // add entry
    {
        p4::WriteRequest request;
        set_election_id(request.mutable_election_id());
        request.set_device_id(dev_id);
        auto update = request.add_updates();
        update->set_type(p4::Update_Type_INSERT);
        update->set_allocated_entity(&entity);
        ClientContext context;
        p4::WriteResponse rep;
        auto status = pi_stub_->Write(&context, request, &rep);
        assert(status.ok());
        update->release_entity();
    }

    std::cout << "Done.\n";
#endif // SUD

    // Close the bidirectional stream.
    {
        stream->WritesDone();
        p4::StreamMessageResponse response;
        while (stream->Read(&response)) {
        }
        auto status = stream->Finish();
        assert(status.ok());
    }

    return 0;
}

int
ControllerSetConfig()
{
    int dev_id = 0;

    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());
    std::unique_ptr<p4::P4Runtime::Stub> pi_stub_(
        p4::P4Runtime::NewStub(channel));

    auto p4info = parse_p4info(test_proto_txt);

    auto set_election_id = [](p4::Uint128 *election_id) {
        election_id->set_high(0);
        election_id->set_low(1);
    };

    // initial handshake: open bidirectional stream and advertise election
    // id. This stream needs to stay open for the lifetime of the controller.
    ClientContext stream_context;
    auto stream = pi_stub_->StreamChannel(&stream_context);
    {
        p4::StreamMessageRequest request;
        auto arbitration = request.mutable_arbitration();
        arbitration->set_device_id(dev_id);
        set_election_id(arbitration->mutable_election_id());
        stream->Write(request);
        p4::StreamMessageResponse response;
        stream->Read(&response);
        assert(response.update_case() ==
               p4::StreamMessageResponse::kArbitration);
        assert(response.arbitration().status().code() ==
               ::google::rpc::Code::OK);
    }

    std::cout
        << "SetForwardingPipelineConfigRequest: Setting forwarding topo..."
        << std::flush;

    {
        p4::SetForwardingPipelineConfigRequest request;
        request.set_device_id(dev_id);
        request.set_action(
            p4::SetForwardingPipelineConfigRequest_Action_VERIFY_AND_COMMIT);
        set_election_id(request.mutable_election_id());
        auto config = request.mutable_config();
        config->set_allocated_p4info(&p4info);
        p4::tmp::P4DeviceConfig device_config;
        std::ifstream istream(test_json);
        device_config.mutable_device_data()->assign(
            (std::istreambuf_iterator<char>(istream)),
            std::istreambuf_iterator<char>());
        device_config.SerializeToString(config->mutable_p4_device_config());

        p4::SetForwardingPipelineConfigResponse rep;
        ClientContext context;
        auto status =
            pi_stub_->SetForwardingPipelineConfig(&context, request, &rep);
        assert(status.ok());
        config->release_p4info();
    }

    std::cout << "Done." << std::endl;

#if 0
  auto read_one = [&dev_id, &pi_stub_, &table_entry] () {
    p4::ReadRequest request;
    request.set_device_id(dev_id);
    auto entity = request.add_entities();
    entity->set_allocated_table_entry(table_entry);
    ClientContext context;
    std::unique_ptr<grpc::ClientReader<p4::ReadResponse> > reader(
        pi_stub_->Read(&context, request));
    p4::ReadResponse rep;
    reader->Read(&rep);
    auto status = reader->Finish();
    assert(status.ok());
    entity->release_table_entry();
    return rep;
  };

  // get entry, check it is the one we added
  {
    auto rep = read_one();
    assert(rep.entities().size() == 1);
    assert(MessageDifferencer::Equals(entity, rep.entities().Get(0)));
  }

  // remove entry
  {
    p4::WriteRequest request;
    set_election_id(request.mutable_election_id());
    request.set_device_id(dev_id);
    auto update = request.add_updates();
    update->set_type(p4::Update_Type_DELETE);
    update->set_allocated_entity(&entity);
    ClientContext context;
    p4::WriteResponse rep;
    auto status = pi_stub_->Write(&context, request, &rep);
    assert(status.ok());
    update->release_entity();
  }

  // check entry is indeed gone
  {
    auto rep = read_one();
    assert(rep.entities().size() == 0);
  }
#endif

    // Close the bidirectional stream.
    {
        stream->WritesDone();
        p4::StreamMessageResponse response;
        while (stream->Read(&response)) {
        }
        auto status = stream->Finish();
        assert(status.ok());
    }

    return 0;
}
