//
///  @file   AftCosSchedNode.h
///  @brief  AFT CoS Scheduler Node classes
//
//  Created by Barry Burns on 3/9/17.
//  Copyright © 2017 Juniper Networks Inc. All rights reserved.
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftCosSchedNode__
#define __AftCosSchedNode__

#include <iostream>
#include <list>
#include <map>
#include <algorithm>
#include "jnx/AftNode.h"
#include "AftCosCommon.h"
#include "jnx/TableMaker.h"

using namespace CosCommon;

///
/// @class   AftCosRate
/// @brief   CoS Scheduler Rate class
///
class AftCosRate {
protected:
    ///
    /// Class Variables
    ///
    u_int64_t         _rate;       // Rate value in bps, unless excess then in %
    u_int32_t         _burstSize;  // Burst Size in bytes

public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftCosRate() : _rate(0ULL), _burstSize(0) { }
    AftCosRate(u_int64_t newRate, u_int32_t newBurstSize) :
            _rate(newRate), _burstSize(newBurstSize) { }

    ///
    /// Accessor Methods
    ///
    u_int64_t rateGet() const      { return _rate; }
    void rateSet(u_int64_t r)      { _rate = r; }
    u_int32_t burstSizeGet() const { return _burstSize; }
    void burstSizeSet(u_int32_t s) { _burstSize = s; }

    ///
    /// Operator Methods
    ///
    bool operator==(const AftCosRate &t) const;
    bool operator!=(const AftCosRate &t) const;
    std::ostream & description (std::ostream &os) const;
};

///
/// @class   AftCosRate Group
/// @brief   CoS Scheduler Rate Group class
///
class AftCosRateGroup {
protected:
    ///
    /// Class Variables
    ///
    AftCosRate _nominal;            // Nominal Rate
    AftCosRate _highPriority;       // High Priority Rate
    AftCosRate _mediumHighPriority; // Medium High Priority Rate
    AftCosRate _mediumLowPriority;  // Medium Low Priority Rate
    AftCosRate _lowPriority;        // Low Priority Rate

public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftCosRateGroup() : _nominal(), _highPriority(), _mediumHighPriority(),
                        _mediumLowPriority(), _lowPriority() { }
    AftCosRateGroup(AftCosRate newNom, AftCosRate newH, AftCosRate newMH,
                    AftCosRate newML, AftCosRate newL) :
            _nominal(newNom),
            _highPriority(newH),
            _mediumHighPriority(newMH),
            _mediumLowPriority(newML),
            _lowPriority(newL) { }

    ///
    /// Accessor Methods
    ///
    AftCosRate & nomGet()          { return _nominal; }
    AftCosRate & highPrioGet()     { return _highPriority; }
    AftCosRate & medHighPrioGet()  { return _mediumHighPriority; }
    AftCosRate & medLowPrioGet()   { return _mediumLowPriority; }
    AftCosRate & lowPrioGet()      { return _lowPriority; }
    const AftCosRate & nomConstGet()         const {return _nominal; }
    const AftCosRate & highPrioConstGet()    const {return _highPriority; }
    const AftCosRate & medHighPrioConstGet() const {return _mediumHighPriority;}
    const AftCosRate & medLowPrioConstGet()  const {return _mediumLowPriority;}
    const AftCosRate & lowPrioConstGet()     const {return _lowPriority; }

    ///
    /// Operator Methods
    ///
    bool operator==(const AftCosRateGroup &t) const;

    std::ostream & description(std::ostream &os) const;

    //
    // @function  printTableRow
    // @brief     Should only be called from base class AftCosSchedNode
    //
    void printTableRow(std::ostream    &os,
                       TableMaker      &tbl,
                       std::string     rateType,
                       AftCosRateGroup const &rg) const;
};

using AftSchedChildren = std::list<AftNodeToken>;
class AftCosSchedNode;
using AftCosSchedNodePtr = std::shared_ptr<AftCosSchedNode>;

using AftRedDropArrayInt64 = const uint64_t (&)[COS_MAX_RED_STATS_PROFILES];
using AftRedDropArrayInt32 = const uint32_t (&)[COS_MAX_RED_STATS_PROFILES];

