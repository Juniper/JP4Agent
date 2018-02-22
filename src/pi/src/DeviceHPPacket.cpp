//
// Juniper P4 Agent
//
/// @file   DeviceHPPacket.cpp
/// @brief  Device hostpath packet
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

#include "pvtPI.h"

// 
// @brief  Create Transmit Packet
//
DeviceHPPacketPtr DeviceHPPacket::createTransmit (uint16_t dataSize,
                                        SandboxId sandboxId, 
                                        PortIndex portIndex, 
                                        DeviceHPPacket::PacketType packetType)
{
    DeviceHPPacketPtr pkt = std::make_shared<DeviceHPPacket>();
        
    pkt->_selfPtr = pkt;

    pkt->_sandboxId    = sandboxId;
    pkt->_portIndex    = portIndex; 
    pkt->_pktDir       = PacketDirTransmit;  
    pkt->_innerPktType = packetType;

    pkt->_totalLength = pkt->_headerSize + dataSize;
    // TBD: Revisit
    //pkt->_pktDataBuffer = new (std::nothrow) uint8_t [pkt->_baseHdr.totalLength()];
    pkt->_pktDataBuffer = new (std::nothrow) uint8_t[pkt->_totalLength]{};

    if (pkt->_pktDataBuffer == nullptr) {
        // TBD: error assigning memory. Take measures.
        return nullptr;
    }

    // Serialize the hdr into the data buffer
    pkt->headerSerialize();

    return pkt;
}

// 
// @brief Create Receive Packet
//
DeviceHPPacketPtr DeviceHPPacket::createReceive (uint16_t dataSize)
{
    DeviceHPPacketPtr pkt = std::make_shared<DeviceHPPacket>();
        
    pkt->_selfPtr = pkt;

    pkt->_pktDir       = PacketDirReceive;  // TBD: Revisit
    pkt->_totalLength = pkt->_headerSize + dataSize;

    // TBD: Revisit 
    // First allocate for header and then increase/decrease
    //pkt->_pktDataBuffer = new (std::nothrow) uint8_t [BaseHeader::size + 5000];
    pkt->_pktDataBuffer = new (std::nothrow) uint8_t [pkt->_totalLength]{};
    if (pkt->_pktDataBuffer == nullptr) {
        // error assigning memory. Take measures.
        return nullptr;
    }

    return pkt;
}

//
// @brief Serializes header
//
void DeviceHPPacket::headerSerialize(void)
{
    uint8_t   version = 0;
    uint8_t   direction = DeviceHPPacket::PacketDirTransmit;

    uint8_t  *hdr = _pktDataBuffer;

    ((*(uint8_t *)hdr) &= 0x0F);
    ((*(uint8_t *)hdr) |= (((uint8_t)version) << 4) & 0xF0);

    ((*(uint8_t *)hdr) &= 0xF7);
    ((*(uint8_t *)hdr) |= (((uint8_t)direction) << 3) & 0x08);

    *(uint8_t  *)(hdr + 1) = 0;
    *(uint16_t *)(hdr + 2) = htons(_totalLength);
    *(uint16_t *)(hdr + 4) = htons(_sandboxId);
    *(uint16_t *)(hdr + 6) = htons(_portIndex);
}

//
// @brief Parses header of Aft packet
//
void DeviceHPPacket::headerParse(void)
{
    {
        boost::io::ios_all_saver ias(std::cout);
        std::cout << "Pkt hdr:" << std::hex;
        for (int i = 0; i < _headerSize; i++) {
            std::cout << " " << std::setfill('0') << std::setw(2)
                      << +_pktDataBuffer[i];
        }
        std::cout << "\n";
    }

    uint8_t  *hdr         = _pktDataBuffer;
    //    uint8_t  version      = (((*(uint8_t *)hdr) & 0xF0) >> 4);
    uint8_t  direction    = (((*(uint8_t *)hdr) & 0x08) >> 3);
    uint16_t total_length = *(uint16_t *)(hdr + 2);
    uint16_t sb_index     = *(uint16_t *)(hdr + 4);
    uint16_t port_index   = *(uint16_t *)(hdr + 6);

    total_length = ntohs(total_length);
    sb_index     = ntohs(sb_index);
    port_index   = ntohs(port_index);

    _sandboxId    = static_cast<uint16_t>(sb_index);
    _portIndex    = port_index; 
    if (direction == 0) {
        _pktDir       = PacketDirReceive;  
    } else {
        _pktDir       = PacketDirTransmit;  
    }
    _innerPktType = PacketTypeL2;
}
