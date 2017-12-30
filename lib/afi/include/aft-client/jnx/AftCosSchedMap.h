///
///  @file   AftCosSchedMap.h
///  @brief AFT CoS Scheduler Map Node classes
///
///  Created by Barry Burns on 3/9/17.
///  Copyright © 2017 Juniper Networks Inc. All rights reserved.
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftCosSchedMap__
#define __AftCosSchedMap__

#include <iostream>
#include <list>
#include <map>
#include "jnx/AftNode.h"
#include "AftCosCommon.h"

using namespace CosCommon;

///
/// @class  AftCosSchedMapRate
/// @brief  Class used to define a rate value: scalar, type and burst Size
///
class AftCosSchedMapRate {
protected:
    ///
    /// Class Variables
    ///
    CosCommon::AftCosTxType     _type;     // Specifies the format of rate field
    CosCommon::AftCosTxModifier _modifier; // exact, rate-limit, etc.
    u_int64_t                   _rate;     // Rate value
    u_int64_t                   _burstSize; // Burst Size in Bytes

public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftCosSchedMapRate()  : _type(AftCosTxType::Unknown),
                            _modifier(AftCosTxModifier::None),
                            _rate(0ULL),
                            _burstSize(0ULL) { }

    AftCosSchedMapRate(const CosCommon::AftCosTxType     newType,
                       const u_int64_t                   newRate,
                       const u_int64_t                   newBS,
                       const CosCommon::AftCosTxModifier newMod =
                       AftCosTxModifier::None)  :
                            _type(newType),
                            _modifier(newMod),
                            _rate(newRate),
                            _burstSize(newBS) { }

    ///
    /// Accessor Methods
    ///
    void      get(u_int64_t &rate, CosCommon::AftCosTxType &type,
                  u_int64_t &bs, CosCommon::AftCosTxModifier &mod) const;
    void      set(u_int64_t newRate, CosCommon::AftCosTxType newType,
                  u_int64_t newBS, CosCommon::AftCosTxModifier newMod =
                  AftCosTxModifier::None);
    CosCommon::AftCosTxType getType(void) const { return _type;};
    u_int64_t getRate(void) const { return _rate;};
    u_int64_t absRateGet(u_int64_t maxRate) const;
    void rateStrGet(std::stringstream &str, u_int8_t percision) const;
    void burstSizeStrGet(std::stringstream &str, u_int8_t percision) const;
    u_int64_t burstSizeGet(void) const { return _burstSize; };
    CosCommon::AftCosTxModifier modifierGet(void) { return _modifier; };
    bool isValid(std::ostream &os) {
        if (_type == AftCosTxType::Percentage && _rate > 100ULL) {
            AftCosError()
                << "Rate Percentage Value must be less than or equal to 100: "
                << "act=" << _rate << std::endl;
            return false;
        } // end percentage greater than 100
        return true;
    }

    ///
    /// Operator Methods
    ///
    bool operator==(const AftCosSchedMapRate &t) const;
    bool operator!=(const AftCosSchedMapRate &t) const;
    friend std::ostream &operator<< (std::ostream &os,
                                     const AftCosSchedMapRate &r) {
        return r.description(os);
    }

    std::ostream & description(std::ostream &os, u_int8_t percision=1) const;
};

class AftCosSchedMapQ;
using AftCosSchedMapQPtr = std::shared_ptr<AftCosSchedMapQ>;
using AftWredTokenMap = std::map<CosCommon::AftCosLossPriority, AftNodeToken>;
///
/// @class  AftCosSchedMapQ
/// @brief  Part of Scheduler Map that represents an individual queue.
///         Contains all the CoS configuration for said queue.
///
class AftCosSchedMapQ {
protected:
    ///
    /// Class Variables
    ///
    CosCommon::AftCosPriority  _priority;        // Queue Prioirty
    CosCommon::AftCosExcessPriority  _excessPriority;  // Queue Excess Prioirty
    AftCosSchedMapRate         _guaranteedRate;  // Guaranteed Rate
    AftCosSchedMapRate         _maximumRate;     // Maximum Rate
    AftCosSchedMapRate         _excessRate;      // Excess Rate
    CosCommon::AftCosBfrType   _bufferType;      // Specifies format of Buffer
    u_int64_t                  _bufferValue;          // Buffer Value

    //
    // WRED Curves Token Array - Key is loss Priority
    //
    AftWredTokenMap        _wredLossPriorityTokens;

public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftCosSchedMapQ() : _priority(AftCosPriority::Low),
                        _excessPriority(AftCosExcessPriority::Low),
                        _guaranteedRate(), _maximumRate(), _excessRate(),
                        _bufferType(AftCosBfrType::Remainder),
                        _bufferValue(0) { }

