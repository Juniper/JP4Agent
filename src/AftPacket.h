//
/// @file   AftPacket.h
/// @brief  AftPacket class definition
//
// Created by Sandesh Kumar Sodhi, December 2017
// Copyright (c) [2017] Juniper Networks, Inc. All rights reserved.
//
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms. Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftPacket__
#define __AftPacket__

#include "jnx/AftTypes.h"
#include "jnx/AftData.h"

// TBD : Change them to enum/const?
#define AFI_PACKET_HDR_VERSION             0
#define AFT_CLIENT_HOSTPATH_PORT           8001
#define AFT_PACKETIO_HOSTPATH_PORT_STR    "8002"


class AftPacket;

typedef uint16_t AftSandboxId; ///< Typedef of 16 bit scalar used to define all unique sandbox IDs

///
/// @addtogroup AftPacket
/// @{
///
using AftPacketPtr = std::shared_ptr<AftPacket>;   ///< Pointer type of all AftPacket instances
using AftPacketWeakPtr = std::weak_ptr<AftPacket>; ///< Pointer type of all AftPacket instances

///
/// @}
///


///
/// @addtogroup AftPacket pkt
/// @brief Classes for accessing and managing Aft packets
/// @{
///

///
/// @class   AftPacket
/// @brief   Primary class for Aft Packets
///
class AftPacket {
public:
    ///  
    /// @enum  PacketDir
    /// @brief AftPacket packet direction
    ///  
    enum PacketDir {
        PacketDirReceive = 0, ///< Aft Sandbox to Aft Client
        PacketDirTransmit,    ///< Aft Client to Aft Sandbox
    };

    ///  
    /// @enum  PacketType
    /// @brief AftPacket inner packet type
    ///  
    enum PacketType {
        PacketTypeL2 = 0, ///< Layer 2 packet
        PacketTypeL3,     ///< Layer 3 packet
    };

    static const int _headerSize = 8;  // 8 Bytes

private:
    AftPacketWeakPtr  _selfPtr;     ///< Weak pointer to itself

    // TBD: For now, initialize length to 1500
    AftLength                 _totalLength{1500};
    AftSandboxId              _sandboxId;      ///< Sandbox ID
    AftIndex                  _portIndex;      ///< Port Index
    AftPacket::PacketDir      _pktDir;         ///< Packet Direction
    AftPacket::PacketType     _innerPktType;   ///< Inner Packet Type

    //
    // Format of Aft packet data buffer
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

    uint8_t                  *_pktDataBuffer{nullptr};  ///< Packet Data Buffer

public:

    //
    // Constructor and destructor
    //
    ///
    /// @brief              Default constructor for AftPacket
    ///
    AftPacket() = default;
    ~AftPacket()
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
    static AftPacketPtr createTransmit (AftLength dataSize,
                                        AftSandboxId sandboxId,
                                        AftIndex portIndex,
                                        AftPacket::PacketType packetType);

    ///
    /// @brief                 Factory method to create packet for receive
    /// @param [in] dataSize   Packet data length excluding header
    /// @returns               Return Aft packet shared pointer 
    ///
    static AftPacketPtr createReceive (AftLength dataSize);

    //
    // Accessors
    //

    /// @brief Packet data size
    //void     setTotalLength(uint16_t len) { _totalLength = len; }
    //uint16_t totalLength() const { return _totalLength; }

    /// @returns Sandbox Id
    AftSandboxId sandboxId() const { return _sandboxId; };

    /// @returns Port index
    AftIndex portIndex() const { return _portIndex; };

    /// @returns Packet direction
    AftPacket::PacketDir packetDir() const { return _pktDir; };

    /// @returns Inner packet type
    AftPacket::PacketType innerPacketType() const { return  _innerPktType; };

    /// @returns Start of packet data buffer
    uint8_t *header()  const { return &_pktDataBuffer[0]; }

    /// @returns Header size
    int      headerSize() const { return (_headerSize); }

    /// @returns Start of packer data
    uint8_t *data () const { return &_pktDataBuffer[headerSize()]; }

    /// @returns Packet data size
    int      dataSize() const { return (_totalLength - headerSize()); }

    /// @returns Packet size
    int      size() const { return(_totalLength); }  //TBD: Revisit

    /// @brief Serializes header
    void     headerSerialize();

    /// @brief Parses header
    void     headerParse();
};

///
/// @}
///
 
#endif // __AftPacket__

