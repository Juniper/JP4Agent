//
// Juniper P4 Agent
//
/// @file  Base.h
/// @brief P4 Info
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

#ifndef __Base__
#define __Base__

#include <memory>


#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/algorithm/string.hpp>
#include "base64.h"


extern const std::string _debugmode;

#define BOOST_UDP boost::asio::ip::udp::udp

#define INVALID_PORT_INDEX       0xFFFF

// Packet header definitions

struct __attribute__((packed)) cpu_header_t {
    char zeros[8];
    uint16_t reason;
    uint16_t port;
};

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

#endif // __Base__