    AftCosSchedMapQ(const CosCommon::AftCosPriority     newPriority,
                    const CosCommon::AftCosExcessPriority newExcessPriority,
                    const AftCosSchedMapRate           &newGRate,
                    const AftCosSchedMapRate           &newMRate,
                    const AftCosSchedMapRate           &newERate,
                    const CosCommon::AftCosBfrType      newBfrType,
                    const u_int64_t                     newBfrVal)  :
                        _priority(newPriority),
                        _excessPriority(newExcessPriority),
                        _guaranteedRate(newGRate), _maximumRate(newMRate),
                        _excessRate(newERate),
                        _bufferType(newBfrType), _bufferValue(newBfrVal) { }


    ///
    /// Accessor Methods
    ///
    const AftCosSchedMapRate & guaranteedRateGet() const {
        return _guaranteedRate; }
    void guaranteedRateSet(AftCosSchedMapRate &r) { _guaranteedRate = r; }
    const AftCosSchedMapRate & maximumRateGet() const { return _maximumRate;}
    void maximumRateSet(AftCosSchedMapRate &r) { _maximumRate = r; }
    const AftCosSchedMapRate & excessRateGet() const { return _excessRate;}
    void excessRateSet(AftCosSchedMapRate &r) { _excessRate = r; }
    CosCommon::AftCosBfrType bufferTypeGet() const { return _bufferType; }
    void bufferTypeSet(CosCommon::AftCosBfrType t) { _bufferType = t; }
    u_int64_t bufferValueGet() const { return _bufferValue; }
    u_int64_t
        absBufferValueGet(u_int64_t ifRate,
                          u_int64_t defDbbTimeUs = (DEFAULT_BFR_TIME_MS *
                                                    COS_USEC_PER_MSEC)) const;
    void bufferValueSet(u_int64_t v) { _bufferValue = v; }
    CosCommon::AftCosPriority priorityGet() const { return _priority;}
    void prioritySet(CosCommon::AftCosPriority p) { _priority = p; }
    CosCommon::AftCosExcessPriority excessPriorityGet() const { return _excessPriority;}
    void excessPrioritySet(CosCommon::AftCosExcessPriority p) { _excessPriority = p; }
    void wredLossPrioritySet(AftCosLossPriority lp, AftNodeToken crvToken) {
        _wredLossPriorityTokens[lp] = crvToken;
    }
    AftNodeToken wredLossPriorityGet(AftCosLossPriority lp) const;
    u_int32_t wredLossPriorityNumGet() const {
        return (u_int32_t) _wredLossPriorityTokens.size(); }
    const AftWredTokenMap & wredLossPriorityMapGet() const {
        return _wredLossPriorityTokens;}

    bool isValid(std::ostream &os) {
        u_int64_t        gRate, mRate, bs;
        AftCosTxModifier gMod, mMod;
        AftCosTxType     gType, mType;

        if (!CosCommon::priorityValid(_priority)) {
            AftCosError()
                << int(CosCommon::enumAsValue(_priority)) << std::endl;
            return false;
        } // end invalid priority
        if (!CosCommon::excessPriorityValid(_excessPriority)) {
            AftCosError()
                << int(CosCommon::enumAsValue(_excessPriority)) << std::endl;
            return false;
        } // end invalid excess priority
        if (_bufferType == AftCosBfrType::Percentage) {
            if (_bufferValue == 0ULL || _bufferValue > 100ULL) {
                AftCosError()
                    << "Percentage Buffer Value must be greater than 0 and "
                    << "less than or equal to 100: act=" << _bufferValue
                    << std::endl;
                return false;
            }
        } // end buffer percentage
        if (_bufferType == AftCosBfrType::Temporal) {
            if (_bufferValue == 0ULL) {
                AftCosError()
                    << "Temproral Buffer Value must be greater than 0"
                    << ": act=" << _bufferValue << std::endl;
                return false;
            }
        } // end buffer percentage
        if (!_guaranteedRate.isValid(os)) {
            AftCosError()
                << "Guaranteed Rate is invalid rate="
                << _guaranteedRate << std::endl;
            return false;
        } // end grate
        _guaranteedRate.get(gRate, gType, bs, gMod);
        _maximumRate.get(mRate, mType, bs, mMod);
        if ((gMod == AftCosTxModifier::Exact ||
             gMod == AftCosTxModifier::RateLimit)) {
            if (mRate != 0) {
                //
                // Cannot use Guaranteed Rate modifier "Exact" or "RateLimit"
                // AND specify a Maximum Rate
                //
                AftCosError()
                    << "Illegal Config: Guaranteed Rate Modifier is "
                    << CosCommon::rateModifierString(gMod)
                    << " AND a Maximum Rate was specified." << std::endl;
                return false;
            }
            if (gType == AftCosTxType::Remainder) {
                //
                // Cannot use Guaranteed Rate modifier "Exact" or "RateLimit"
                // AND Type is REMAINDER
                //
                AftCosError()
                    << "Illegal Config: Guaranteed Rate Modifier is "
                    << CosCommon::rateModifierString(gMod)
                    << " AND a Type was specified as Remainder." << std::endl;
                return false;
            }
        }
        if (!_maximumRate.isValid(os)) {
            AftCosError()
                << "Maximum Rate is invalid rate="
                << _maximumRate << std::endl;
            return false;
        } // end grate
        if (!_excessRate.isValid(os)) {
            AftCosError()
                << "Excess Rate is invalid rate="
                << _excessRate << std::endl;
            return false;
        } // end grate
        if (_maximumRate.modifierGet() != CosCommon::AftCosTxModifier::None) {
            AftCosError()
                << "Maximum Rate cannot have modifiers:  modifier="
                << CosCommon::rateModifierString(_maximumRate.modifierGet())
                << std::endl;
            return false;
        }
        if (_excessRate.modifierGet() != CosCommon::AftCosTxModifier::None) {
            AftCosError()
                << "Excess Rate cannot have modifiers:  modifier="
                << CosCommon::rateModifierString(_excessRate.modifierGet())
                << std::endl;
            return false;
        }
        for (auto & lp : _wredLossPriorityTokens) {
            if (!CosCommon::AftLossPriorityIsValid(lp.first)) {
                AftCosError()
                    << "Invalid Loss Priority="
                    << int(CosCommon::enumAsValue(lp.first)) << std::endl;
                return false;
            }
            if (lp.second != AFT_NODE_TOKEN_NONE) {

            }
        } // end for each LP
        return true;
    }

