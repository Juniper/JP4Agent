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

#include <google/rpc/code.pb.h>
#ifdef UBUNTU
#include <p4runtime.grpc.pb.h>
#else
#include <p4runtime_wrl.grpc.pb.h>
#endif
#include <p4/tmp/p4config.grpc.pb.h>

#include "P4InfoUtils.h"
#include "Utils.h"

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

int ControllerAddMyMacEntry(std::string mac);

int ControllerAddRtEncapEntry(uint32_t    dAddr,
                              uint16_t    pLen,
                              uint32_t    vrf,
                              uint64_t    smac,
                              uint64_t    dmac,
                              uint16_t    oPort,
                              uint8_t     l3ClassId,
                              std::string type);

int ControllerAddPuntEntry(uint32_t iport,
                           uint32_t oport,
                           uint16_t etype,
                           uint64_t dmac,
                           uint8_t  tos,
                           uint8_t  ttl,
                           uint32_t saddr,
                           uint32_t daddr,
                           uint8_t  proto,
                           uint32_t arpTAddr,
                           uint8_t  icmpType,
                           uint16_t vid,
                           uint8_t  dot1p,
                           uint8_t  ingressClassId,
                           uint32_t vrfId,
                           uint8_t qId);

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

template <typename T>
void
addTEntryMF(p4::config::P4Info p4info,
            std::string        t_name,
            ::p4::TableEntry*  t_entry,
            std::string        mf_type,
            std::string        mf_name,
            T                  mf,
            int                bitWidth) 
{
    Utils utils;
    std::string mfS = utils.uint2Str(mf);

    T mfMask = 0;
    while (bitWidth--) {
        mfMask <<= 1;
        mfMask |= 1;
    }
    std::string mfMaskS = utils.uint2Str(mfMask);

    auto mf_id = get_mf_id(p4info, t_name, mf_name);
    auto match = t_entry->add_match();
    match->set_field_id(mf_id);
    if (mf_type == "ternary") {
        auto tnary = match->mutable_ternary();
        tnary->set_value(mfS);
        tnary->set_mask(mfMaskS);
    }
}

template <typename T>
void
addTEntryAction(p4::config::P4Info p4info,
                std::string        a_name,
                ::p4::Action*      action,
                std::string        af_name,
                T                  af)
{
    Utils utils;
    std::string afS = utils.uint2Str(af);

    auto p_id = get_param_id(p4info, a_name, af_name);
    auto param = action->add_params();
    param->set_param_id(p_id);
    param->set_value(afS);
}

#endif // __Controller__
