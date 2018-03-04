//
// Juniper P4 Agent
//
/// @file  CLIServer.h
/// @brief Debug CLI command handler service
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

#include <grpc++/grpc++.h>
#include "cli/jp4cli.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using jp4cli::CmdHandler;
using jp4cli::CmdReply;
using jp4cli::CmdRequest;

class CmdHandlerSvcImpl final : public CmdHandler::Service
{
    Status SendCmd(ServerContext *context, const CmdRequest *req,
                   CmdReply *reply) override;
};

class CLIService
{
 private:
    const std::string _cli_serv_addr;
    CmdHandlerSvcImpl _service;

    void runGrpcServer();

 public:
    explicit CLIService(const std::string &serv_addr)
        : _cli_serv_addr(serv_addr)
    {
    }

    void startCLIService();
};
