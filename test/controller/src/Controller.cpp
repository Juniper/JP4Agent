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
#ifdef UBUNTU
#include <p4runtime.grpc.pb.h>
#else
#include <p4runtime_wrl.grpc.pb.h>
#endif
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
#include "Utils.h"
#include "Controller.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using google::protobuf::util::MessageDifferencer;
using namespace std::chrono_literals;

char test_json[] =
    "/root/JP4Agent/test/controller/testdata/afi_switch.json";
char test_proto_txt[] =
    "/root/JP4Agent/test/controller/testdata/simple_router.proto.txt";

// Static configuration for interfaces
constexpr interface_t intfs[] = {
    // ge-0.0.2-vmx1
    {0, {103, 30, 120, 1}, {0xfe, 0x26, 0x0a, 0x2e, 0xaa, 0xf2}},
    // ge-0.0.3-vmx1
    {1, {103, 30, 130, 1}, {0xfe, 0x26, 0x0a, 0x2e, 0xaa, 0xf3}}
};

void ControllerSetP4Input(std::string pipelineFile, std::string runtimeFile)
{
    strcpy(test_json, pipelineFile.c_str());
    strcpy(test_proto_txt, runtimeFile.c_str());
}

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

bool
ControllerInjectPuntL2Pkt(const std::string &inject_l2_pkt,
                          std::string &punt_l2_pkt,
                          uint16_t egress_port,
                          uint16_t ingress_port,
                          std::chrono::milliseconds timeout_ms)
{
    // Create gRPC stub and open the stream channel
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());

    StreamChannelSyncClient sc{channel, timeout_ms};

    // Encapsulate L2 pkt with cpu_header
    cpu_header_t cpu_hdr;
    memset(&cpu_hdr, 0, cpu_hdr_sz);
    cpu_hdr.port = htons(ingress_port);

    std::string payload{(char *)&cpu_hdr, cpu_hdr_sz};
    payload.append(inject_l2_pkt);

    // Inject L2 pkt on the stream
    sc.send_pkt_out(std::move(payload));

    // Listen for L2 pkt on the stream channel
    auto *stream = sc.get_stream();
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

    egress_port = ntohs(((struct cpu_header_t *)recvd_pkt.data())->port);
    std::cout << __PRETTY_FUNCTION__
              << ": Received L2 pkt (size: " << recvd_pkt.size() - cpu_hdr_sz
              << " bytes) on egress port " << egress_port << "\n";

    // Copy L2 pkt payload alone
    punt_l2_pkt.assign(recvd_pkt.begin() + cpu_hdr_sz, recvd_pkt.end());
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
    std::string tmp_recvd_pkt = response.packet().payload();

    // XXX: HACK ALERT: Possible bug in VMXZT leads to 5 extra bytes being
    // appended to the punted packet. Work around this for now.
    const size_t tmp_payload_len = (tmp_recvd_pkt.size() > 5)
                                       ? (tmp_recvd_pkt.size() - 5)
                                       : tmp_recvd_pkt.size();
    std::string recvd_pkt(tmp_recvd_pkt, 0, tmp_payload_len);
    
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

    std::string send_pkt(recvd_pkt);

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

