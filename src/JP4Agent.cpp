//
// Juniper P4 Agent
//
/// @file  JP4Agent.cpp
/// @brief Juniper P4 Agent
//
// Created by Sandesh Kumar Sodhi, November 2017
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

#include <arpa/inet.h>
#include <algorithm>
#include <cstddef>
#include <mutex>
#include "JP4Agent.h"


JP4Agent *jP4Agent;

// This connection represents the bi-directional streaming channel between the
// controller and the JP4Agent. For now, assume that there is only one
// controller connected at a time.
class ControllerConnection
{
  public:
    void set_stream(StreamChannelReaderWriter *stream)
    {
        std::lock_guard<std::mutex> lock{scm};
        stream_ = stream;
    }
    void clear_stream()
    {
        std::lock_guard<std::mutex> lock{scm};
        stream_ = nullptr;
    }

    // Send pkt on the stream channel to the controller.
    bool send_pkt_in(p4::PacketIn *pkt) const
    {
        bool pkt_sent = false;
        std::lock_guard<std::mutex> lock{scm};
        if (stream_) {
            p4::StreamMessageResponse response;
            response.set_allocated_packet(pkt);
            stream_->Write(response);
            response.release_packet();
            pkt_sent = true;
        }
        return pkt_sent;
    }

  private:
    mutable std::mutex scm;  // Guards access to stream channel ptr.
    StreamChannelReaderWriter *stream_{nullptr};
};

static ControllerConnection controller_conn;

Status 
P4RuntimeServiceImpl::SetForwardingPipelineConfig(
      ServerContext *context,
      const p4::SetForwardingPipelineConfigRequest *request,
      p4::SetForwardingPipelineConfigResponse *rep)
{
    if (_debugmode.find("debug-pi") != std::string::npos) {
		Log(DEBUG) << "P4Runtime SetForwardingPipelineConfig\n";
		//Log(DEBUG) << request->DebugString();
    }
    (void) rep;

    p4::SetForwardingPipelineConfigRequest_Action a  = request->action();
    p4::ForwardingPipelineConfig config = request->config();

    p4::config::P4Info p4info_proto = config.p4info();

    if (_debugmode.find("debug-pi") != std::string::npos) {

		Log(DEBUG) << "action:" << a;
		Log(DEBUG) << "request->configs_size():" << request->configs_size();
        Log(DEBUG) << p4info_proto.DebugString();
		Log(DEBUG) << "________ P4 INFO __________";

		Log(DEBUG) << "p4info_proto.tables_size():" << p4info_proto.tables_size();
		Log(DEBUG) << "p4info_proto.actions_size():" << p4info_proto.actions_size();
    }
    Log(DEBUG) << p4info_proto.DebugString();

    const auto &actions = p4info_proto.actions();
    for (const auto &action : actions) {
		const auto &pre = action.preamble();
		if (_debugmode.find("debug-pi") != std::string::npos) {
			Log(DEBUG) << "_________ Action _______";
			Log(DEBUG) << "Id:" << pre.id();
			Log(DEBUG) << "Name:" << pre.name();
        }

        P4InfoResourcePtr res(new P4InfoAction(action));
        _p4Info.insert2IdMap(res);
        _p4Info.insert2NameMap(res);
    }

    const auto &tables = p4info_proto.tables();
	Log(DEBUG) << "tables.size():" << tables.size();
    for (const auto &table : tables) {
        const auto &pre = table.preamble();
		if (_debugmode.find("debug-pi") != std::string::npos) {
			Log(DEBUG) << "_________ Table _______";
			Log(DEBUG) << "Id:" << pre.id();
			Log(DEBUG) << "Name:" << pre.name();
        }

        P4InfoResourcePtr res(new P4InfoTable(table));
        _p4Info.insert2IdMap(res);
        _p4Info.insert2NameMap(res);
    }

    p4::tmp::P4DeviceConfig p4_device_config;
    if (!p4_device_config.ParseFromString(config.p4_device_config())) {
        Log(ERROR) << "Invalid 'p4_device_config', not an instance of " <<
                     "p4::tmp::P4DeviceConfig";
    }    

    const auto &device_data = p4_device_config.device_data();

    std::string dd_str = std::string((char *)device_data.data(), device_data.size());

    if (_debugmode.find("debug-pi") != std::string::npos) {
		Log(DEBUG) << "device_data.data():" << device_data.data();
		Log(DEBUG) << "device_data.size():" << device_data.size();
		Log(DEBUG) << "_____ JASON ________\n";
		Log(DEBUG) << dd_str;
    }

    std::istringstream ss(dd_str);
    std::istream *is = &ss;

    Json::Value cfg_root;
    (*is) >> cfg_root;

    _afiDevCfg.populateAfiObjects(cfg_root);
    _afiDevCfg.bindAfiObjects();
    return Status::OK;
}

