//
// Juniper P4 Agent AFI Client
//
/// @file  AfiClient.h
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

#ifndef __AfiClient__
#define __AfiClient__

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

#include "Base.h"
#include "Log.h"
#include "jnx/Aft.h"
#include "jnx/AfiTransport.h"
#include "Utils.h"

//
// @class   AfiClient
// @brief   Implements a sample AFI client
//
class AfiClient
{
public:
    enum CrossConnectOp {
        CC_DELETE = 0,
        CC_ADD = 1 
    };  

    //
    // Constructor
    //
    AfiClient(const std::string &afiServerAddr)
              : _afiServerAddr(afiServerAddr) {
        //
        // Initialize transport connection to AFI server
        //
        //_transport = AfiTransport::create(_afiServerAddr);
        _transport = AfiTransport::create();
        assert(_transport != nullptr);
    }

    //
    // Destructor
    //
    ~AfiClient();

    //
    // Create sandbox
    //
    int openSandbox();

    //
    // Cross connect operation
    // Adds/Deletes cross connect
    //
    int crossConnectPortsOp(const std::string &endpoint1,
                            const std::string &endpoint2,
                            AfiClient::CrossConnectOp op);

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
    // Create Index table
    //
    AftNodeToken createIndexTable(const std::string &field_name,
                                  AftIndex iTableSize);

    //
    // Add Index table entry
    //
    int addIndexTableEntry(AftNodeToken iTableToken,
                           u_int32_t    entryIndex,
                           AftNodeToken entryTargetToken);

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

    //
    // Add MPLS label encapsulation node
    //
    AftNodeToken addLabelEncap(const std::string &outerLabelStr,
                               const std::string &innerLabelStr,
                               AftNodeToken       nextToken);

    //
    // Add MPLS label decapsulation node
    //
    AftNodeToken addLabelDecap(AftNodeToken nextToken);

    //
    // Add counter node
    //
    AftNodeToken addCounterNode(void);

    //
    // Add discard node
    //
    AftNodeToken addDiscardNode(void);


    AftNodeToken outputPortToken(AftIndex portIndex);
    AftNodeToken puntPortToken(void);

private:
    std::string                 _afiServerAddr;   //< AFI server address
    std::string                 _afiHostpathAddr; //< AFI hostpath address

    const std::string           _sandbox_name = "jp4agent"; //< Sandbox name
    AftSandboxPtr               _sandbox;
    AftTransportPtr             _transport;

    bool                        _tracing;  //< True if debug tracing is enabled

    void sendboxSend(AftInsertPtr insert) {
        if (_debugmode.find("no-afi-server") != std::string::npos) {
            Log(DEBUG) << "_debugmode: "<< _debugmode << " Not call ing _sandbox->send()";
            return;
        } 
        _sandbox->send(insert);
    }

};

#endif // __AfiClient__