int
ControllerAddRouteEntry(uint32_t dAddr,
                        uint16_t pLen,
                        uint32_t nAddr,
                        uint64_t mac,
                        uint16_t oPort)
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

    std::cout << "Write: Adding route table entry...";

    Utils utils;
    std::string dstAddr  = utils.uint2Str(dAddr);
    std::string nhAddr  = utils.uint2Str(nAddr);
    std::string macAddr  = utils.uint2Str(mac);
    std::string oPortStr = utils.uint2Str(oPort);

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
    lpm->set_value(dstAddr);
    lpm->set_prefix_len(pLen);

    auto table_action = table_entry->mutable_action();
    auto action = table_action->mutable_action();
    action->set_action_id(a_id);
    {
        auto param = action->add_params();
        param->set_param_id(p0_id);
        param->set_value(nhAddr);
    }
    {
        auto param = action->add_params();
        param->set_param_id(p1_id);
        // param->set_value(std::string("\x00\x09", 2));
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
ControllerAddVrfEntry(uint16_t etype,
                      std::string smacAddr, 
                      uint32_t addr,
                      uint32_t vrf)
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

    std::cout << "Write: Adding vrf table entry...";

    Utils utils;
    std::string ethType  = utils.uint2Str(etype);
    std::string dstAddr  = utils.uint2Str(addr);

    const uint16_t etMask = 0xffff;
    const uint32_t dAddrMask = 0xffffffff;
    const uint64_t sAddrMask = 0xffffffffffff;

    std::string ethTypeMask  = utils.uint2Str(etMask);
    std::string dstAddrMask  = utils.uint2Str(dAddrMask);
    //std::string smacAddrMask = utils.uint2Str(0xffffffffffff);

    char tsmacMask[6];
    memcpy(&tsmacMask[0], (char *) &sAddrMask, 6);
    std::string smacAddrMask((char *) &tsmacMask[0], 6);

    auto t_id = get_table_id(p4info, "ingress.vrf.vrf_classifier_table");
    auto mf0_id = get_mf_id(p4info,
                            "ingress.vrf.vrf_classifier_table",
                            "hdr.ethernet.ether_type");
    auto mf1_id = get_mf_id(p4info,
                            "ingress.vrf.vrf_classifier_table",
                            "hdr.ethernet.src_addr");
    auto mf2_id = get_mf_id(p4info,
                            "ingress.vrf.vrf_classifier_table",
                            "hdr.ipv4_base.dst_addr");

    p4::Entity entity;
    auto table_entry = entity.mutable_table_entry();

    table_entry->set_table_id(t_id);

    auto match0 = table_entry->add_match();
    match0->set_field_id(mf0_id);
    auto tnary0 = match0->mutable_ternary();
    tnary0->set_value(ethType);
    tnary0->set_mask(ethTypeMask);

    auto match1 = table_entry->add_match();
    match1->set_field_id(mf1_id);
    auto tnary1 = match1->mutable_ternary();
    tnary1->set_value(smacAddr);
    tnary1->set_mask(smacAddrMask);

    auto match2 = table_entry->add_match();
    match2->set_field_id(mf2_id);
    auto tnary2 = match2->mutable_ternary();
    tnary2->set_value(dstAddr);
    tnary2->set_mask(dstAddrMask);

    auto a_id = get_action_id(p4info, "ingress.vrf.set_vrf");
    auto p0_id = get_param_id(p4info, "ingress.vrf.set_vrf", "vrf_id");

    std::string vrfId = utils.uint2Str(vrf);

    auto table_action = table_entry->mutable_action();
    auto action = table_action->mutable_action();
    action->set_action_id(a_id);
    {
        auto param = action->add_params();
        param->set_param_id(p0_id);
        param->set_value(vrfId);
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
ControllerAddClassIdEntry(uint16_t vid,
                          uint8_t  pcp,
                          uint16_t etype,
                          uint8_t  proto,
                          uint8_t  ttl,
                          uint32_t dAddr,
                          uint16_t sPort,
                          uint16_t dPort,
                          uint8_t  classId)
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

    std::cout << "Write: Adding classId table entry...";

    Utils utils;
    std::string pcpS     = utils.uint2Str(pcp);
    std::string protoS   = utils.uint2Str(proto);
    std::string ttlS     = utils.uint2Str(ttl);
    std::string classIdS = utils.uint2Str(classId);
    std::string vidS     = utils.uint2Str(vid);
    std::string etypeS   = utils.uint2Str(etype);
    std::string dAddrS   = utils.uint2Str(dAddr);
    std::string sPortS   = utils.uint2Str(sPort);
    std::string dPortS   = utils.uint2Str(dPort);

    const uint16_t vidMask = 0x0fff;
    std::string vidMaskS   = utils.uint2Str(vidMask);

    const uint8_t pcpMask = 0x07;
    const uint8_t protoMask = 0xff;
    const uint8_t ttlMask = 0xff;
    const uint16_t etypeMask = 0xffff;
    const uint16_t sPortMask = 0xffff;
    const uint16_t dPortMask = 0xffff;
    const uint32_t dAddrMask = 0xffffffff;

    std::string pcpMaskS   = utils.uint2Str(pcpMask);
    std::string protoMaskS = utils.uint2Str(protoMask);
    std::string ttlMaskS   = utils.uint2Str(ttlMask);
    std::string etypeMaskS = utils.uint2Str(etypeMask);
    std::string sPortMaskS = utils.uint2Str(sPortMask);
    std::string dPortMaskS = utils.uint2Str(dPortMask);
    std::string dAddrMaskS = utils.uint2Str(dAddrMask);

    auto t_id = get_table_id(p4info,
                             "ingress.class_id.class_id_assignment_table");

    auto mf0_id = get_mf_id(p4info,
                            "ingress.class_id.class_id_assignment_table",
                            "hdr.vlan_tag[0].vid");
    auto mf1_id = get_mf_id(p4info,
                            "ingress.class_id.class_id_assignment_table",
                            "hdr.vlan_tag[0].pcp");
    auto mf2_id = get_mf_id(p4info,
                            "ingress.class_id.class_id_assignment_table",
                            "hdr.ethernet.ether_type");
    auto mf3_id = get_mf_id(p4info,
                            "ingress.class_id.class_id_assignment_table",
                            "hdr.ipv4_base.ttl");
    auto mf4_id = get_mf_id(p4info,
                            "ingress.class_id.class_id_assignment_table",
                            "hdr.ipv4_base.protocol");
    auto mf5_id = get_mf_id(p4info,
                            "ingress.class_id.class_id_assignment_table",
                            "hdr.ipv4_base.dst_addr");
    auto mf6_id = get_mf_id(p4info,
                            "ingress.class_id.class_id_assignment_table",
                            "local_metadata.l4_src_port");
    auto mf7_id = get_mf_id(p4info,
                            "ingress.class_id.class_id_assignment_table",
                            "local_metadata.l4_dst_port");

    p4::Entity entity;
    auto table_entry = entity.mutable_table_entry();

    table_entry->set_table_id(t_id);

    auto match0 = table_entry->add_match();
    match0->set_field_id(mf0_id);
    auto tnary0 = match0->mutable_ternary();
    tnary0->set_value(vidS);
    tnary0->set_mask(vidMaskS);

    auto match1 = table_entry->add_match();
    match1->set_field_id(mf1_id);
    auto tnary1 = match1->mutable_ternary();
    tnary1->set_value(pcpS);
    tnary1->set_mask(pcpMaskS);

    auto match2 = table_entry->add_match();
    match2->set_field_id(mf2_id);
    auto exact2 = match2->mutable_exact();
    exact2->set_value(etypeS);

    auto match3 = table_entry->add_match();
    match3->set_field_id(mf3_id);
    auto tnary3 = match3->mutable_ternary();
    tnary3->set_value(ttlS);
    tnary3->set_mask(ttlMaskS);

    auto match4 = table_entry->add_match();
    match4->set_field_id(mf4_id);
    auto tnary4 = match4->mutable_ternary();
    tnary4->set_value(protoS);
    tnary4->set_mask(protoMaskS);

    auto match5 = table_entry->add_match();
    match5->set_field_id(mf5_id);
    auto tnary5 = match5->mutable_ternary();
    tnary5->set_value(dAddrS);
    tnary5->set_mask(dAddrMaskS);

    auto match6 = table_entry->add_match();
    match6->set_field_id(mf6_id);
    auto tnary6 = match6->mutable_ternary();
    tnary6->set_value(sPortS);
    tnary6->set_mask(sPortMaskS);

    auto match7 = table_entry->add_match();
    match7->set_field_id(mf7_id);
    auto tnary7 = match7->mutable_ternary();
    tnary7->set_value(dPortS);
    tnary7->set_mask(dPortMaskS);

    auto a_id = get_action_id(p4info, "ingress.class_id.set_class_id");
    auto p0_id = get_param_id(p4info,
                              "ingress.class_id.set_class_id",
                              "class_id_value");

    auto table_action = table_entry->mutable_action();
    auto action = table_action->mutable_action();
    action->set_action_id(a_id);
    {
        auto param = action->add_params();
        param->set_param_id(p0_id);
        param->set_value(classIdS);
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

int ControllerAddMyMacEntry(std::string mac)
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

    std::cout << "Write: Adding MyMac table entry...";

    std::string t_name = "ingress.l3_admit.dst_mac_classifier_table";
    auto t_id = get_table_id(p4info, t_name);
    auto mf0_id = get_mf_id(p4info, t_name, "hdr.ethernet.dst_addr");

    p4::Entity entity;
    auto table_entry = entity.mutable_table_entry();

    table_entry->set_table_id(t_id);

    auto match0 = table_entry->add_match();
    match0->set_field_id(mf0_id);
    auto exact0 = match0->mutable_exact();
    exact0->set_value(mac);

    auto a_id = get_action_id(p4info, "ingress.l3_admit.set_l3_admit");
    auto table_action = table_entry->mutable_action();
    auto action = table_action->mutable_action();
    action->set_action_id(a_id);

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

int ControllerAddRtEncapEntry(uint32_t    dAddr,
                              uint16_t    pLen,
                              uint32_t    vrf,
                              uint64_t    smac,
                              uint64_t    dmac,
                              uint16_t    oPort,
                              uint8_t     l3ClassId,
                              std::string type)
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

    std::cout << "Write: Adding RtEncap table entry...";

    Utils utils;
    std::string dAddrS     = utils.uint2Str(dAddr);
    std::string vrfS       = utils.uint2Str(vrf);
    std::string smacS      = utils.uint2Str(smac);
    std::string dmacS      = utils.uint2Str(dmac);
    std::string oPortS     = utils.uint2Str(oPort);
    std::string l3ClassIdS = utils.uint2Str(l3ClassId);

    std::string t_name;
    if (type == "override")
        t_name = std::string("ingress.l3_fwd.l3_ipv4_override_table");
    else if (type == "vrf")
        t_name = std::string("ingress.l3_fwd.l3_ipv4_vrf_table");
    else if (type == "fallback")
        t_name = std::string("ingress.l3_fwd.l3_ipv4_fallback_table");

    auto t_id = get_table_id(p4info, t_name);
    p4::Entity entity;
    auto table_entry = entity.mutable_table_entry();
    table_entry->set_table_id(t_id);

    auto mf0_id = get_mf_id(p4info, t_name, "hdr.ipv4_base.dst_addr");
    auto match0 = table_entry->add_match();
    match0->set_field_id(mf0_id);
    auto lpm0 = match0->mutable_lpm();
    lpm0->set_value(dAddrS);
    lpm0->set_prefix_len(pLen);

    if (type == "vrf") {
        auto mf1_id = get_mf_id(p4info, t_name, "local_metadata.vrf_id");
        auto match1 = table_entry->add_match();
        match1->set_field_id(mf1_id);
        auto exact1 = match1->mutable_exact();
        exact1->set_value(vrfS);
    }

    std::string a_name("ingress.l3_fwd.set_nexthop");
    auto a_id = get_action_id(p4info, a_name);
    auto table_action = table_entry->mutable_action();
    auto action = table_action->mutable_action();
    action->set_action_id(a_id);

    {
        auto param = action->add_params();
        auto p0_id = get_param_id(p4info, a_name, "port");
        param->set_param_id(p0_id);
        param->set_value(oPortS);
    }

    {
        auto param = action->add_params();
        auto p1_id = get_param_id(p4info, a_name, "smac");
        param->set_param_id(p1_id);
        param->set_value(smacS);
    }

    {
        auto param = action->add_params();
        auto p2_id = get_param_id(p4info, a_name, "dmac");
        param->set_param_id(p2_id);
        param->set_value(dmacS);
    }

    {
        auto param = action->add_params();
        auto p3_id = get_param_id(p4info, a_name, "l3_class_id");
        param->set_param_id(p3_id);
        param->set_value(l3ClassIdS);
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

int ControllerAddPuntEntry(uint32_t iport,
                           uint32_t oport,
                           uint16_t etype,
                           uint64_t dmac,
                           uint8_t  tos,
                           uint8_t  ttl,
                           uint32_t saddr,
                           uint32_t daddr,
                           uint8_t  proto,
                           uint32_t arpTAddr,
                           uint8_t  icmpType,
                           uint16_t vid,
                           uint8_t  pcp,
                           uint8_t  iClassId,
                           uint32_t vrfId,
                           uint8_t qId)
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

    std::cout << "Write: Adding punt table entry...";

    auto t_name = "ingress.punt.punt_table";
    auto t_id = get_table_id(p4info, t_name);
    p4::Entity entity;
    auto table_entry = entity.mutable_table_entry();
    table_entry->set_table_id(t_id);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "standard_metadata.ingress_port", iport, 32);

#ifdef SUD
    // egress_spec falls in egress pipeline?
    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "standard_metadata.egress_spec", oport, 32);
#endif // SUD

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.ethernet.ether_type", etype, 16);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.ethernet.dst_addr", dmac, 64);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.ipv4_base.diffserv", tos, 8);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.ipv4_base.ttl", ttl, 8);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.ipv4_base.src_addr", saddr, 32);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.ipv4_base.dst_addr", daddr, 32);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.ipv4_base.protocol", proto, 8);

#ifdef LATER
    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.arp.target_proto_addr", arpTAddr, 32);
#endif // LATER

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.icmp.type", icmpType, 8);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.vlan_tag[0].vid", vid, 16);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "hdr.vlan_tag[0].pcp", pcp, 8);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "local_metadata.class_id", iClassId, 8);

    addTEntryMF(p4info, t_name, table_entry, "ternary",
                "local_metadata.vrf_id", vrfId, 32);

    std::string a_name = "ingress.punt.set_queue_and_clone_to_cpu";
    auto a_id = get_action_id(p4info, a_name);
    auto table_action = table_entry->mutable_action();
    auto action = table_action->mutable_action();
    action->set_action_id(a_id);

    addTEntryAction(p4info, a_name, action, "queue_id", qId);

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