Status
P4RuntimeServiceImpl::tableInsert(const p4::TableEntry &tableEntry)
{

    P4InfoResourceId actionId;

    const auto tableId = tableEntry.table_id();
    const auto priority= tableEntry.priority();
    Log(DEBUG) << "tableInsert: tableId: "<< tableId;
    Log(DEBUG) << "tableInsert: priority: "<< priority;


    Status status = Status::OK;

    if (tableEntry.is_default_action()) {
      if (!tableEntry.match().empty()) {
          Log(ERROR) << "Default tableEntry has non-empty key";
          //TBD: Return error status and translate it to
          // grpc status
      }
      if (tableEntry.priority() != 0) {
          Log(ERROR) << "Default tableEntry has non-zero priority";
      }
      return status;
    }

    Log(DEBUG) << "tableInsert: match size: "<< static_cast<size_t>(tableEntry.match().size());


    char prefix_bytes[4];
    int num_prefix_bytes = 0;
    int prefix_len = 0;


    for (const auto &mf : tableEntry.match()) {
        Log(DEBUG) << "match field id: "<< mf.field_id();

            if (mf.has_valid()) {
      		    Log(DEBUG) << "match field has valid: ";
            }
            if (mf.has_exact()) {
      		    Log(DEBUG) << "match field has exact: ";
            }
            if (mf.has_lpm()) {
      		    Log(DEBUG) << "match field has lpm: ";

                auto lpm = mf.lpm();

                const std::string &keystr = lpm.value();
      		    Log(DEBUG) << "keystr.size(): " << keystr.size();
      		    Log(DEBUG) << "keystr: " << keystr;
      		    Log(DEBUG) << "bytes: ";
                char const *c = keystr.c_str();
                std::cout << int(c[0]) << "." << int(c[1]) << "." << int(c[2]) << "." << int(c[3]) << "\n"; 
                memcpy(prefix_bytes, c, keystr.size());
                num_prefix_bytes = keystr.size();

                std::cout << static_cast<uint8_t>(keystr[0]);
                std::cout << static_cast<uint8_t>(keystr[1]);
                std::cout << static_cast<uint8_t>(keystr[2]);
                std::cout << static_cast<uint8_t>(keystr[3]);


                int pLen = lpm.prefix_len();
                //lpm->set_prefix_len(pLen);
      		    Log(DEBUG) << "pLen: " << pLen; 
                prefix_len = pLen;

            }
            if (mf.has_ternary()) {
      		    Log(DEBUG) << "match field has ternary: ";
            }
            if (mf.has_range()) {
      		    Log(DEBUG) << "match field has range: ";
            }
    }

    uint16_t portId = 0;
    const p4::TableAction &tableAction = tableEntry.action();

    switch (tableAction.type_case()) {
        case p4::TableAction::kAction:
              {
				  Log(DEBUG) << "tableAction : Action ";
				  const p4::Action &action = tableAction.action();
                  actionId = tableAction.action().action_id();
				  Log(DEBUG) << "actionId: "<< actionId;

                  P4InfoResourcePtr resAction = _p4Info.p4InfoResource(actionId);
                  P4InfoActionPtr p4InfoAction = std::dynamic_pointer_cast<P4InfoAction>(resAction);

                  p4InfoAction->display();

				  for (const auto &p : action.params()) {
				      Log(DEBUG) << "paramt id: "<< p.param_id();


                      const char *c = p.value().data();

                      size_t s = p.value().size();
				      Log(DEBUG) << "bytes size: "<< s;

                      for (size_t i =0 ; i < s ; i++) {
                           std::cout << int(c[i]) << "\n"; 
                      }

                      std::string paramName = p4InfoAction->actionParamName(p.param_id());

                      if (paramName.compare("port") == 0) {
                          assert(p.value().size() == 2);
                          portId = ntohs(*(uint16_t *)(c)); 
                          //std::cout<<"portId :" << portId << "\n";
                      }
				  }

              }
              break;
        case p4::TableAction::kActionProfileMemberId:
        	  Log(DEBUG) << "tableAction : ActionProfileMemberId ";
              break;
        case p4::TableAction::kActionProfileGroupId:
        	  Log(DEBUG) << "tableAction : ActionProfileGroupId";
              break;
        default:
        	  Log(ERROR) << "tableAction : error";
              break;
    }


    P4InfoResourcePtr resTable = _p4Info.p4InfoResource(tableId);
    P4InfoTablePtr p4InfoTable = std::dynamic_pointer_cast<P4InfoTable>(resTable);

    p4InfoTable->display();


    P4InfoResourcePtr resAction = _p4Info.p4InfoResource(actionId);
    P4InfoActionPtr p4InfoAction = std::dynamic_pointer_cast<P4InfoAction>(resAction);

    p4InfoAction->display();


    AfiObjectPtr afiObjPtr = _afiDevCfg.getAfiTree(p4InfoTable->name());

    AfiTreePtr afiTreePtr = std::dynamic_pointer_cast<AfiTree>(afiObjPtr);


    std::cout<<"afiTree :" << afiTreePtr << "\n";

    AftNodeToken afiTreeToken = 0;
    if (afiTreePtr != nullptr) {
        afiTreeToken = afiTreePtr->token();
    }
    std::cout<<"afiTreePtr->token() :" << afiTreeToken << "\n";

    std::cout<<"portId              :" << portId << "\n";

    AftNodeToken outputPortToken = jP4Agent->afiClient().outputPortToken(portId);
    std::cout<<"outputPortToken:" << outputPortToken << "\n";

    AftNodeToken etherEncapToken = jP4Agent->afiClient().addEtherEncapNode(
                                        "32:26:0a:2e:ff:f1",
                                        "5e:d8:f9:32:bd:85",
                                         outputPortToken);

    std::cout<<"etherEncapToken:" << etherEncapToken << "\n";
    //jP4Agent->afiClient().addRoute(afiTreeToken, "1.1.1.1/10", etherEncapToken);
    std::cout<<"Adding route...\n";
    for (int i =0; i < num_prefix_bytes; i++) {
         std::cout << int(prefix_bytes[i]) << "." "\n"; 
    }
    jP4Agent->afiClient().addRoute(afiTreeToken, prefix_bytes, num_prefix_bytes, prefix_len, etherEncapToken);

    return status;
}


