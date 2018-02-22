//
// Juniper P4 Agent AFI Client
//
/// @file  AftClient.h
/// @brief AFI client for Juniper P4 Agent
//
//
// Created by Sandesh Kumar Sodhi, January 2017
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

#ifndef __AftClient__
#define __AftClient__

#include <memory>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <thread>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>

#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/algorithm/string.hpp>

//#include "Base.h"
//#include "JP4Agent.h"
#include "Log.h"
#include "jnx/Aft.h"
#include "jnx/AfiTransport.h"
#include "Utils.h"
#include "Aft.h"

//namespace AFTHALP {

//
// @class   AftClient
// @brief   Implements AFT client
//
class AftClient
{
public:
    static AftClient& instance() {
        static AftClient aftclient;
        return aftclient;
    }   
  
    AftClient(AftClient const&) = delete;
    AftClient(AftClient&&) = delete;
    AftClient& operator=(AftClient const&) = delete;
    AftClient& operator=(AftClient &&) = delete;

    void init(const std::string &configFile);

    //
    // Add a table to the sandbox
    //
    AftNodeToken addTable(const std::string &tableName,
                          const std::string &fieldName,
                          AftNodeToken defaultTragetToken);

    //
    // Add a routing table to the sandbox
    //
    AftNodeToken addRouteTable(const std::string &rttName,
                               AftNodeToken defaultTragetToken);


    int addRoute(AftNodeToken  rttNodeToken,
                 const char *prefix_bytes,
                 int num_prefix_bytes,
                 int prefix_len,
                 AftNodeToken  routeTragetToken);

    //
    // Add route to a routing table
    //
    int addRoute(AftNodeToken      rttNodeToken,
                 const std::string &prefix,
                 AftNodeToken       routeTragetToken);

    //
    // Create list
    //
    AftNodeToken createList(AftTokenVector tokVec);

    //
    // Set next node for an input port
    //
    int setInputPortNextNode(AftIndex inputPortIndex,
                             AftNodeToken nextToken);


    int setIngressStart(AftNodeToken ingressStartToken);

    //
    // Get token for an output port
    //
    AftNodeToken getOuputPortToken(AftIndex outputPortIndex);


    AftNodeToken addReceiveNode(uint32_t receiveCode, uint64_t context);

    //
    // Add ethernet encapsulation node
    //
    AftNodeToken addEtherEncapNode(const std::string &dst_mac,
                                   const std::string &src_mac,
                                   AftNodeToken       nextToken);


    AftNodeToken outputPortToken(AftIndex portIndex);
    AftNodeToken puntPortToken(void);

protected:
    AftClient() {}
    //
    // Destructor
    //
    ~AftClient() {};

private:
    std::string                 _afiServerAddr;   //< AFI server address
    std::string                 _afiHostpathAddr; //< AFI hostpath address

    const std::string           _sandbox_name = "jp4agent"; //< Sandbox name
    AftSandboxPtr               _sandbox;
    AftTransportPtr             _transport;

    bool                        _tracing;  //< True if debug tracing is enabled

    //
    // read configuration
    //
    int readConfig(const std::string &configFile);

    //
    // Constructor
    //
    //AftClient(const std::string &afiServerAddr)
    //          : _afiServerAddr(afiServerAddr) 
    // TBD: move to AftClient.cpp
    void createTransportToAftServer() {
        //
        // Initialize transport connection to AFI server
        //
        //_transport = AfiTransport::create(_afiServerAddr);
        _transport = AfiTransport::create();
        assert(_transport != nullptr);
    }

    //
    // Create sandbox
    //
    int openSandbox();

    void sendboxSend(AftInsertPtr insert) {
        if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
            Log(DEBUG) << "_aft_debugmode: "<< _aft_debugmode << " Not calling _sandbox->send()";
            return;
        } 
        _sandbox->send(insert);
    }

};

//}  // namespace AFTHALP

#endif // __AftClient__
