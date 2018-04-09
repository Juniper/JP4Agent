//
// Test controller
//
/// @file  Controller.cpp
/// @brief Main file for Juniper P4 Agent
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


#ifndef __Controller__
#define __Controller__

#include <chrono>
#include <string>
#ifndef SUD
#include <typeinfo>
#endif // SUD

int ControllerSetConfig();

bool ControllerInjectL2Pkt(const std::string&, uint16_t);

bool ControllerPuntPkt(std::string&, uint16_t&, std::chrono::milliseconds);

bool ControllerICMPEcho(std::chrono::milliseconds);

bool ControllerHandleArpReq(std::chrono::milliseconds);

int ControllerAddRouteEntry();

// Packet header definitions
struct __attribute__((packed)) cpu_header_t {
    char     zeros[8];
    uint16_t reason;
    uint16_t port;
};

constexpr size_t cpu_hdr_sz = sizeof(cpu_header_t);

enum class Cpureason {
    NoArpEnt = 0,  // Packet punted up for ARP resolution
    ArpMsg,
    DataPkt
};

// Controller's view of an interface on the device.
struct interface_t {
    uint16_t port;
    uint8_t  ip_addr[4];
    uint8_t  mac_addr[6];
};

#ifndef SUD
class Utils {
public:
    template <typename T>
    std::string uint2Str(T i)
    {
        std::string ret;

        if (typeid(T) == typeid(uint16_t)) {
            i = ntohs(i);
            ret = std::string(reinterpret_cast<char *>(&i), sizeof(i));
        } else if (typeid(T) == typeid(uint32_t)) {
            i = ntohl(i);
            ret = std::string(reinterpret_cast<char *>(&i), sizeof(i));
        } else if (typeid(T) == typeid(uint64_t)) {
            unsigned char c[6];

            for (int j = 5; j > 0; j--) {
                c[j] = i & 0xff;
                i = i >> 8;
            }
            c[0] = i & 0xff;

            ret = std::string(reinterpret_cast<const char *>(c), sizeof(c));
        }

        return ret;
    }
};
#endif // SUD

#endif // __Controller__