Status
P4RuntimeServiceImpl::tableWrite(p4::Update_Type update,
                                 const p4::TableEntry &table_entry)
{
    Log(DEBUG) << "tableWrite: table_id: "<< table_entry.table_id();
    //if (!check_p4_id(table_entry.table_id(), P4ResourceType::TABLE))
    //  return make_invalid_p4_id_status();
    if (table_entry.has_meter_config() || table_entry.has_counter_data()) {
      Log(DEBUG) << "Direct resources not supported in TableEntry yet: ";
      return Status::OK;
    }    
    //Status status;
    Status status = Status::OK;
    switch (update) {
      case p4::Update_Type_UNSPECIFIED:
        Log(DEBUG) << "p4::Update_Type_UNSPECIFIED";
        break;
      case p4::Update_Type_INSERT:
        Log(DEBUG) << "p4::Update_Type_INSERT";
        return tableInsert(table_entry);
        break;
      case p4::Update_Type_MODIFY:
        Log(DEBUG) << "p4::Update_Type_MODIFY";
        break;
      case p4::Update_Type_DELETE:
        Log(DEBUG) << "p4::Update_Type_DELETE";
        break;
      default:
        Log(DEBUG) << "tableWrite: ____ default";
        break;
    }    
    return status;
}


Status
P4RuntimeServiceImpl::_write(const p4::WriteRequest &request)
{
    Status status = Status::OK;
    //status.set_code(Code::OK);
    for (const auto &update : request.updates()) {
      const auto &entity = update.entity();
      switch (entity.entity_case()) {
        case p4::Entity::kExternEntry:
          Log(DEBUG) << "p4::Entity::kExternEntry";
          break;
        case p4::Entity::kTableEntry:
          Log(DEBUG) << "p4::Entity::kTableEntry";
          status = tableWrite(update.type(), entity.table_entry());
          break;
        case p4::Entity::kActionProfileMember:
          Log(DEBUG) << "p4::Entity::kActionProfileMember";
          break;
        case p4::Entity::kActionProfileGroup:
          Log(DEBUG) << "p4::Entity::kActionProfileGroup";
          break;
        case p4::Entity::kMeterEntry:
          Log(DEBUG) << "p4::Entity::kMeterEntry";
          break;
        case p4::Entity::kDirectMeterEntry:
          Log(DEBUG) << "p4::Entity::kDirectMeterEntry";
          break;
        case p4::Entity::kCounterEntry:
          Log(DEBUG) << "p4::Entity::kCounterEntry";
          break;
        case p4::Entity::kDirectCounterEntry:
          Log(DEBUG) << "p4::Entity::kDirectCounterEntry";
          break;
        default:
          Log(DEBUG) << "_____default";
          break;
      }
      //if (status.code() != Code::OK) break;
    }
    return status;
}


