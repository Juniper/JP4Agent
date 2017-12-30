///
/// @file   AftL2Node.h
/// @brief  Definitions for L2 (bridging) specific generic Aft Nodes
///
///  Created by Jagadish Grandhi on 7/7/17.
///  Copyright © 2017 Juniper Networks Inc. All rights reserved.
///

//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//

#ifndef __AftL2Node__
#define __AftL2Node__

#include "jnx/AftNode.h"
#include "jnx/AftEntry.h"

///
/// @addtogroup AftL2Node
/// @brief Classes used to define Aft Layer2/Bridging support
/// @{
///

///
/// @class AftBridgeDomain
/// @brief Node used to define the Bridge domain data (BD entry)
///
class AftBridgeDomain : public AftNode {
  public:
    ///
    /// @enum BdFlagBpos
    /// @brief Bit position for Bridge Domain specific flags
    ///
    enum BdFlagBpos {
        arpSuppression = 0, ///< Set if ARP requests to be suppressed
        v4McastToIrb,       ///< Set for IPv4 multicast forwarding on IRB
        v6McastToIrb,       ///< Set for IPv6 multicast forwarding on IRB
        ftf,                ///< Set when FTF is configured on this BD
        outputFtf,          ///< Set when egress FTF is configured
        macStats,           ///< Set when SMAC/DMAC accounting is configured
        learn,              ///< Set if learning is enabled on this BD
        localSwitching,     ///< Set if CE->CE traffic switching is allowed
        mcastSnooping,      ///< Set if IGMP snooping is turned ON
        macLimitHit,        ///< Set when MACs learnt exceeds BD's limit
        mlpSendToFab,       ///< To force a fabric-hop for egress leg
        debugStats,         ///< Set if collision related accounting is ON
    };

  protected:
    uint32_t    _bdIndex;     ///< Bridge Domain (BD) Identifier
    uint32_t    _bdFlags;     ///< Bridge Domain specific flags

    /// @brief Bridge Domain config data used during L2 packet handling
    uint32_t _numMeshGroup;   ///< Number of mesh-groups required for this BD
    uint32_t _macLimitAction; ///< Forwarding action on reaching MAC limit
    uint32_t _smacAgingTime;  ///< Duration for SMAC aging

    /// @brief Aft objects referenced by datapath
    AftNodeToken _irbStart; ///< Token for IRB ifl's features
    AftNodeToken _mlpFlood; ///< Token for MLP forwarding
    AftNodeToken _ftfStart; ///< Token for firewall nexthop
    AftNodeToken _outputFtfStart; ///< Token for output firewall nexthop
    AftNodeToken _learnCounter; ///< Token for learn counter block
    AftNodeToken _debugCounter; ///< Token for learn counter block

    /// @brief Bridge Domain data to facilitate datapath operation
    uint32_t _dmacAgingTime; ///< Duration for DMAC aging

  public:

    /// @brief Pointer type for AftBridgeDomain
    using Ptr = std::shared_ptr<AftBridgeDomain>;

    ///
    /// @brief Factory constructor for AftBridgeDomain
    ///
    static AftBridgeDomain::Ptr create (const uint32_t     newBdIndex,
                                        const uint32_t     newBdFlags,
                                        const uint32_t     newNumMeshGroup,
                                        const uint32_t     newMacLimitAction,
                                        const uint32_t     newSmacAgingTime,
                                        const AftNodeToken newIrbStart,
                                        const AftNodeToken newMlpFlood,
                                        const AftNodeToken newFtfStart,
                                        const AftNodeToken newOutputFtfStart,
                                        const AftNodeToken newLearnCounter,
                                        const AftNodeToken newDebugCounter,
                                        const uint32_t     newDmacAgingTime) {
        return std::make_shared<AftBridgeDomain>(newBdIndex,
                                                 newBdFlags,
                                                 newNumMeshGroup,
                                                 newMacLimitAction,
                                                 newSmacAgingTime,
                                                 newIrbStart,
                                                 newMlpFlood,
                                                 newFtfStart,
                                                 newOutputFtfStart,
                                                 newLearnCounter,
                                                 newDebugCounter,
                                                 newDmacAgingTime);
    }

