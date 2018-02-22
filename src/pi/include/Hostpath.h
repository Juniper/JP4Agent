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

#define BOOST_UDP boost::asio::ip::udp::udp

struct __attribute__((packed)) cpu_header_t {
    char zeros[8];
    uint16_t reason;
    uint16_t port;
};

class Hostpath
{
public:
    Hostpath(boost::asio::io_service &ioService,
             short hpUdpPort,
             const std::string &pktIOListenAddr)
        : _pktIOListenAddr(pktIOListenAddr),
          _ioService(ioService),
          _hpUdpPort(hpUdpPort),
          _hpUdpSock(ioService, BOOST_UDP::endpoint(BOOST_UDP::v4(), hpUdpPort))
    {
        std::vector<std::string> hostpathAddr_substrings;
        boost::split(hostpathAddr_substrings, _pktIOListenAddr,
                     boost::is_any_of(":"));
        std::string &hpIpStr = hostpathAddr_substrings[0];
        std::string &hpUDPPortStr = hostpathAddr_substrings[1];

        BOOST_UDP::resolver resolver(_ioService);
        _pktIOEndpoint = *resolver.resolve({BOOST_UDP::v4(), hpIpStr, hpUDPPortStr});
    }

    ~Hostpath(){};

    //
    // Handler hostpath packet from device
    //
    int handlePacketFromDevice();

    // Send pkt out via the UDP to device
    void sendPacketOut(const std::string& pkt);

    //
    // Start packet listner thread
    // It handles packets received from PktIO
    //
    void startDevicePacketHandler(void);

private:
    const std::string        _pktIOListenAddr;

    boost::asio::io_service& _ioService;
    unsigned short           _hpUdpPort;     //< Hospath UDP port
    BOOST_UDP::socket        _hpUdpSock;     //< Hospath UDP socket

    BOOST_UDP::endpoint      _pktIOEndpoint;


    //
    // Hostpath UDP server
    //
    void hostPathUDPServer(void);

    //
    // Inject layer 2 packet to a port
    //
    int injectL2Packet(SandboxId  sandboxId,
                       DevicePortIndex      portIndex,
                       const uint8_t *l2Packet,
                       int           l2PacketLen);
};

#endif // __Hostpath__
