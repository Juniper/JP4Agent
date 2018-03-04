//
// Juniper P4 Agent
//
/// @file  handle_cmd.cpp
/// @brief Debug CLI service for JP4Agent
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
#include <sstream>
#include <string>
#include <thread>
#include <jaegertracing/Tracer.h>
#include <boost/algorithm/string.hpp>

#include "Afi.h"
#include "CLIService.h"

extern std::unique_ptr<opentracing::v1::Span> span;

Status
CmdHandlerSvcImpl::SendCmd(ServerContext *context, const CmdRequest *req,
                           CmdReply *reply)
{
    const std::string cmdstr = req->cmdstr();
    std::cout << "Received cmd: " << cmdstr << std::endl;
    std::string cmdoutstr;

    // Split cmd string into cmd and args
    std::vector<std::string> cmd_sub_str;
    boost::split(cmd_sub_str, cmdstr, boost::is_any_of("\t "));
    if (cmd_sub_str[0] == "add-table-entry") {
        if (cmd_sub_str.size() != 4) {
            cmdoutstr =
                "Invalid cmd. Please specify table name, prefix and prefix "
                "length.";
            goto quit;
        }
        const std::string tblname = cmd_sub_str[1];
        const std::string prefix  = cmd_sub_str[2];
        const int         plen    = std::stoi(cmd_sub_str[3]);
        AFIHAL::Afi::instance().addEntry(prefix, plen);
        cmdoutstr = "Added entry to table " + tblname;
    } else if (cmd_sub_str[0] == "add-table") {
        if (cmd_sub_str.size() != 6) {
            cmdoutstr = "Invalid add-table cmd.";
            goto quit;
        }
        const std::string  tblname     = cmd_sub_str[1];
        const std::string  key_field   = cmd_sub_str[2];
        const int          protocol    = std::stoi(cmd_sub_str[3]);
        const std::string  def_nxt_obj = cmd_sub_str[4];
        const unsigned int tblsz       = std::stoul(cmd_sub_str[5]);
        AFIHAL::Afi::instance().addAfiTree(tblname, key_field, protocol,
                                           def_nxt_obj, tblsz);
        cmdoutstr = "Added table with name: " + tblname;
    } else if (cmd_sub_str[0] == "show-afi-objects") {
        std::vector<AFIHAL::AfiObjectPtr> objs =
            AFIHAL::Afi::instance().getAfiObjects();
        std::ostringstream obj_details;

        for (const auto &obj : objs) {
            obj_details << "Object name: " << obj->name() << "\n";
            obj_details << "Object type: " << obj->type() << "\n";
            obj_details << "Object string: " << obj->objStr() << "\n\n";
        }
        cmdoutstr = obj_details.str();
    } else {
        cmdoutstr = "Invalid cmd: " + cmd_sub_str[0];
    }

quit:
    reply->set_cmdout(cmdoutstr);
    return Status::OK;
}

void
CLIService::runGrpcServer()
{
    ServerBuilder builder;
    builder.AddListeningPort(_cli_serv_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&_service);

    std::unique_ptr<Server> server{builder.BuildAndStart()};
    std::cout << "CLI Server listening on " << _cli_serv_addr << std::endl;

    // TBD : Remove later. HACK: Create a span for route table
    auto tracer = opentracing::Tracer::Global();
    if (!span) {
        span = tracer->StartSpan("Route Table");
    }

    server->Wait();
}

void
CLIService::startCLIService()
{
    std::thread cli_server{[this]() { this->runGrpcServer(); }};
    cli_server.detach();
}