//
// AftQueueStats
//
class AftQueueStats {
protected:
    ///
    /// Class Variables
    ///
    uint64_t  _offeredLoadByteRate;
    uint64_t  _outputByteRate;
    uint32_t  _offeredLoadPktsRate;
    uint32_t  _outputPktsRate;
    uint32_t  _tailDropPktsRate;
    uint32_t  _rateLimitDropByteRate;
    uint32_t  _rateLimitDropPktsRate;
    uint32_t  _bufUsageMuByte;   /* m-used pkts buffer usage */
    uint32_t  _bufUsageMuPercent;/* m-used buf usage in percent */
    uint32_t  _bufUsageBuByte;   /* b-used pkts buffer usage */
    uint32_t _redDropPktsRate [COS_MAX_RED_STATS_PROFILES];
    uint64_t _redDropByteRate [COS_MAX_RED_STATS_PROFILES];
    uint64_t  _offeredLoadPkts;   /* packets arriving at each Q */
    uint64_t  _offeredLoadByte;   /* packets arriving at each Q */
    uint64_t  _outputPkts;         /* packet leaving from each Q */
    uint64_t  _outputByte;         /* packet leaving from each Q */
    uint64_t  _tailDropPkts;      /* tail drop count for each Q */
    uint64_t  _rateLimitDropByte;/* RL drop count for each
                                               Q(IQ2/Q2) */
    uint64_t  _rateLimitDropPkts;/* RL drop packets count for each
                                               Q(Q2) */
    uint32_t  _queueStatsFlag;    /* bit 0 is used as rate_limit flag */

    uint64_t  _redDropPkts [COS_MAX_RED_STATS_PROFILES];
    uint64_t _redDropByte [COS_MAX_RED_STATS_PROFILES];

    uint32_t  _sharedBufBytes;   /* queue's shared buff byte time avgd */
    uint32_t  _sharedBufPkts;/* queue's shared buff pkt time avgd */
    uint32_t  _reservedBufBytes;/* queue's resvd buff byte time avgd */
    uint32_t  _reservedBufPkts;/* queue's resvd buff pkt time avgd */

    /*
     * The following 4 members are part of RLI 17388:
     */
    uint32_t  _depthAverage;      /* Average queue depth */
    uint32_t  _depthCurrent;      /* Current instantaneous queue depth */
    uint32_t  _depthPeak;         /* Peak queue depth attained till now */
    uint32_t  _depthMax;          /* Maximum acceptable queue depth */

public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftQueueStats(const uint64_t  newOfferedLoadByteRate,
               const uint64_t  newOutputByteRate,
               const uint32_t  newOfferedLoadPktsRate,
               const uint32_t  newOutputPktsRate,
               const uint32_t  newTailDropPktsRate,
               const uint32_t  newRateLimitDropByteRate,
               const uint32_t  newRateLimitDropPktsRate,
               const uint32_t  newBufUsageMuByte,
               const uint32_t  newBufUsageMuPercent,
               const uint32_t  newBufUsageBuByte,
               const AftRedDropArrayInt32 newRedDropPktsRate,
               const AftRedDropArrayInt64 newRedDropByteRate,
               const uint64_t  newOfferedLoadPkts,
               const uint64_t  newOfferedLoadByte,
               const uint64_t  newOutputPkts,
               const uint64_t  newOutputByte,
               const uint64_t  newTailDropPkts,
               const uint64_t  newRateLimitDropByte,
               const uint64_t  newRateLimitDropPkts,
               const uint32_t  newQueueStatsFlag,
               const AftRedDropArrayInt64  newRedDropPkts,
               const AftRedDropArrayInt64  newRedDropByte,
               const uint32_t  newSharedBufBytes,
               const uint32_t  newSharedBufPkts,
               const uint32_t  newReservedBufBytes,
               const uint32_t  newReservedBufPkts,
               const uint32_t  newDepthAverage,
               const uint32_t  newDepthCurrent,
               const uint32_t  newDepthPeak,
               const uint32_t  newDepthMax):
                         _offeredLoadByteRate(newOfferedLoadByteRate),
                         _outputByteRate(newOutputByteRate),
                         _offeredLoadPktsRate(newOfferedLoadPktsRate),
                         _outputPktsRate(newOutputPktsRate),
                         _tailDropPktsRate (newTailDropPktsRate),
                         _rateLimitDropByteRate(newRateLimitDropByteRate),
                         _rateLimitDropPktsRate(newRateLimitDropPktsRate),
                         _bufUsageMuByte(newBufUsageMuByte),
                         _bufUsageMuPercent(newBufUsageMuPercent),
                         _bufUsageBuByte(newBufUsageBuByte),
                         _offeredLoadPkts(newOfferedLoadPkts),
                         _offeredLoadByte(newOfferedLoadByte),
                         _outputPkts(newOutputPkts),
                         _outputByte(newOutputByte),
                         _tailDropPkts(newTailDropPkts),
                         _rateLimitDropByte (newRateLimitDropByte),
                         _rateLimitDropPkts(newRateLimitDropPkts),
                         _queueStatsFlag(newQueueStatsFlag),
                         _sharedBufBytes(newSharedBufBytes),
                         _sharedBufPkts(newSharedBufPkts),
                         _reservedBufBytes(newReservedBufBytes),
                         _reservedBufPkts(newReservedBufPkts),
                         _depthAverage(newDepthAverage),
                         _depthCurrent(newDepthCurrent),
                         _depthPeak(newDepthPeak),
                         _depthMax(newDepthMax){
                             std::copy(std::begin(newRedDropPktsRate), std::end(newRedDropPktsRate), _redDropPktsRate);
                             std::copy(std::begin(newRedDropByteRate), std::end(newRedDropByteRate), _redDropByteRate);
                             std::copy(std::begin(newRedDropPkts), std::end(newRedDropPkts), _redDropPkts);
                             std::copy(std::begin(newRedDropByte), std::end(newRedDropByte), _redDropByte);
        }

