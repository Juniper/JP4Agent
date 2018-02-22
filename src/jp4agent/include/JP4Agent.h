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

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <sstream>
#include <arpa/inet.h>
#include <algorithm>
#include <cstddef>
#include <mutex>
#include <fstream>

#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/algorithm/string.hpp>


#include "Log.h"
#include "PI.h"
#include "Afi.h"

extern const std::string _debugmode;

//#define BOOST_UDP boost::asio::ip::udp::udp

#define INVALID_PORT_INDEX       0xFFFF

// Packet header definitions

#if 0
struct __attribute__((packed)) cpu_header_t {
    char zeros[8];
    uint16_t reason;
    uint16_t port;
};
#endif

// TBD: FIXME Move these header declarations to appropriate place
struct __attribute__((packed)) arp_header_t {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t  hw_addr_len;
    uint8_t  proto_addr_len;
    uint16_t opcode;
    uint8_t  hw_src_addr[6];
    uint32_t proto_src_addr;
    uint8_t  hw_dst_addr[6];
    uint32_t proto_dst_addr;
};

struct __attribute__((packed)) eth_header_t {
    uint8_t  dst_addr[6];
    uint8_t  src_addr[6];
    uint16_t ethertype;
};

struct __attribute__((packed)) ipv4_header_t {
    uint8_t noise[8];
    uint8_t ttl;
    uint8_t protocol;
    uint16_t hdr_chksum;
    uint32_t src_addr;
    uint32_t dst_addr;
};

class JP4Agent;
using JP4AgentUPtr = std::unique_ptr<JP4Agent>;

class JP4Agent
{
public:
    JP4Agent(const std::string &configFile) : _config(configFile) {};
    ~JP4Agent(){};

    //
    // Init
    //
    void init();

private:
    class Config
    {
    public:
        Config(const std::string &configFile) : _configFile(configFile) {}
        ~Config(){};

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

    private:
        std::string       _configFile;
        std::string       _debugMode;
        std::string       _piServerAddr;
        std::string       _pktIOServerAddr;
        short             _hostpathPort;
    };


    Config            _config;
    PIUPtr            _pi;
};

#endif // __JP4Agent__
