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

void ControllerSetP4Input(std::string pipelineFile, std::string runtimeFile);

int ControllerSetConfig();

bool ControllerInjectL2Pkt(const std::string&, uint16_t);

bool ControllerPuntPkt(std::string&, uint16_t&, std::chrono::milliseconds);

bool
ControllerInjectPuntL2Pkt(const std::string &inject_l2_pkt,
                          std::string &punt_l2_pkt,
                          uint16_t egress_port,
                          uint16_t ingress_port,
                          std::chrono::milliseconds timeout_ms);

bool ControllerICMPEcho(std::chrono::milliseconds);

bool ControllerHandleArpReq(std::chrono::milliseconds);

int ControllerAddRouteEntry(uint32_t dAddr,
                            uint16_t pLen,
                            uint32_t nAddr,
                            uint64_t mac,
                            uint16_t oPort);

int ControllerAddVrfEntry(uint16_t etype,
                          std::string mac,
                          uint32_t addr,
                          uint32_t vrf);

int
ControllerAddClassIdEntry(uint16_t vid,
                          uint8_t  pcp,
                          uint16_t etype,
                          uint8_t  proto,
                          uint8_t  ttl,
                          uint32_t dAddr,
                          uint16_t sPort,
                          uint16_t dPort,
                          uint8_t  classId);

int ControllerAddRtEncapEntry(uint32_t    dAddr,
                              uint16_t    pLen,
                              uint32_t    vrf,
                              uint64_t    smac,
                              uint64_t    dmac,
                              uint16_t    oPort,
                              uint8_t     l3ClassId,
                              std::string type);

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

#endif // __Controller__