        uint64_t offeredLoadByteRateGet() const { return _offeredLoadByteRate; }
        uint64_t outputByteRateGet() const {  return  _outputByteRate; }
        uint32_t  offeredLoadPktsRateGet() const { return _offeredLoadPktsRate;}
        uint32_t  outputPktsRateGet() const { return _outputPktsRate;}
        uint32_t  tailDropPktsRateGet() const { return _tailDropPktsRate;}
        uint32_t  rateLimitDropByteRateGet() const { return _rateLimitDropByteRate;}
        uint32_t  rateLimitDropPktsRateGet() const { return _rateLimitDropPktsRate;}
        uint32_t  bufUsageMuByteGet() const { return _bufUsageMuByte;}
        uint32_t  bufUsageMuPercentGet() const { return _bufUsageMuPercent;}
        uint32_t  bufUsageBuByteGet() const { return _bufUsageBuByte;}

        AftRedDropArrayInt32& redDropPktsRateGet() const {return _redDropPktsRate; }
        AftRedDropArrayInt64& redDropByteRateGet() const {return _redDropByteRate; }

        uint64_t  offeredLoadPktsGet() const { return _offeredLoadPkts;}
        uint64_t  offeredLoadByteGet() const { return _offeredLoadByte;}
        uint64_t  outputPktsGet() const { return _outputPkts;}
        uint64_t  outputByteGet() const { return _outputByte;}
        uint64_t  tailDropPktsGet() const { return _tailDropPkts;}
        uint64_t  rateLimitDropByteGet() const { return _rateLimitDropByte;}
        uint64_t  rateLimitDropPktsGet() const { return _rateLimitDropPkts;}
        uint32_t  queueStatsFlagGet() const { return _queueStatsFlag; }
        AftRedDropArrayInt64& redDropPktsGet() const {return _redDropPkts; }
        AftRedDropArrayInt64& redDropByteGet() const {return _redDropByte; }
        uint32_t  sharedBufBytesGet() const { return _sharedBufBytes;}
        uint32_t  sharedBufPktsGet() const { return _sharedBufPkts;}
        uint32_t  reservedBufBytesGet() const { return _reservedBufBytes;}
        uint32_t  reservedBufPktsGet() const { return _reservedBufPkts;}
        uint32_t  depthAverageGet() const { return _depthAverage; }
        uint32_t  depthCurrentGet() const { return _depthCurrent;}
        uint32_t  depthPeakGet() const { return _depthPeak;}
        uint32_t  depthMaxGet() const { return _depthMax; }
};

