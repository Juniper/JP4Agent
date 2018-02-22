//
// Juniper P4 Agent
//
/// @file  PIServer.cpp
/// @brief Main file for Juniper P4 Agent
//
// Created by Sandesh Kumar Sodhi, January 2018
// Copyright (c) [2018] Juniper Networks, Inc. All rights reserved.
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

#ifndef __PIServer__
#define __PIServer__


class PIServer;
using PIServerUPtr = std::unique_ptr<PIServer>;

class PIServer
{
public:
    PIServer(const std::string &piServerAddr)
        : _piServerAddr(piServerAddr) {}

    ~PIServer(){};

    //
    // Start PI Server thread
    //
    void startPIServer();

private:
    const std::string        _piServerAddr;

    P4RuntimeServiceImpl     _piService;
    ServerBuilder            _piBuilder;
    std::unique_ptr<Server>  _piServer;

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
};

#endif // __PIServer__
