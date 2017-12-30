//
// Juniper P4 Agent
//
/// @file  JP4Agent.cpp
/// @brief Main file for Juniper P4 Agent
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


#ifndef __JP4Agent__
#define __JP4Agent__

#include "AfiClient.h"
#include "AftPacket.h"

// TBD: Remove grpc:: in code inline
#include <grpc++/grpc++.h>
// #include <grpc++/support/error_details.h>

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <sstream>
#include <json/value.h>
#include <jsoncpp/json/json.h>
#include <fstream>

//#include "gnmi/gnmi.grpc.pb.h"
#include "google/rpc/code.pb.h"
#include "p4runtime.grpc.pb.h"
#include "p4runtime.pb.h"
#include "p4config.pb.h"
#include "uint128.h"
#include "Base.h"
#include "Log.h"
#include "P4Info.h"
#include "AfiObjects.h"
#include "AfiDM.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;


using StreamChannelReaderWriter = grpc::ServerReaderWriter<
  p4::StreamMessageResponse, p4::StreamMessageRequest>;

class JP4Agent;
extern JP4Agent *jP4Agent;

class P4RuntimeServiceImpl : public p4::P4Runtime::Service
{
private:

  Status tableInsert(const p4::TableEntry &tableEntry);
  Status tableWrite(p4::Update_Type update,
                     const p4::TableEntry &table_entry);
  Status _write(const p4::WriteRequest &request);

  Status Write(ServerContext *context,
               const p4::WriteRequest *request,
               p4::WriteResponse *rep) override;
               
  Status Read(ServerContext *context,
              const p4::ReadRequest *request,
              ServerWriter<p4::ReadResponse> *writer) override;

  Status SetForwardingPipelineConfig(
      ServerContext *context,
      const p4::SetForwardingPipelineConfigRequest *request,
      p4::SetForwardingPipelineConfigResponse *rep) override;

  Status GetForwardingPipelineConfig(
      ServerContext *context,
      const p4::GetForwardingPipelineConfigRequest *request,
      p4::GetForwardingPipelineConfigResponse *rep) override;
      
  Status StreamChannel(ServerContext *context,
                       StreamChannelReaderWriter *stream) override;

  static Uint128 convert_u128(const p4::Uint128 &from) {
    return Uint128(from.high(), from.low());
  }

  P4Info _p4Info;
  AfiDeviceCfg _afiDevCfg;
};

class JP4Agent
{
public:
 JP4Agent(boost::asio::io_service &ioService, const std::string &afiServerAddr,
          const std::string &piServerAddr, short hpUdpPort,
          const std::string &pktIOListenAddr)
     : _afiClient(afiServerAddr),
       _piServerAddr(piServerAddr),
       _pktIOListenAddr(pktIOListenAddr),
       _ioService(ioService),
       _hpUdpPort(hpUdpPort),
       _hpUdpSock(ioService, BOOST_UDP::endpoint(BOOST_UDP::v4(), hpUdpPort))
 {
   std::vector<std::string> hostpathAddr_substrings;
   boost::split(hostpathAddr_substrings, _pktIOListenAddr,
                boost::is_any_of(":"));
   std::string &hpIpStr = hostpathAddr_substrings[0];
   std::string &hpUDPPortStr = hostpathAddr_substrings[1];

   BOOST_UDP::resolver resolver(_ioService);
   _pktIOEndpoint = *resolver.resolve({BOOST_UDP::v4(), hpIpStr, hpUDPPortStr});
 }

 ~JP4Agent(){};

 void startAfiClient(void) { _afiClient.openSandbox(); }

 //
 // Handler hostpath packet from sandbox
 //
 int recvHostPathPacket();

 // Send pkt out via the UDP connection to the AFI server
 void send_packet_out(const std::string& pkt);

 //
 // Start packet listner thread
 // It handles packets received from PktIO
 //
 void startAfiPktRcvr(void);

 //
 // Start PI Server thread
 //
 void startPIServer();

 AfiClient& afiClient() { return _afiClient; }

 //
 // Command line interfac
 //
 void cli(void);

private:
    AfiClient                _afiClient;

    const std::string        _piServerAddr;
    P4RuntimeServiceImpl     _piService;
    ServerBuilder            _piBuilder;
    std::unique_ptr<Server>  _piServer;

    const std::string        _pktIOListenAddr;

    boost::asio::io_service& _ioService;
    unsigned short                    _hpUdpPort;     //< Hospath UDP port
    BOOST_UDP::socket        _hpUdpSock;     //< Hospath UDP socket

    BOOST_UDP::endpoint      _pktIOEndpoint;


    std::vector<std::string> _commandHistory;

    //
    // Hostpath UDP server
    //
    void hostPathUDPServer(void);

    //
    // Inject layer 2 packet to a port
    //
    int injectL2Packet(AftSandboxId  sandboxId,
                       AftIndex      portIndex,
                       const uint8_t *l2Packet,
                       int           l2PacketLen);
 

    // Start server and bind to default address (0.0.0.0:50051)
    void PIGrpcServerRun();

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    void PIGrpcServerWait();

    // Shutdown server but waits for all RPCs to finish
    void PIGrpcServerShutdown();

    // Force-shutdown server with a deadline for all RPCs to finish
    void PIGrpcServerForceShutdown(int deadline_seconds);

    //
    // PI server
    //
    void piServer(void);

    //
    // Handle CLI commands
    //
    void handleCliCommand(std::string const & command_str);
};

#endif // __JP4Agent__
