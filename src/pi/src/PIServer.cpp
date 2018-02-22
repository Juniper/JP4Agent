//
// Juniper P4 Agent
//
/// @file  PIServer.cpp
/// @brief PI Server
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

#include "pvtPI.h"

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
PIServer::piServer(void)
{
    PIGrpcServerRun();
    PIGrpcServerWait();
}

//
// @fn
// startPIServer
//
// @brief
// Start PI Server
//
// @param[in] void
// @return void
//

void
PIServer::startPIServer(void)
{
    std::thread piSrvr( [this] { this->piServer(); } );
    piSrvr.detach();
}

void 
PIServer::PIGrpcServerRun()
{
    _piBuilder.AddListeningPort(_piServerAddr,
                                grpc::InsecureServerCredentials());

    _piBuilder.RegisterService(&_piService);

    //builder.RegisterService(&server_data->gnmi_service);

    _piBuilder.SetMaxReceiveMessageSize(256*1024*1024);  // 256MB
    
    _piServer = _piBuilder.BuildAndStart();
    Log(DEBUG) << "PI Server listening on " << _piServerAddr << "\n";
}

void 
PIServer::PIGrpcServerWait()
{
    _piServer->Wait();
}

void
PIServer::PIGrpcServerShutdown()
{
    _piServer->Shutdown();
}

void PIServer::PIGrpcServerForceShutdown(int deadline_seconds)
{
    using clock = std::chrono::system_clock;
    auto deadline = clock::now() + std::chrono::seconds(deadline_seconds);
    _piServer->Shutdown(deadline);
}


