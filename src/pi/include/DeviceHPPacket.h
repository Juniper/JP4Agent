//
// Juniper P4 Agent
//
/// @file   DeviceHPPacket.h
/// @brief  Device hostpath acket
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

#ifndef __DeviceHPPacket__
#define __DeviceHPPacket__

#include <memory>

// TBD : Change them to enum/const?
#define DEVICE_HOSTPATH_PACKET_HDR_VERSION 0
#define AFT_CLIENT_HOSTPATH_PORT 8001
#define AFT_PACKETIO_HOSTPATH_PORT_STR "8002"

class DeviceHPPacket;

using SandboxId = uint16_t;  ///< 16 bit sandbox id
using PortIndex = uint16_t;  ///< 16 bit port index

///
/// @addtogroup DeviceHPPacket
/// @{
///
using DeviceHPPacketPtr     = std::shared_ptr<DeviceHPPacket>;
using DeviceHPPacketWeakPtr = std::weak_ptr<DeviceHPPacket>;

///
/// @}
///

///
/// @addtogroup DeviceHPPacket pkt
/// @brief Classes for accessing and managing hostpath packets
/// @{
///

///
/// @class   DeviceHPPacket
/// @brief   Primary class for device hostpath packets
///
class DeviceHPPacket
{
 public:
    ///
    /// @enum  PacketDir
    /// @brief DeviceHPPacket packet direction
    ///
    enum PacketDir {
        PacketDirReceive = 0,  ///< Packet from Device
        PacketDirTransmit,     ///< Packet to Device
    };

    ///
    /// @enum  PacketType
    /// @brief DeviceHPPacket inner packet type
    ///
    enum PacketType {
        PacketTypeL2 = 0,  ///< Layer 2 packet
        PacketTypeL3,      ///< Layer 3 packet
    };

    static const int _headerSize = 8;  // 8 Bytes

 private:
    DeviceHPPacketWeakPtr _selfPtr;  ///< Weak pointer to itself

    // TBD: For now, initialize length to 1500
    uint16_t                   _totalLength{1500};
    SandboxId                  _sandboxId;     ///< Sandbox ID
    PortIndex                  _portIndex;     ///< Port Index
    DeviceHPPacket::PacketDir  _pktDir;        ///< Packet Direction
    DeviceHPPacket::PacketType _innerPktType;  ///< Inner Packet Type

    //
    // Format of packet data buffer
    // +------------+---------------~~~--------------+
    // |   Header   |              Data /            |
    // | (8 bytes)  |            Inner Packet        |
    // +------------+----------------~~~-------------+
    //
    //
    // Header:
    //  0                   1                   2                   3
    //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |Version|D|    Reserved         |          Total Length         |
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // |         Sandbox Index         |         Port Index            |
    // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // Ver           : 4 bits  < Version
    // Direction     : 1 bit
    //   PacketDirReceive(0)  - From PktIO To AFI Client (punt)
    //   PacketDirTransmit(1) - From AFI Client to PktIO (inject)
    // Res           :  3 bits < Reserved
    // Reason        :  8      < Reason
    // Total length  : 16 bits < Total length of packet including header
    // Sandbox Id    : 16 bits < Sandbox Id
    // Port Index    : 16 bits < Port Index
    //

    uint8_t *_pktDataBuffer{nullptr};  ///< Packet Data Buffer

 public:
    //
    // Constructor and destructor
    //
    ///
    /// @brief              Default constructor for DeviceHPPacket
    ///
    DeviceHPPacket() = default;
    ~DeviceHPPacket()
    {
        if (_pktDataBuffer) {
            delete[] _pktDataBuffer;
        }
    }

    ///
    /// @brief                 Factory method to create packet for transmit
    /// @param [in] dataSize   Packet data length excluding header
    /// @param [in] sandboxId  Sandbox Id
    /// @param [in] portIndex  Port index
    /// @param [in] packetType Packet Type
    /// @returns               Return Aft packet shared pointer
    ///
    static DeviceHPPacketPtr createTransmit(
        uint16_t dataSize, SandboxId sandboxId, PortIndex portIndex,
        DeviceHPPacket::PacketType packetType);

    ///
    /// @brief                 Factory method to create packet for receive
    /// @param [in] dataSize   Packet data length excluding header
    /// @returns               Return Aft packet shared pointer
    ///
    static DeviceHPPacketPtr createReceive(uint16_t dataSize);

    //
    // Accessors
    //

    /// @brief Packet data size
    // void     setTotalLength(uint16_t len) { _totalLength = len; }
    // uint16_t totalLength() const { return _totalLength; }

    /// @returns Sandbox Id
    SandboxId sandboxId() const { return _sandboxId; }

    /// @returns Port index
    PortIndex portIndex() const { return _portIndex; }

    /// @returns Packet direction
    DeviceHPPacket::PacketDir packetDir() const { return _pktDir; }

    /// @returns Inner packet type
    DeviceHPPacket::PacketType innerPacketType() const
    {
        return _innerPktType;
    };

    /// @returns Start of packet data buffer
    uint8_t *header() const { return &_pktDataBuffer[0]; }

    /// @returns Header size
    int headerSize() const { return (_headerSize); }

    /// @returns Start of packer data
    uint8_t *data() const { return &_pktDataBuffer[headerSize()]; }

    /// @returns Packet data size
    int dataSize() const { return (_totalLength - headerSize()); }

    /// @returns Packet size
    int size() const { return (_totalLength); }  // TBD: Revisit

    /// @brief Serializes header
    void headerSerialize();

    /// @brief Parses header
    void headerParse();
};

///
/// @}
///

#endif  // __DeviceHPPacket__