Status
P4RuntimeServiceImpl::Write(ServerContext *context,
                            const p4::WriteRequest *request,
                            p4::WriteResponse *rep)
{

    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "_____ P4Runtime Write _____\n";
        Log(DEBUG) << request->DebugString();
    }
    Log(DEBUG) << "_____ P4Runtime Write _____\n";
    //Log(DEBUG) << request->DebugString();
    (void) rep;

    auto deviceId = request->device_id();
    Log(DEBUG) << "Device id :" << deviceId;
    if(request->has_election_id()) {
       Log(DEBUG) << "Request has election id";
    }
    auto electionId = convert_u128(request->election_id());
    //Log(DEBUG) << "Election id :" << static_cast<int>(electionId);
    std::cout << "Election id :" << electionId << "\n";

    auto status = _write(*request);
    //return Status::OK;
    return status;
}

Status
P4RuntimeServiceImpl::Read(ServerContext *context,
              const p4::ReadRequest *request,
              ServerWriter<p4::ReadResponse> *writer)
{
    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "_____ P4Runtime Read _____\n";
        Log(DEBUG) << request->DebugString();
    }
    p4::ReadResponse response;
    return Status::OK;
}


Status
P4RuntimeServiceImpl::GetForwardingPipelineConfig(
              ServerContext *context,
              const p4::GetForwardingPipelineConfigRequest *request,
      p4::GetForwardingPipelineConfigResponse *rep)
{
    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "_____ P4Runtime GetForwardingPipelineConfig _____\n";
        Log(DEBUG) << request->DebugString();
    }
    (void) rep;
    return Status::OK;
}

