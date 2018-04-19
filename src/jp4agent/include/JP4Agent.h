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

#ifndef __JP4Agent__
#define __JP4Agent__

#include <string>

#include "Afi.h"
#include "Log.h"
#include "PI.h"

extern const std::string _debugmode;

class JP4Agent
{
public:
    explicit JP4Agent(const std::string &configFile) : _config(configFile){};
    ~JP4Agent()
    {}
    //
    // Init
    //
    void init();
private:
    class Config
    {
    public:
        Config(const std::string &configFile) : _configFile(configFile) {}

        //
        // Read configuration
        //
        bool readConfig();

        //
        // Validate configuration
        //
        bool validateConfig();

        //
        // Display configuration
        //
        void displayConfig();

        // Config fields
        std::string _configFile;
        std::string _debugMode;
        std::string _piServerAddr;
        std::string _pktIOServerAddr;
        std::string _cliServerAddr;
	std::string _jaegerConfigFile;
        uint16_t    _hostpathPort;
        std::string _targetAddr;
    };

    Config _config;
    PIUPtr _pi;
};

#endif  // __JP4Agent__
