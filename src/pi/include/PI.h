//
// Juniper P4 Agent
//
/// @file  PI.h
/// @brief PI
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

#ifndef __PI__
#define __PI__

#include <memory>
#include <string>
#include "pvtPI.h"

class PI;
using PIUPtr = std::unique_ptr<PI>;

class PI
{
 public:
    PI(const std::string &piServAddr, uint16_t hpUdpPort,
       const std::string &pktIOListenAddr, const std::string &cliServAddr)
    {
        _piServer = std::make_unique<PIServer>(piServAddr, hpUdpPort,
                                               pktIOListenAddr, cliServAddr);
    }

    void init()
    {
        _piServer->startPIServer();
        _piServer->startPktIOHandler();
        _piServer->startDbgCLIServer();
    }

 private:
    PIServerUPtr _piServer;
};

#endif  // __PI__