    ///
    /// Constructor and destructor for AftBridgeDomain
    ///
    AftBridgeDomain (const uint32_t     newBdIndex,
                     const uint32_t     newBdFlags,
                     const uint32_t     newNumMeshGroup,
                     const uint32_t     newMacLimitAction,
                     const uint32_t     newSmacAgingTime,
                     const AftNodeToken newIrbStart,
                     const AftNodeToken newMlpFlood,
                     const AftNodeToken newFtfStart,
                     const AftNodeToken newOutputFtfStart,
                     const AftNodeToken newLearnCounter,
                     const AftNodeToken newDebugCounter,
                     const uint32_t     newDmacAgingTime)
        : AftNode(),
          _bdIndex(newBdIndex),
          _bdFlags(newBdFlags),
          _numMeshGroup(newNumMeshGroup),
          _macLimitAction(newMacLimitAction),
          _smacAgingTime(newSmacAgingTime),
          _irbStart(newIrbStart),
          _mlpFlood(newMlpFlood),
          _ftfStart(newFtfStart),
          _outputFtfStart(newOutputFtfStart),
          _learnCounter(newLearnCounter),
          _debugCounter(newDebugCounter),
          _dmacAgingTime(newDmacAgingTime) {};

    ~AftBridgeDomain () {}

    ///
    /// Accessors
    ///
    uint32_t     bdIndex () const { return _bdIndex; };
    uint32_t     bdFlags () const { return _bdFlags; };
    uint32_t     numMeshGroup () const { return _numMeshGroup; };
    uint32_t     macLimitAction () const { return _macLimitAction; };
    uint32_t     smacAgingTime () const { return _smacAgingTime; };
    AftNodeToken irbStart () const { return _irbStart; };
    AftNodeToken mlpFlood () const { return _mlpFlood; };
    AftNodeToken ftfStart () const { return _ftfStart; };
    AftNodeToken outputFtfStart () const { return _outputFtfStart; };
    AftNodeToken learnCounter () const { return _learnCounter; };
    AftNodeToken debugCounter () const { return _debugCounter; };
    uint32_t     dmacAgingTime () const { return _dmacAgingTime; };

    ///
    /// @brief check, set and clear routines for Bridge Domain flags
    ///
    bool isSetBdFlag(AftBridgeDomain::BdFlagBpos flag)
    {
        return ((_bdFlags >> flag) & 0x1);
    }
    static void setBdFlag(uint32_t *flags, AftBridgeDomain::BdFlagBpos flag)
    {
        *flags |= (0x1 << flag);
    }
    static void clearBdFlag(uint32_t *flags, AftBridgeDomain::BdFlagBpos flag)
    {
        *flags &= ~(0x1 << flag);
    }

    ///
    /// @returns node type for AftBridgeDomain
    ///
    virtual const std::string nodeType () const {
        return "AftBridgeDomain";
    };

    virtual void nextNodes (AftTokenVector &nextTokens) {
        AftNode::nextNodes(nextTokens);
        nextTokens.push_back(_mlpFlood);
        nextTokens.push_back(_irbStart);
        nextTokens.push_back(_ftfStart);
        nextTokens.push_back(_outputFtfStart);
        nextTokens.push_back(_learnCounter);
        nextTokens.push_back(_debugCounter);
    };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << ", BdIndex = " << _bdIndex;
        os << ", BdFlags = " << _bdFlags;
        os << ", NumMeshGroup = " << _numMeshGroup;
        os << ", MacLimitAction = " << _macLimitAction;
        os << ", SmacAgingTime = " << _smacAgingTime << std::hex;
        os << ", IrbStart = 0x" << _irbStart;
        os << ", MlpFlood = 0x" << _mlpFlood;
        os << ", FtfStart = 0x" << _ftfStart;
        os << ", OutputFtfStart = 0x" << _outputFtfStart;
        os << ", LearnCounter = 0x" << _learnCounter;
        os << ", DebugCounter = 0x" << _debugCounter << std::dec;
        os << ", DmacAgingTime = " << _dmacAgingTime;
        return os;
    }
};

