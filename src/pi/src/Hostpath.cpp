//
// Juniper P4 Agent
//
/// @file  Hostpath.h
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

#include <array>
#include <iostream>
#include <thread>

#include "ControllerConnection.h"
#include "DeviceHPPacket.h"
#include "Hostpath.h"

//
// @fn
// handlePacketFromDevice
//
// @brief
// Receive hostpath packet
//
// @param[in]
//     pkt Aft packet where received packet will be copied to
// @return 0 - Success, -1 - Error
//

int Hostpath::handlePacketFromDevice()
{
    std::array<char, 2000> _buf;
    udp::endpoint sender_endpoint;

    // Block until data has been received successfully or an error occurs.
    const size_t recvlen =
        _hpUdpSock.receive_from(boost::asio::buffer(_buf), sender_endpoint);
    if (recvlen == 0) {
        std::cout << "Read empty packet!!\n";
        return 0;
    } else if (recvlen <= DeviceHPPacket::_headerSize) {
        std::cout << "Received malformed pkt(len: " << recvlen
                  << "). Dropping it.\n";
        return 0;
    }

    // Allocate packet context *and* buffer
    DeviceHPPacketPtr pkt =
        DeviceHPPacket::createReceive(recvlen - DeviceHPPacket::_headerSize);

    pktTrace("Received (hostpath) pkt ", _buf.data(), recvlen);

    std::copy_n(_buf.data(), recvlen, pkt->header());

    pktTrace("packet header", (char *)(pkt->header()), pkt->headerSize());

    std::cout << "\n\n";
    std::cout << "pkt->headerSize(): " << pkt->headerSize() << " bytes\n";
    std::cout << "Header: Received " << recvlen << " bytes\n";

    pkt->headerParse();

    std::cout << "Received packet:" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "Sandbox Id : " << pkt->sandboxId() << std::endl;
    std::cout << "Port Index : " << pkt->portIndex() << std::endl;
    std::cout << "Data Size  : " << pkt->dataSize() << std::endl;

    std::cout << "pkt->dataSize(): " << pkt->dataSize() << " bytes"
              << std::endl;

    std::cout << "Data: Received " << recvlen << " bytes" << std::endl;

    pktTrace("pkt data", (char *)(pkt->data()), pkt->dataSize());

    // Construct pkt with cpu header
    cpu_header_t cpu_hdr;
    constexpr size_t cpu_hdr_sz = sizeof(cpu_hdr);
    memset(&cpu_hdr, 0, cpu_hdr_sz);
    cpu_hdr.port = htons(pkt->portIndex());

    // XXX: HACK ALERT: Possible bug in VMXZT leads to 5 extra bytes being
    // appended to the punted packet. Work around this for now.
    size_t payload_len =
        (pkt->dataSize() > 5) ? pkt->dataSize() - 5 : pkt->dataSize();
    std::string payload(cpu_hdr_sz + payload_len, '\0');
    memcpy(&payload[0], &cpu_hdr, cpu_hdr_sz);
    memcpy(&payload[cpu_hdr_sz], pkt->data(), payload_len);

    // Punt it to the controller on the stream channel
    p4::PacketIn packet_in;
    packet_in.set_payload(std::move(payload));
    bool result = controller_conn.send_pkt_in(&packet_in);
    if (!result) {
        std::cout << "Failed to send pkt to master controller. No stream.\n";
    }

    return 0;
}

//
// @fn
// hostPathUDPServer
//
// @brief
// Hostpath UDP server
//
// @param[in] void
// @return void
//

void Hostpath::hostPathUDPServer()
{
    // TBD:: move this log to appropriate place
    Log(DEBUG) << "Listening for hostpath packets from device on (UDP) 0.0.0.0:"
               << _hpUdpPort;
    while (true) {
        handlePacketFromDevice();
    }
}

//
// @fn
// startDevicePacketHandler
//
// @brief
// Start AFI packer receiver
//
// @param[in] void
// @return void
//

void Hostpath::startDevicePacketHandler()
{
    std::thread udpSrvr([this] { this->hostPathUDPServer(); });
    udpSrvr.detach();
}

void Hostpath::sendPacketOut(const std::string &pkt)
{
    constexpr auto cpu_hdr_sz = sizeof(cpu_header_t);
    const auto in_pkt_sz = pkt.size();

    // Sanity check
    char zero[8]{};
    if ((in_pkt_sz <= cpu_hdr_sz) || (memcmp(zero, pkt.data(), 8) != 0)) {
        std::cout << "Malformed packet!!\n";
        return;
    }

    uint16_t egress_port = ntohs(((struct cpu_header_t *)pkt.data())->port);

    // XXX: For now, use sandbox ID 0
    injectL2Packet(0, egress_port, (uint8_t *)&pkt[cpu_hdr_sz],
                   (in_pkt_sz - cpu_hdr_sz));
}

//
// @fn
// injectL2Packet
//
// @brief
// Inject layer 2 packet on specified (output)
// port of specified sandbox
//
// @param[in]
//     sandboxId Sandbox index
// @param[in]
//     portIndex Port index
// @param[in]
//     l2Packet Pointer to layer 2 packet to be injected
// @param[in]
//     l2PacketLen Length of layer 2 packet
// @return void
//

int Hostpath::injectL2Packet(SandboxId sandboxId, DevicePortIndex portIndex,
                             const uint8_t *l2Packet, int l2PacketLen)
{
    std::cout << "Injecting layer2 packet - ";
    std::cout << "Sandbox index:" << sandboxId << " ";
    std::cout << "Port index: " << portIndex << std::endl;

    if (!l2Packet) {
        std::cout << "l2Packet NULL" << std::endl;
    }

    DeviceHPPacketPtr pkt = DeviceHPPacket::createTransmit(
        l2PacketLen, sandboxId, portIndex, DeviceHPPacket::PacketTypeL2);

    //
    // Get base of packet data.
    // This is in the buffer immediately after the header.
    //
    uint8_t *pktData = pkt->data();

    //
    // Fill the pktData with 'L2 packet to be injected'
    //
    memcpy(pktData, l2Packet, l2PacketLen);

    pktTrace("xmit pkt ", (char *)(pkt->header()), pkt->size());

    std::cout << __PRETTY_FUNCTION__
              << ": Injecting pkt of size: " << pkt->size() << "\n";

    _hpUdpSock.send_to(boost::asio::buffer(pkt->header(), pkt->size()),
                       _pktIOEndpoint);
    return 0;
}