//
// AftVoqStats Classes
//
class AftVoqPfeStats {
public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftVoqPfeStats(const uint32_t  newDropPktsRate,
                   const uint32_t  newDropByteRate,
                   const uint64_t  newDropPkts,
                   const uint64_t  newDropByte):
                           _dropPktsRate(newDropPktsRate),
                           _dropByteRate(newDropByteRate),
                           _dropPkts(newDropPkts),
                           _dropByte(newDropByte){}
    uint32_t  dropPktsRateGet() const { return _dropPktsRate;}
    uint32_t  dropByteRateGet() const { return _dropByteRate;}
    uint64_t  dropPktsGet() const { return _dropPkts;}
    uint64_t  dropByteGet() const { return _dropByte;}

protected:
    ///
    /// Class Variables
    ///
    uint32_t  _dropPktsRate;
    uint32_t  _dropByteRate;
    uint64_t  _dropPkts;
    uint64_t  _dropByte;

};

class AftVoqStats {
public:
    using VoqPfeStatsVector = std::vector<AftVoqPfeStats>;
    AftVoqStats(VoqPfeStatsVector &&newVoqPfeStats)
        : _voqPfeStats(std::move(newVoqPfeStats)) {}

    const VoqPfeStatsVector& voqPfeStatsGet() const { return _voqPfeStats; }

 protected:
    VoqPfeStatsVector _voqPfeStats;
};

///
/// @class   AftCosSchedNode
/// @brief   CoS Scheduler node
///
class AftCosSchedNode : public AftNode {
protected:
    ///
    /// Class Variables
    ///
    AftNodeToken     _parentSchedToken;   ///< Token of Scheduler Parent Node
    AftNodeToken     _schedMapToken;      ///< Token of Scheduler Map
    AftNodeToken     _streamToken;        ///< Token of output stream

    bool             _enable;             ///< Interface enable
    AftCosSchedMode  _mode;               ///< Scheduler mode
    u_int32_t        _overheadAcct;       ///< Byte adjust
    u_int32_t        _dbbTimeUs = 100000; ///< Delay Bandwidth Buffer time in micro-seconds
    u_int64_t        _interfaceRateBps;   ///< Interface Rate in bps
    AftCosRate       _guaranteedRate;     ///< Guaranteed Rate in bps
    AftCosRateGroup  _excessRate;         ///< Excess Rate
    AftCosRateGroup  _maximumRate;        ///< Maximum/Shaping Rate -  The Max Rate will be used to convert all child variables that are dependent upon interface speed, sucha as an OQ's GRate specified in percentile.
    u_int32_t        _globalBaseID;       ///< For PTX, this is the base of the range of VOQ IDs to be used by the individual OQs assigned here
    int8_t           _maximumHierarchyLevels; ///< Maximum Number of hierarchy levels. Only valid for root node
    int8_t           _level;              ///< Level of this node in the SW scheduling hierarchy. Will be computed by AFT server

public:
    ///
    /// @brief Factory generate a Cos Sched Node Shared Pointer
    ///
    static AftCosSchedNodePtr create(const AftNodeToken newParentSchedToken,
                                     const AftNodeToken newSchedMapToken,
                                     const AftNodeToken newStreamToken,
                                     const bool         newEnable,
                                     const CosCommon::AftCosSchedMode newMode,
                                     const u_int32_t    newOverheadAcct,
                                     const AftCosRate   &newGRate,
                                     const AftCosRateGroup &newERate,
                                     const AftCosRateGroup &newMRate,
                                     const u_int64_t        ifRate,
                                     const u_int32_t        dbbTime = 100000,
                                     const u_int32_t        globalBaseId = 0,
                                     const int8_t           maxLevels = -1) {
        return std::make_shared<AftCosSchedNode>(newParentSchedToken,
                                                 newSchedMapToken,
                                                 newStreamToken,
                                                 newEnable,
                                                 newMode,
                                                 newOverheadAcct,
                                                 newGRate,
                                                 newERate,
                                                 newMRate,
                                                 ifRate,
                                                 dbbTime,
                                                 globalBaseId,
                                                 maxLevels);
    }

