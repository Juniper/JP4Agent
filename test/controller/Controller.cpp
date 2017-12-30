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
#include <cstring>
#include <cassert>
#include <unistd.h>

#include "Utils.h"

using grpc::ClientContext;
using grpc::Status;

using google::protobuf::util::MessageDifferencer;

constexpr char test_json[] = "/root/JP4Agent/test/controller/testdata/afi_switch.json";
constexpr char test_proto_txt[] = "/root/JP4Agent/test/controller/testdata/simple_router.proto.txt";

// cpu header
struct __attribute__((packed)) cpu_header_t {
    char zeros[8];
    uint16_t reason;
    uint16_t port;
};

static constexpr size_t cpu_hdr_sz = sizeof(struct cpu_header_t);

int
ControllerInjectL2Pkt(const std::string &l2_pkt, uint16_t egress_port)
{
    // Create gRPC stub and open the stream channel
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());

    std::unique_ptr<p4::P4Runtime::Stub> pi_stub{
        p4::P4Runtime::NewStub(channel)};

    ClientContext stream_context;
    auto stream = pi_stub->StreamChannel(&stream_context);

    // Encapsulate L2 pkt with cpu_header
    cpu_header_t cpu_hdr;
    constexpr size_t cpu_hdr_sz = sizeof(cpu_hdr);
    memset(&cpu_hdr, 0, cpu_hdr_sz);
    cpu_hdr.port = htons(egress_port);

    std::string payload(cpu_hdr_sz + l2_pkt.size(), '\0');
    memcpy(&payload[0], &cpu_hdr, cpu_hdr_sz);
    memcpy(&payload[cpu_hdr_sz], l2_pkt.data(), l2_pkt.size());

    // Inject L2 pkt on the stream
    p4::StreamMessageRequest request;
    request.mutable_packet()->set_payload(payload);
    stream->WriteLast(request, grpc::WriteOptions());

    // Close stream channel
    auto status = stream->Finish();
    if (!status.ok()) {
        std::cout << "Failed to close stream.\n";
        return -1;
    }

    return 0;
}

void ControllerPuntPkt(std::string &l2_pkt, uint16_t &ingress_port)
{
    // Create gRPC stub and open the stream channel
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::InsecureChannelCredentials());

    std::unique_ptr<p4::P4Runtime::Stub> pi_stub{
        p4::P4Runtime::NewStub(channel)};

    ClientContext stream_context;
    auto stream = pi_stub->StreamChannel(&stream_context);

    std::string recvd_pkt;
    // Listen for L2 pkt on the stream channel
    p4::StreamMessageResponse response;
    stream->Read(&response);
    if (response.update_case() != p4::StreamMessageResponse::kPacket) {
        return;
    }
    recvd_pkt = response.packet().payload();

    // Decapsulate cpu_header
    char zero[8]{};
    if ((recvd_pkt.size() <= cpu_hdr_sz) ||
        (memcmp(zero, recvd_pkt.data(), 8) != 0)) {
        return;
    }

    ingress_port = ntohs(((struct cpu_header_t *)recvd_pkt.data())->port);
    std::cout << "Received L2 pkt (size: " << recvd_pkt.size() - cpu_hdr_sz
              << " bytes) on ingress port " << ingress_port << "\n";

    // Copy L2 pkt payload alone
    l2_pkt.clear();
    l2_pkt.append(&recvd_pkt[cpu_hdr_sz], recvd_pkt.size() - cpu_hdr_sz);

    // Close stream channel
    stream->WritesDone();
    auto status = stream->Finish();
    if (!status.ok()) {
        std::cout << "Failed to close stream.\n";
    }
}

int
TestCaseSimpleRouter() {
  int dev_id = 0;

  auto channel = grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials());
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
    assert(response.update_case() == p4::StreamMessageResponse::kArbitration);
    assert(response.arbitration().status().code() == ::google::rpc::Code::OK);
  }

  std::cout<< "SetForwardingPipelineConfigRequest: Setting forwarding topo...\n";

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
    auto status = pi_stub_->SetForwardingPipelineConfig(
        &context, request, &rep);
    assert(status.ok());
    config->release_p4info();
  }

#if 0
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
  //lpm->set_value(std::string("\x0a\x00\x00\x01", 4));  // 10.0.0.1
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
    //param->set_value(std::string("\x00\x09", 2));
    param->set_value(std::string("\x00\x01", 2));
  }

  sleep(5);

  std::cout<< "Write: Adding route table entry...\n";

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
    while (stream->Read(&response)) { }
    auto status = stream->Finish();
    assert(status.ok());
  }

  return 0;
}
