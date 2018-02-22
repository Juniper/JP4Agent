//
// Juniper P4 Agent
//
/// @file  handle_cmd.cpp
/// @brief CLI service for JP4Agent
//
// Created by Karthikeyan Sivaraj, February 2018
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

#include <iostream>
#include <memory>
#include <string>
#include "handle_cmd.h"

Status CmdHandlerSvcImpl::SendCmd(ServerContext *context, const CmdRequest *req,
                                  CmdReply *reply)
{
    std::cout << "Received cmd: " << req->cmdstr() << std::endl;
    std::string cmdoutstr = "Output for " + req->cmdstr();
    reply->set_cmdout(cmdoutstr);
    return Status::OK;
}

void CLIServer::RunServer()
{
    CmdHandlerSvcImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(cli_serv_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server{builder.BuildAndStart()};
    std::cout << "CLI Server listening on " << cli_serv_addr << std::endl;

    server->Wait();
}

int main()
{
    CLIServer serv{"0.0.0.0:53421"};
    serv.RunServer();
}