    ///
    /// @brief Constructor/Destructor
    ///
    AftCosSchedNode(const AftNodeToken    newParentSchedToken,
                    const AftNodeToken    newSchedMapToken,
                    const AftNodeToken    newStreamToken,
                    const bool            newEnable,
                    const CosCommon::AftCosSchedMode newMode,
                    const u_int32_t       newOverheadAcct,
                    const AftCosRate      &newGRate,
                    const AftCosRateGroup &newERate,
                    const AftCosRateGroup &newMRate,
                    const u_int64_t        ifRate,
                    const u_int32_t        dbbTime = 100000,
                    const u_int32_t        globalBaseId = 0,
                    const int8_t           maxLevels = -1) :
            _parentSchedToken(newParentSchedToken),
            _schedMapToken(newSchedMapToken),
            _streamToken(newStreamToken),
            _enable(newEnable),
            _mode(newMode),
            _overheadAcct(newOverheadAcct),
            _dbbTimeUs(dbbTime),
            _interfaceRateBps(ifRate),
            _guaranteedRate(newGRate),
            _excessRate(newERate),
            _maximumRate(newMRate),
            _globalBaseID(globalBaseId),
            _maximumHierarchyLevels(maxLevels),
            _level(-1) { }

    ///
    /// Accessor Methods
    ///
    AftNodeToken prtTknGet() const {return _parentSchedToken;}
    AftNodeToken schedMapTknGet() const {return _schedMapToken;}
    AftNodeToken streamTknGet() const {return _streamToken;}
    bool enableGet() const {return _enable;}
    AftCosSchedMode modeGet() {return _mode;}
    u_int32_t ovrHdAcctGet() const {return _overheadAcct;}
    u_int32_t dbbTimeUsGet() const { return _dbbTimeUs; }
    const AftCosRate & gRateGet() const {return _guaranteedRate;}
    const AftCosRateGroup & eRateGet() const {return _excessRate;}
    const AftCosRateGroup & mRateGet() const {return _maximumRate;}
    u_int32_t  globalBaseIdGet() const { return _globalBaseID; }
    u_int64_t  interfaceRateGet() const { return _interfaceRateBps; }
    int8_t     maxHierLevelsGet() const { return _maximumHierarchyLevels; }
    int8_t     levelGet() const { return _level; }
    std::ostream & descriptionFormatted(std::ostream &os) const;
    std::ostream & descriptionBrief(std::ostream &os) const;

    //
    // Note that level is AftServer only variable and is setup when the
    // node is pushed to the AFT server. Thus, we have a set function for this
    // variable.
    //
    // We should move this to Engineman Node.
    //
    void levelSet(int8_t v) { _level = v; }
    void maxHierLevelsSet(int8_t v) { _maximumHierarchyLevels = v; }

    //
    // Virtual AFT Node Functions
    //
    virtual const std::string nodeType() const { return "AftCosSched"; }
    virtual void nextNodes(AftTokenVector &nextTokens);
    virtual std::ostream & description(std::ostream &os) const;

    ///
    /// Alias for packet counter vector
    ///
    using QueueStatsVector = std::vector<AftQueueStats>;
    using VoqStatsVector = std::vector<AftVoqStats>;

    ///
    /// @class Info
    /// @brief Information return class for AftCounter
    ///
    class Info: public AftInfo {
    protected:
        QueueStatsVector _queueStats;
        VoqStatsVector _voqStats;

    public:
        Info(AftNodeToken newNodeToken, QueueStatsVector &&newQueueStats)
            : AftInfo(newNodeToken),
            _queueStats(std::move(newQueueStats)) {}

        Info(AftNodeToken newNodeToken, VoqStatsVector &&newVoqStats)
            : AftInfo(newNodeToken),
            _voqStats(std::move(newVoqStats)) {}

        ~Info() {}

        const QueueStatsVector &queueStatsGet() const { return _queueStats; }
        const VoqStatsVector &voqStatsGet() const { return _voqStats; }

        virtual const std::string infoType() const { return _voqStats.empty() ? "AftQueueStats" : "AftVoqStats"; }
    };

    virtual Info::UPtr nodeInfo(QueueStatsVector &&newQueueStats) const {
        return std::unique_ptr<AftCosSchedNode::Info>(new AftCosSchedNode::Info(_nodeToken, std::move(newQueueStats)));
    };

    virtual Info::UPtr nodeInfo(VoqStatsVector &&newVoqStats) const {
        return std::unique_ptr<AftCosSchedNode::Info>(new AftCosSchedNode::Info(_nodeToken, std::move(newVoqStats)));
    };
};

#endif
