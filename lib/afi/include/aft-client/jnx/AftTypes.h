//
/// @file   AftTypes.h
/// @brief  Fundamental type definitions used by Aft
//
//  Copyright (c) Juniper Networks, Inc., [2017].
//
//  All rights reserved.
//
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftTypes__
#define __AftTypes__

#include <iostream>
#include <iomanip> 
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <regex>

#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

//
// Simple classes and typedefs
//
class AftProto;
class AftGroup;
class AftNode;
class AftEntry;
class AftDeleteNode;
class AftPort;
class AftSandboxCore;
class AftSandbox;
class AftInfo;
class AftActive;
class AftOperation;
class AftTransport;
class AftTransportSession;
class AftReceiver;
class AftKey;
class AftInsert;
class AftRemove;
class AftSandboxInfo;
class AftSandboxFind;
class AftNodeTest;
class AftEntryTest;
class AftNodeInfo;
class AftNodeActive;
class AfiClient;
class AfiServer;
class AfiSandboxRpc;
class AfiNodeRpc;
class AfiEntryRpc;

///
/// @addtogroup   AftFieldKey
/// @{
///
typedef std::vector<AftKey> AftKeyVector; ///< Type of std::vector used for all lists of keys

///
/// @}
///

///
/// @addtogroup AftNodes
/// @{
///
typedef uint64_t AftNodeToken;  ///< Typedef of 64 bit scalar used to define all unique node tokens
typedef std::shared_ptr<AftNode>        AftNodePtr;  ///< Pointer type for all Aft nodes
typedef std::shared_ptr<AftDeleteNode>  AftDeleteNodePtr;  ///< Pointer type for all Aft delete nodes
typedef std::shared_ptr<AftPort>        AftPortPtr;  ///< Shared pointer type for port instances
typedef std::shared_ptr<AftInsert>      AftInsertPtr; ///< Shared pointer type for inserts
typedef std::shared_ptr<AftRemove>      AftRemovePtr; ///< Shared pointer type for removes
typedef std::shared_ptr<AftNodeTest>    AftNodeTestPtr; ///< Shared pointer type for node test
typedef std::shared_ptr<AftEntryTest>   AftEntryTestPtr; ///< Shared pointer type for entry test
typedef std::shared_ptr<AftSandboxInfo> AftSandboxInfoPtr; ///< Shared pointer type for sandbox info
typedef std::shared_ptr<AftSandboxFind> AftSandboxFindPtr; ///< Shared pointer type for sandbox find
typedef std::shared_ptr<AftNodeInfo>    AftNodeInfoPtr; ///< Shared pointer type for node info
typedef std::shared_ptr<AftNodeActive>  AftNodeActivePtr; ///< Shared pointer type for node active state
typedef std::shared_ptr<AftEntry>       AftEntryPtr; ///< Pointer type for all Aft entries
typedef std::vector<AftNodeToken> AftTokenVector; ///< Typedef of vector used to manage all lists of AftNodeToken values
typedef std::vector<AftNodePtr>   AftNodeVector;  ///< Typedef of vector used to manage all lists of AftNodePtr values
typedef std::vector<AftEntryPtr>  AftEntryVector; ///< Typedef of vector used to manage all lists of AftEntryPtr values
typedef std::vector<AftProto>     AftProtoVector; ///< Typedef of std::vector used for all lists of protos
typedef std::vector<AftGroup>     AftGroupVector; ///< Typedef of std::vector used for all lists of Groups

///
/// @}
///

typedef uint64_t AftMask;   ///< Typedef of 64 bit scalar used to mask operations
typedef uint32_t AftLength; ///< General type used for all Aft length variables
typedef uint64_t AftIndex;  ///< General type used for Aft indicies
typedef uint32_t AftTypeIndex; ///< General type used for all Aft type indicies
typedef uint32_t AftGroupIndex; ///< Typedef for node groups
typedef uint32_t AftProtoIndex; ///< Typedef for protocol indicies
typedef uint32_t AftFieldIndex; ///< Typedef for field indicies
typedef uint32_t AftEncapIndex; ///< Typedef for encap indicies
typedef uint32_t AftDecapIndex; ///< Typedef for decap indicies

///
/// @addtogroup AftSandbox
/// @{
///
typedef std::shared_ptr<AftSandboxCore> AftSandboxCorePtr;     ///< Pointer type of all AftSandboxCore instances
typedef std::shared_ptr<AftSandbox>     AftSandboxPtr;         ///< Pointer type of all AftSandbox instances
typedef std::weak_ptr<AftSandbox>       AftSandboxWeakPtr;     ///< Pointer type of all AftSandbox instances

///
/// @}
///

///
/// @addtogroup  AftTransport
/// @{
///
typedef std::shared_ptr<AftTransport> AftTransportPtr; ///< Pointer type of all AftTransports
typedef std::shared_ptr<AftReceiver>  AftReceiverPtr;  ///< Pointer type of all AftReceivers
typedef std::shared_ptr<AftTransportSession> AftTransportSessionPtr; ///< Pointer type of all AftTransportSessions

///
/// @}
///

///
/// @addtogroup AftOperation
/// @{
///
typedef std::shared_ptr<AftOperation> AftOperationPtr;     ///< Pointer type of all AftOperation instances
typedef std::unique_ptr<AftInfo>      AftInfoUPtr;         ///< Pointer type of all AftInfo instances
typedef std::unique_ptr<AftActive>    AftActiveUPtr;       ///< Pointer type of all AftActive instances

typedef std::function<void(const AftOperationPtr &op)> AftOperationResponse;
typedef std::function<void(const AftOperationPtr &op)> AftTelemetryResponse;
typedef std::vector<AftOperationPtr> AftOperationVector;

///
/// @}
///

///
/// @addtogroup AftReplicate
/// @{
///

typedef std::bitset<64>     AftRepFlags;

///
/// @}
///

#endif