///
/// @class AftMac
/// @brief Node for Mac Entry Data in the MAC table
///
class AftMac : public AftNode {
  public:
    ///
    /// @enum MacFlagBpos
    /// @brief Bit position for Mac Entry specific flags
    ///
    enum MacFlagBpos {
        unResolved = 0,       ///< Set if this entry is for unResolved MAC
        staticMac,            ///< Set if this is a static MAC entry
        learnRetry,           ///< Set if host notification needs to be sent
        forwardStaticMacMove, ///< Set to enable forwarding on static MAC move
        useDmacToken,         ///< Set for multicast MAC entries
        l2ptAddr,             ///< Set if l2pt translation is required
        ignoreCvlan4LSBits,   ///< l2backhaul related MAC
        localChassis,         ///< Set if the MAC belongs to local chassis
        macPinning,           ///< Set if the MAC is pinned behind the IFL
    };

  protected:
    ///
    /// Fields used for searching this element/Node in the data base
    ///
    uint64_t     _mac;        ///< MAC address
    uint32_t     _bdIndex;    ///< Bridge Domain (BD) Identifier
    uint32_t     _lvlan;      ///< Learn Vlan, required in qualified learning

    ///
    /// Feilds in the data-portion of the entry
    ///
    uint32_t     _macFlags;   ///< MAC entry specific flags

    ///
    /// Info used for split-horizon checks, epoch validity etc
    ///
    uint32_t     _stpIndex;   ///< STP index of the IFL
    uint32_t     _epoch;      ///< IFL's epoch at the time of MAC installation
    uint32_t     _iflIndex;   ///< IFL behind which this MAC is hosted
    uint32_t     _mgId;       ///< MAC's mesh group

    ///
    /// Info used for forwarding traffic with DA matching this entry
    ///
    uint32_t     _pfeId;      ///< PFE hosting this MAC
    uint32_t     _vcMemberId; ///< qualifies PFE in case of virtual chassis
    AftNodeToken _layer2Nh;   ///< forwarding NH
    uint32_t     _fwdType;    ///< forwarding treatment for packets

    ///
    /// Info used by the MAC aging logic
    ///
    uint32_t     _agingShift; ///< shift value for computing MAC aging time
    uint32_t     _maxAge;     ///< useful for aging the MAC
    uint32_t     _currAge;    ///< running variable used in aging of MAC

  public:

    /// @brief Pointer type for AftMac
    using Ptr = std::shared_ptr<AftMac>;

    ///
    /// @brief Factory constructor for AftMac
    ///
    static AftMac::Ptr create (const uint64_t     newMac,
                               const uint32_t     newBdIndex,
                               const uint32_t     newLvlan,
                               const uint32_t     newMacFlags,
                               const uint32_t     newStpIndex,
                               const uint32_t     newEpoch,
                               const uint32_t     newIflIndex,
                               const uint32_t     newMgId,
                               const uint32_t     newPfeId,
                               const uint32_t     newVcMemberId,
                               const AftNodeToken newLayer2Nh,
                               const uint32_t     newFwdType,
                               const uint32_t     newAgingShift,
                               const uint32_t     newMaxAge,
                               const uint32_t     newCurrAge) {
        return std::make_shared<AftMac>(newMac,
                                        newBdIndex,
                                        newLvlan,
                                        newMacFlags,
                                        newStpIndex,
                                        newEpoch,
                                        newIflIndex,
                                        newMgId,
                                        newPfeId,
                                        newVcMemberId,
                                        newLayer2Nh,
                                        newFwdType,
                                        newAgingShift,
                                        newMaxAge,
                                        newCurrAge);
    }