Status
P4RuntimeServiceImpl::StreamChannel(ServerContext *context,
				    StreamChannelReaderWriter *stream)
{
    // Update the handle to the StreamChannelReaderWriter.
    controller_conn.set_stream(stream);

    p4::StreamMessageRequest request;
    while (stream->Read(&request)) {
        switch (request.update_case()) {
            case p4::StreamMessageRequest::kArbitration:
            {
                if (_debugmode.find("debug-pi") != std::string::npos) {
                    Log(DEBUG) << "p4::StreamMessageRequest::kArbitration\n";
                }
                auto device_id = request.arbitration().device_id();
                auto election_id =
                    convert_u128(request.arbitration().election_id());
                if (_debugmode.find("debug-pi") != std::string::npos) {
                    Log(DEBUG) << "device_id:" << device_id;
                    Log(DEBUG) << "election_id:" << election_id;
                }
                p4::StreamMessageResponse response;
                auto arbitration = response.mutable_arbitration();
                stream->Write(response);
            }
            break;

            case p4::StreamMessageRequest::kPacket:
            {
                if (_debugmode.find("debug-pi") != std::string::npos) {
                    Log(DEBUG) << "p4::StreamMessageRequest::kPacket\n";
                }
                const std::string &payload = request.packet().payload();

                // Received L2 pkt. Send to the device on the UDP socket.
                jP4Agent->send_packet_out(payload);
            }
            break;

            default:
                break;
        }
    }

    // This stream channel is closing. Clear handle now.
    controller_conn.clear_stream();
    return Status::OK;
}

//
// @fn
// piServer
//
// @brief
// PI server
//
// @param[in] void
// @return void
//

void
JP4Agent::piServer(void)
{
    PIGrpcServerRun();
    PIGrpcServerWait();
}

//
// @fn
// startAfiPktRcvr
//
// @brief
// Start AFI packer receiver
//
// @param[in] void
// @return void
//

void
JP4Agent::startPIServer(void)
{
    std::thread piSrvr( [this] { this->piServer(); } );
    piSrvr.detach();
}

//
// @fn
// recvHostPathPacket
//
// @brief
// Receive hostpath packet
//
// @param[in]
//     pkt Aft packet where received packet will be copied to
// @return 0 - Success, -1 - Error
//

int 
JP4Agent::recvHostPathPacket()
{
    enum { max_length = 2000 };
    char _data[max_length];
    BOOST_UDP::endpoint sender_endpoint;

    // Block until data has been received successfully or an error occurs.
    const size_t recvlen = _hpUdpSock.receive_from(
        boost::asio::buffer(_data, max_length), sender_endpoint);
    if (recvlen == 0) {
        std::cout << "Read empty packet!!\n";
        return 0;
    }

    // Allocate packet context *and* buffer
    AftPacketPtr pkt =
        AftPacket::createReceive(recvlen - AftPacket::_headerSize);

    pktTrace("Received (hostpath) pkt ", _data, recvlen);

    std::copy_n(_data, recvlen, pkt->header());

    pktTrace("packet header", (char *)(pkt->header()), pkt->headerSize());

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "pkt->headerSize(): " << pkt->headerSize() << " bytes" << std::endl;
    std::cout << "Header: Received " << recvlen << " bytes" << std::endl;

    pkt->headerParse();

    std::cout << "Received packet:" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "Sandbox Id : " << pkt->sandboxId() << std::endl;
    std::cout << "Port Index : " << pkt->portIndex() << std::endl;
    std::cout << "Data Size  : " << pkt->dataSize()  << std::endl;

    std::cout << "pkt->dataSize(): " << pkt->dataSize() << " bytes" << std::endl;

    std::cout << "Data: Received " << recvlen << " bytes" << std::endl;

    pktTrace("pkt data", (char *)(pkt->data()), pkt->dataSize());

    // Construct pkt with cpu header
    cpu_header_t cpu_hdr;
    constexpr size_t cpu_hdr_sz = sizeof(cpu_hdr);
    memset(&cpu_hdr, 0, cpu_hdr_sz);
    cpu_hdr.port = htons(pkt->portIndex());

    std::string payload(cpu_hdr_sz + pkt->dataSize(), '\0');
    memcpy(&payload[0], &cpu_hdr, cpu_hdr_sz);
    memcpy(&payload[cpu_hdr_sz], pkt->data(), pkt->dataSize());

    // Punt it to the controller on the stream channel
    p4::PacketIn packet_in;
    packet_in.set_payload(payload);
    bool result = controller_conn.send_pkt_in(&packet_in);
    if (!result) {
        std::cout << "Failed to send pkt to master controller. No stream.\n";
    }

    return 0;
}

