//
// Juniper P4 Agent
//
/// @file  Hostpath.cpp
/// @brief Hostpath
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

#ifndef __Hostpath__
#define __Hostpath__

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include "pvtPI.h"

using boost::asio::io_service;
using boost::asio::ip::udp;

struct __attribute__((packed)) cpu_header_t {
    char     zeros[8];
    uint16_t reason;
    uint16_t port;
};

class Hostpath
{
 public:
    Hostpath(uint16_t hpUdpPort, const std::string &pktIOListenAddr)
        : _pktIOListenAddr(pktIOListenAddr),
          _hpUdpPort(hpUdpPort),
          _hpUdpSock(_ioService, udp::endpoint(udp::v4(), _hpUdpPort))
    {
        // Connect to the pktIO UDP server on the devide to send packets
        std::vector<std::string> hostpathAddr_substrings;
        boost::split(hostpathAddr_substrings, _pktIOListenAddr,
                     boost::is_any_of(":"));
        std::string &hpIpStr      = hostpathAddr_substrings[0];
        std::string &hpUDPPortStr = hostpathAddr_substrings[1];

        udp::resolver resolver(_ioService);
        _pktIOEndpoint = *resolver.resolve({udp::v4(), hpIpStr, hpUDPPortStr});
    }

    // Send pkt out via the UDP socket to device
    void sendPacketOut(const std::string &pkt);

    //
    // Start packet listener thread
    // It handles packets received from PktIO
    //
    void startDevicePacketHandler();

 private:
    io_service        _ioService;
    const std::string _pktIOListenAddr;

    const uint16_t _hpUdpPort;  //< Hospath UDP port
    udp::socket    _hpUdpSock;  //< Hospath UDP socket

    udp::endpoint _pktIOEndpoint;

    //
    // Handler for hostpath packet from device
    //
    int handlePacketFromDevice();

    //
    // Hostpath UDP server
    //
    void hostPathUDPServer();

    //
    // Inject layer 2 packet to a port
    //
    int injectL2Packet(SandboxId sandboxId, DevicePortIndex portIndex,
                       const uint8_t *l2Packet, int l2PacketLen);
};

#endif  // __Hostpath__