    ///
    /// Operator Methods
    ///
    bool operator==(const AftCosSchedMapQ &data) const;
    friend std::ostream &operator<< (std::ostream &os,
                                     const AftCosSchedMapQ &q) {
        return q.description(os);
    }

    std::ostream & description(std::ostream &os) const;
};

using AftCosSchedMapQV = std::vector<AftCosSchedMapQ>;
class AftCosSchedMapNode;
using AftCosSchedMapNodePtr = std::shared_ptr<AftCosSchedMapNode>;

///
/// @class   AftCosSchedMapNode
/// @brief   CoS Scheduler Map AFT Node
///
class AftCosSchedMapNode : public AftNode {
protected:
    ///
    /// Class Variables
    ///
    /// A vector that contains q configuration
    /// Note that the translation between fcID and queue number is
    /// performed by the AftCosFcToQueueNode
    ///
    /// This vector is indexed by queue number
    ///
    AftCosSchedMapQV     _queueMapVectors;

public:
    enum {QUEUES_PER_SCHED_MAP_MAX = 8};
    ///
    /// @brief Constructor/Destructor
    ///
    AftCosSchedMapNode(AftCosSchedMapQV &&qMap) :
        _queueMapVectors(std::move(qMap)) { }

    static AftCosSchedMapNodePtr create(AftCosSchedMapQV &&qMap) {
        return std::make_shared<AftCosSchedMapNode>(std::move(qMap));
    }

    ///
    /// Accessor Methods
    ///
    unsigned int numQueuesGet() { return (unsigned int) _queueMapVectors.size();}
    const AftCosSchedMapQ & schedMapQueueGet(const u_int8_t qnum) const {
        return _queueMapVectors[qnum];
    }

    std::ostream & descriptionFormatted(std::ostream &os) const;
    std::ostream & descriptionBrief(std::ostream &os) const;

    //
    // Virtual AFT Node Functions
    //
    virtual const std::string nodeType() const { return "AftCosSchedMap"; };
    virtual void nextNodes(AftTokenVector &nextTokens);
    virtual std::ostream & description(std::ostream &os) const;
    friend std::ostream &operator<< (std::ostream &os,
                                     const AftCosSchedMapNode &n) {
        return n.description(os);
    }
    ///
    /// @function nodeIsValid
    /// @brief returns true if node is valid
    ///
    virtual bool nodeIsValid(const AftValidatorPtr &validator,
                             std::ostream          &os) {
#if 1
        //
        // Validate Node Parameters
        //
        if (_queueMapVectors.size() > QUEUES_PER_SCHED_MAP_MAX) {
            AftCosError()
                << "Queue Map Vector too large: act="
                << _queueMapVectors.size() << " max="
                << QUEUES_PER_SCHED_MAP_MAX << std::endl;
            return false;
        }
        int q = 0;
        for (auto &entry : _queueMapVectors) {
            if (!entry.isValid(os)) {
                AftCosError() << "QMAP INVALID for queue=" << q << std::endl;
                return false;
            }
            q++;
        }
#endif
        return true;
    }
};

#endif // __AftCosSchedMap__