//
// @fn
// hostPathUDPServer
//
// @brief
// Hostpath UDP server
//
// @param[in] void
// @return void
//

void
JP4Agent::hostPathUDPServer(void)
{
    Log(DEBUG) << "Listening for hostpath packets on (UDP) 0.0.0.0:" << _hpUdpPort;
    while (true) {
        recvHostPathPacket();
    }
}

//
// @fn
// startAfiPktRcvr
//
// @brief
// Start AFI packer receiver
//
// @param[in] void
// @return void
//

void
JP4Agent::startAfiPktRcvr(void)
{
    std::thread udpSrvr( [this] { this->hostPathUDPServer(); } );
    udpSrvr.detach();
}

void
JP4Agent::send_packet_out(const std::string &pkt)
{
    constexpr auto cpu_hdr_sz = sizeof(cpu_header_t);
    const auto in_pkt_sz = pkt.size();

    // Sanity check
    char zero[8]{};
    if ((in_pkt_sz <= cpu_hdr_sz) || (memcmp(zero, pkt.data(), 8) != 0)) {
        std::cout << "Malformed packet!!\n";
        return;
    }

    // XXX: Should we convert to host order?
    uint16_t egress_port = ntohs(((struct cpu_header_t *)pkt.data())->port);

    // XXX: For now, use sandbox ID 0
    injectL2Packet(0, egress_port, (uint8_t *)&pkt[cpu_hdr_sz],
                   (in_pkt_sz - cpu_hdr_sz));
}

//
// @fn
// injectL2Packet
//
// @brief
// Inject layer 2 packet on specified (output) 
// port of specified sandbox
//
// @param[in] 
//     sandboxId Sandbox index
// @param[in] 
//     portIndex Output port index
// @param[in] 
//     l2Packet Pointer to layer 2 packet to be injected
// @param[in] 
//     l2PacketLen Length of layer 2 packet
// @return void
//

int
JP4Agent::injectL2Packet(AftSandboxId  sandboxId,
                         AftIndex      portIndex,
                         const uint8_t *l2Packet,
                         int           l2PacketLen)
{
    std::cout << "Injecting layer2 packet - " ;
    std::cout << "Sandbox index:" << sandboxId  <<" ";
    std::cout << "Port index: "<< portIndex << std::endl;

    if (!l2Packet) {
        std::cout << "l2Packet NULL" << std::endl;
    }

    AftPacketPtr pkt = AftPacket::createTransmit(
                                       l2PacketLen, sandboxId,
                                       portIndex, AftPacket::PacketTypeL2);

    //
    // Get base of packet data.
    // This is in the buffer immediately after the header.
    //
    uint8_t *pktData = pkt->data();

    //
    // Fill the pktData with 'L2 packet to be injected'
    //
    memcpy(pktData, l2Packet, l2PacketLen);

    pktTrace("xmit pkt ", (char *)(pkt->header()), pkt->size());

    std::cout << __PRETTY_FUNCTION__
              << ": Injecting pkt of size: " << pkt->size() << "\n";

    _hpUdpSock.send_to(boost::asio::buffer(pkt->header(), pkt->size()),
                       _pktIOEndpoint);
    return 0;
}

void 
JP4Agent::PIGrpcServerRun()
{

    _piBuilder.AddListeningPort(_piServerAddr, grpc::InsecureServerCredentials());

    _piBuilder.RegisterService(&_piService);

    //builder.RegisterService(&server_data->gnmi_service);

    _piBuilder.SetMaxReceiveMessageSize(256*1024*1024);  // 256MB
    
    _piServer = _piBuilder.BuildAndStart();
    Log(DEBUG) << "PI Server listening on " << _piServerAddr << "\n";
}

void 
JP4Agent::PIGrpcServerWait()
{
    _piServer->Wait();
}

void
JP4Agent::PIGrpcServerShutdown()
{
    _piServer->Shutdown();
}

void JP4Agent::PIGrpcServerForceShutdown(int deadline_seconds)
{
    using clock = std::chrono::system_clock;
    auto deadline = clock::now() + std::chrono::seconds(deadline_seconds);
    _piServer->Shutdown(deadline);
}

