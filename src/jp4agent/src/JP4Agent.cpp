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

#include <fstream>

#include "JP4Agent.h"
#include "PI.h"
#include "JaegerLog.h"

//
// @fn
// readConfig
//
// @brief
// Reads configuration
//
// @param[in] void
// @return void
//

bool
JP4Agent::Config::readConfig()
{
    Json::Value   cfg_root;
    std::ifstream cfgfile(_configFile);
    cfgfile >> cfg_root;

    _debugMode =
        cfg_root["JP4AgentConfig"]["DebugConfig"]["debug-mode"].asString();
    _piServerAddr =
        cfg_root["JP4AgentConfig"]["PIConfig"]["pi-server-address"].asString();
    _pktIOServerAddr =
        cfg_root["JP4AgentConfig"]["DevicePktIOConfig"]["pktio-server-address"]
            .asString();
    _cliServerAddr =
        cfg_root["JP4AgentConfig"]["DebugCLIConfig"]["cli-server-address"]
            .asString();
    _hostpathPort =
        cfg_root["JP4AgentConfig"]["HostpathConfig"]["hostpath-server-port"]
            .asUInt();
    _targetAddr =
        cfg_root["JP4AgentConfig"]["TargetConfig"]["target-address"].asString();
    _jaegerConfigFile =
        cfg_root["JP4AgentConfig"]["JaegerConfig"]["jaeger-config-file"]
            .asString();
    return true;
}

//
// @fn
// validateConfig
//
// @brief
// Validates configuration
//
// @param[in] void
// @return void
//

bool
JP4Agent::Config::validateConfig()
{
    Log(DEBUG) << "Validating Configuration :TBD ";
    return true;
}

//
// @fn
// displayConfig
//
// @brief
// Displays configuration
//
// @param[in] void
// @return void
//

void
JP4Agent::Config::displayConfig()
{
    std::cout << "___ Configuration ___" << std::endl;

    Log(DEBUG) << "configFile      : " << _configFile;
    Log(DEBUG) << "debugmode       : " << _debugmode;
    Log(DEBUG) << "piServerAddr    : " << _piServerAddr;
    Log(DEBUG) << "pktIOServerAddr : " << _pktIOServerAddr;
    Log(DEBUG) << "dbgCLIServAddr  : " << _cliServerAddr;
    Log(DEBUG) << "hostpathPort    : " << _hostpathPort;
    Log(DEBUG) << "jaegerConfigFile: " << _jaegerConfigFile;
}

//
// @fn
// init
//
// @brief
// Initialize
//
// @param[in] void
// @return void
//


void
JP4Agent::init()
{
    _config.readConfig();
    _config.validateConfig();
    _config.displayConfig();

    std::string cfg = _config._jaegerConfigFile;
    if (!cfg.empty()) {
      JaegerLog::getInstance()->initTracing(cfg);
    }

    _pi = std::make_unique<PI>(_config._piServerAddr,
                               _config._hostpathPort,
                               _config._pktIOServerAddr,
                               _config._cliServerAddr);
    //
    // Initialize PI
    //
    _pi->init();

    //
    // Initialize Afi
    //
    AFIHAL::Afi::instance().init(_config._targetAddr);
}
