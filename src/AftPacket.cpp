//
//  AftPkt.cpp
//  Aft - A Forwarding Toolkit
//
/// @file  AftPkt.cpp
/// @brief AftPkt class management
//
// Created by Sandesh Kumar Sodhi, December 2017
// Copyright (c) [2017] Juniper Networks, Inc. All rights reserved.
//

#include "AftPacket.h"
#include <cassert>
#include <boost/io/ios_state.hpp>

// 
// @brief  Create Transmit Packet
//
AftPacketPtr AftPacket::createTransmit (AftLength dataSize,
                                        AftSandboxId sandboxId, 
                                        AftIndex portIndex, 
                                        AftPacket::PacketType packetType)
{
    AftPacketPtr pkt = std::make_shared<AftPacket>();
        
    pkt->_selfPtr = pkt;

    pkt->_sandboxId    = sandboxId;
    pkt->_portIndex    = portIndex; 
    pkt->_pktDir       = PacketDirTransmit;  
    pkt->_innerPktType = packetType;

    pkt->_totalLength = pkt->_headerSize + dataSize;
    // TBD: Revisit after POC
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
AftPacketPtr AftPacket::createReceive (AftLength dataSize)
{
    AftPacketPtr pkt = std::make_shared<AftPacket>();
        
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
void AftPacket::headerSerialize(void)
{
    uint8_t   version = 0;
    uint8_t   direction = AftPacket::PacketDirTransmit;

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
void AftPacket::headerParse(void)
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