//
// @fn
// handleCliCommand
//
// @brief
// Handle CLI command
//
// @param[in] 
//     command_str CLI command string
// @return void
//

void
JP4Agent::handleCliCommand(std::string const & command_str)
{
    std::vector<std::string> command_sub_strings;
    boost::split(command_sub_strings, command_str, boost::is_any_of("\t "));

    std::string command = command_sub_strings.at(0);

    std::vector<std::string> command_args;
    for(int t=0; t < command_sub_strings.size(); ++t){
        if ((t != 0) && (!command_sub_strings.at(t).empty())) {
            command_args.push_back(command_sub_strings.at(t));
        }
    }

    if (command_args.size() > 0) {
        std::string command_arg1 = command_args.at(0);
    }
    if (command_args.size() > 1) {
        std::string command_arg1 = command_args.at(1);
    }

    if ((command.compare("help") == 0) ||
        (command.compare("h")    == 0) ||
        (command.compare("?")    == 0)) {
        std::cout << "\tSupported commands:" << std::endl;
        std::cout << "\t cross-connect <port-name> <port-name>: Cross connect ports" << std::endl;
        std::cout << "\t inject-l2-pkt <sandbox-index> <port-index>: Inject layer 2 packet" << std::endl;
        std::cout << "\t add-ether-encap <src-mac> <dst-mac> <0 or inner-vlan-id> <0 or outer-vlan-id> <output-port-token>" << std::endl;
        std::cout << "\t set-input-port-next-node <port-index> <next-node-token>" << std::endl;
        std::cout << "\t add-route <rtt-token> <prefix> <next-node-token>" << std::endl;
        std::cout << "\t add-receive <receive-code> <context>" << std::endl;
        std::cout << "\t history " << std::endl;
        std::cout << "\t clear-history " << std::endl;
        std::cout << "\t quit/exit " << std::endl;

    } else if ((command.compare("cc") == 0) ||
                (command.compare("cross-connect") == 0)) {

        _afiClient.crossConnectPortsOp("xe-0/0/0:1", "xe-0/0/0:1", AfiClient::CC_ADD);
        return;
        if (command_args.size() != 2) {
            std::cout << "Please provide port names" << std::endl;
            std::cout << "Example: inject-l2-pkt port-0 port-1" << std::endl;
            return;
        }

        _afiClient.crossConnectPortsOp(command_args.at(0), command_args.at(1), AfiClient::CC_ADD);

    } else  if (command.compare("set-input-port-next-node") == 0) {
        if (command_args.size() != 2) {
            std::cout << "Please provide port index and node-token" << std::endl;
            std::cout << "Example: set-all-input-ports-next-node 0 10" << std::endl;
            return;
        }
        std::cout << "Attaching next token to input port" << std::endl;
        AftIndex inputPortIndex = std::strtoull(command_args.at(0).c_str(),NULL,0);
        AftNodeToken nextToken  = std::strtoull(command_args.at(1).c_str(),NULL,0);

        _afiClient.setInputPortNextNode(inputPortIndex, nextToken);

    } else if (command.compare("add-receive") == 0) {
        if (command_args.size() != 2) {
            std::cout << "Please provide receive-code and context (a number)" << std::endl;
            std::cout << "Example: add-receive 10 20" << std::endl;
            return;
        }

        uint32_t receiveCode = std::strtoull(command_args.at(0).c_str(), NULL, 0); // Sandbox ID
        uint64_t context  = std::strtoull(command_args.at(1).c_str(), NULL, 0);    // Port Index

        AftNodeToken recvToken = _afiClient.addReceiveNode(receiveCode, context);

        std::cout << "Recive node token: " << recvToken << std::endl;



    } else  if (command.compare("add-ether-encap") == 0) {
        if (command_args.size() != 3) {
            std::cout << "Please provide source mac, destination mac and output port token" << std::endl;
            std::cout << "Examples: " << std::endl;
            std::cout << "add-ether-encap 32:26:0a:2e:ff:f1 5e:d8:f9:32:bd:85 10" << std::endl;
            return;
        }
        std::cout << "Adding ether encap node" << std::endl;
        AftNodeToken nextToken = std::strtoull(command_args.at(2).c_str(),NULL,0);;

        AftNodeToken nhEncapToken = _afiClient.addEtherEncapNode(command_args.at(1), 
                                                      command_args.at(0), 
                                                      nextToken);

        std::cout << "Ether encap node token: " << nhEncapToken << std::endl;

    } else  if (command.compare("add-route") == 0) {
        if (command_args.size() != 3) {
            std::cout << "Please provide rtt token, route prefix and next-node-token" << std::endl;
            std::cout << "Example: add-route 10 103.30.60.1 100" << std::endl;
            return;
        }
        AftNodeToken rttToken = std::strtoull(command_args.at(0).c_str(), NULL, 0);
        AftNodeToken routeTragetToken = std::strtoull(command_args.at(2).c_str(), NULL, 0);

        _afiClient.addRoute(rttToken, command_args.at(1), routeTragetToken);

    } else if ((command.compare("pkt") == 0) ||
                (command.compare("inject-l2-pkt") == 0)) {
        if (command_args.size() != 2) {
            std::cout << "Please provide sandbox-index and port-index" << std::endl;
            std::cout << "Example: inject-l2-pkt 0 0" << std::endl;
            return;
        }
        AftSandboxId  sandboxId = std::strtoull(command_args.at(0).c_str(), NULL, 0); // Sandbox ID
        AftIndex      portIndex = std::strtoull(command_args.at(1).c_str(), NULL, 0);    // Port Index

#define PKT_BUFF_SIZE 1500
#define ETHERNET_PACKET_BUF_SIZE 5000
        // Mac tap1 a2:24:4f:ce:94:b4
        // Src IP : 103.30.70.1
        // Dst IP : 103.30.70.3
        char ethernet_pkt[ETHERNET_PACKET_BUF_SIZE] =
            "a224 4fce 94b4 3226 0a2e fff1 0800 4500"
            "0054 dacc 4000 4001 059c 671e 4601 671e"
            "4603 0800 5cba 492b 3942 ee7c 5658 0000"
            "0000 0a30 0b00 0000 0000 1011 1213 1415"
            "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
            "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
            "3637";

        char pktBuff[PKT_BUFF_SIZE];
        int pktLen;

        pktLen = convertHexPktStrToPkt(ethernet_pkt,
                                       pktBuff,
                                       PKT_BUFF_SIZE);

        injectL2Packet(sandboxId, portIndex, (uint8_t *)pktBuff, pktLen);

    } else  if (command.compare("history") == 0) {
        std::cout << "Command history: " << std::endl;
        for(int t=0; t < _commandHistory.size(); ++t){
            std::cout << "\t" << _commandHistory.at(t) << std::endl;
        }
    } else  if (command.compare("clear-history") == 0) {
        _commandHistory.clear();
        std::cout << "Cleared history" << std::endl;

    } else  if ((command.compare("quit") == 0) ||
                (command.compare("exit") == 0)) {
        std::cout << "Exiting... " << std::endl;
        exit(1);
    } else {
        std::cout << "Invalid command '" << command << "'" << std::endl;
    }

    //
    // Add commands to command history -
    // skip commands which are not to be added.
    //
    if ((command.compare("history") != 0) &&
        (command.compare("clear-history") != 0)) {
        _commandHistory.push_back(command_str);
    }
}

//
// @fn
// cli
//
// @brief
// This example afi client's command line interface
//
// @param[in] void
// @return void
//

void
JP4Agent::cli(void) {
    std::cout << "Starting CLI...\n";
    std::cout << "Enter 'help' to display list of available commands\n";

    for (std::string command_str; std::cout << "____JP4Agent____ > " &&
         std::getline(std::cin, command_str); )
    {
        if ((!command_str.empty()) &&
            (command_str.find_first_not_of(' ') != std::string::npos)) {
            handleCliCommand(command_str);
        }
    }
}