    ///
    /// Constructor and destructor for AftMac
    ///
    AftMac (const uint64_t     newMac,
            const uint32_t     newBdIndex,
            const uint32_t     newLvlan,
            const uint32_t     newMacFlags,
            const uint32_t     newStpIndex,
            const uint32_t     newEpoch,
            const uint32_t     newIflIndex,
            const uint32_t     newMgId,
            const uint32_t     newPfeId,
            const uint32_t     newVcMemberId,
            const AftNodeToken newLayer2Nh,
            const uint32_t     newFwdType,
            const uint32_t     newAgingShift,
            const uint32_t     newMaxAge,
            const uint32_t     newCurrAge)
        : AftNode(),
          _mac(newMac),
          _bdIndex(newBdIndex),
          _lvlan(newLvlan),
          _macFlags(newMacFlags),
          _stpIndex(newStpIndex),
          _epoch(newEpoch),
          _iflIndex(newIflIndex),
          _mgId(newMgId),
          _pfeId(newPfeId),
          _vcMemberId(newVcMemberId),
          _layer2Nh(newLayer2Nh),
          _fwdType(newFwdType),
          _agingShift(newAgingShift),
          _maxAge(newMaxAge),
          _currAge(newCurrAge) {};

    ~AftMac () {}

    ///
    /// Accessors
    ///
    uint64_t     mac () const { return _mac; };
    uint32_t     bdIndex () const { return _bdIndex; };
    uint32_t     lvlan () const { return _lvlan; };
    uint32_t     macFlags () const { return _macFlags; };
    uint32_t     stpIndex () const { return _stpIndex; };
    uint32_t     epoch () const { return _epoch; };
    uint32_t     iflIndex () const { return _iflIndex; };
    uint32_t     mgId () const { return _mgId; };
    uint32_t     pfeId () const { return _pfeId; };
    uint32_t     vcMemberId () const { return _vcMemberId; };
    AftNodeToken layer2Nh () const { return _layer2Nh; };
    uint32_t     fwdType () const { return _fwdType; };
    uint32_t     agingShift () const { return _agingShift; };
    uint32_t     maxAge () const { return _maxAge; };
    uint32_t     currAge () const { return _currAge; };

    ///
    /// @brief check, set and clear routines for Mac flags
    ///
    bool isSetMacFlag(AftMac::MacFlagBpos flag)
    {
        return ((_macFlags >> flag) & 0x1);
    }
    static void setMacFlag(uint32_t *flags, AftMac::MacFlagBpos flag)
    {
        *flags |= (0x1 << flag);
    }
    static void clearMacFlag(uint32_t *flags, AftMac::MacFlagBpos flag)
    {
        *flags &= ~(0x1 << flag);
    }

    ///
    /// @returns node type for AftMac
    ///
    virtual const std::string nodeType () const {
        return "AftMac";
    };

    virtual void nextNodes (AftTokenVector &nextTokens) {
        AftNode::nextNodes(nextTokens);
        nextTokens.push_back(_layer2Nh);
    };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << std::hex << ", Mac = 0x" << _mac << std::dec;
        os << ", BdIndex = " << _bdIndex;
        os << ", Lvlan = " << _lvlan;
        os << ", MacFlags = " << _macFlags;
        os << ", StpIndex = " << _stpIndex;
        os << ", Epoch = " << _epoch;
        os << ", IflIndex = " << _iflIndex;
        os << ", MgId = " << _mgId;
        os << ", PfeId = " << _pfeId;
        os << ", VcMemberId = " << _vcMemberId << std::hex;
        os << ", Layer2Nh = 0x" << _layer2Nh << std::dec;
        os << ", FwdType = " << _fwdType;
        os << ", AgingShift = " << _agingShift;
        os << ", MaxAge = " << _maxAge;
        os << ", CurrAge = " << _currAge;
        return os;
    }
};

///
/// @}
///

#endif
