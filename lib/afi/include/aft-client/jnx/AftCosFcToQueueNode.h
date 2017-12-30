///
///  @file   AftCosFcToQueueNode.h
///  @brief  Forwarding Class to Queue Mapping node
///
///  Created by Barry Burns on 3/9/17.
///  Copyright © 2017 Juniper Networks Inc. All rights reserved.
///

#ifndef _AftCosFcToQueueNode_h_
#define _AftCosFcToQueueNode_h_

#include <iostream>
#include <vector>
#include "jnx/AftNode.h"
#include "jnx/AftStatus.h"
#include "AftCosCommon.h"

using namespace CosCommon;

///
/// @class   AftCosFcToQueueEntry
/// @brief   CoS Forwarding Class to Queue Entry
///          Maps one Forwading class to a queue number
///
class AftCosFcToQueueEntry {
protected:
    ///
    /// Class Members
    ///
    u_int8_t                   _queueNum;
    u_int8_t                   _restrictedQueue;
    AftCosFcPolicingPriority   _policingPriority;
    AftCosFcSpuPriority        _spuPriority;

public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftCosFcToQueueEntry() :
        _queueNum(AFT_COS_MAX_FORW_CLASS),
        _restrictedQueue(AFT_COS_MAX_FORW_CLASS),
        _policingPriority(AftCosFcPolicingPriority::Normal),
        _spuPriority(AftCosFcSpuPriority::Low)
    { }

    AftCosFcToQueueEntry(u_int8_t qnum, u_int8_t resQ,
                         AftCosFcPolicingPriority polPrio,
                         AftCosFcSpuPriority spuPrio) :
            _queueNum(qnum),
            _restrictedQueue(resQ),
            _policingPriority(polPrio),
            _spuPriority(spuPrio)
    { }


    ///
    /// Accessor Methods
    ///
    u_int8_t queueNumGet() const { return _queueNum; }
    u_int8_t restrictedQueueGet() const { return _restrictedQueue; }
    AftCosFcPolicingPriority policingPriorityGet() const {
        return _policingPriority; }
    AftCosFcSpuPriority spuPriorityGet() const { return _spuPriority; }

    ///
    /// Operators
    ///
    AftCosFcToQueueEntry& operator=(const AftCosFcToQueueEntry& );
    bool operator==(const AftCosFcToQueueEntry& ) const;
    bool operator!=(const AftCosFcToQueueEntry& ) const;

}; // end of AftCosFcToQueueEntry


class AftCosFcToQueueNode;
using AftCosFcToQueueNodePtr = std::shared_ptr<AftCosFcToQueueNode>;
using AftCosFcToQueueV = std::vector<AftCosFcToQueueEntry>;

///
/// @class   AftCosFcToQueueNode
/// @brief   CoS Forwarding Class to Queue AFT Node
///          The AFT node contains a vector of AftCosFcToQueueEntrys.
///          The vector is indexed by the Forwarding Class ID, 0-15.
///
class AftCosFcToQueueNode : public AftNode {
protected:
    ///
    /// Class Variables
    ///
    AftCosFcToQueueV  _fcEntryVector; // Vector indexed by FC ID

public:
    ///
    /// @brief Factory generate a FC To Queue Node Shared Pointer
    ///
    static AftCosFcToQueueNodePtr create(AftCosFcToQueueV &&fcV) {
        return std::make_shared<AftCosFcToQueueNode>(std::move(fcV));
    }

    ///
    /// @brief Constructor/Destructor
    ///
    AftCosFcToQueueNode(AftCosFcToQueueV &&fcV) :
        _fcEntryVector(std::move(fcV)) { }

    ///
    /// @ returns true if node is valid
    ///
    virtual bool nodeIsValid(const AftValidatorPtr &validator,
                             std::ostream          &os) {
        //
        // Validate Node Parameters
        //
        if (_fcEntryVector.size() > AFT_COS_MAX_FORW_CLASS) {
            AftCosError() << "Forwarding Class Vector too large: act="
                << _fcEntryVector.size() << " max="
                << AFT_COS_MAX_FORW_CLASS << std::endl;
            return false;
        }
        int fc = 0;
        for (auto &entry : _fcEntryVector) {
            if (entry.queueNumGet() >= AFT_COS_MAX_NUM_QUEUES) {
                AftCosError() << "FC=" << fc
                    << ": Queue number out of range: queue="
                    << int(entry.queueNumGet())
                    << " max=" << (AFT_COS_MAX_NUM_QUEUES - 1) << std::endl;
                return false;
            }
            if (entry.restrictedQueueGet() >= AFT_COS_MAX_NUM_QUEUES) {
                AftCosError() << "FC=" << fc
                    << ": Restricted Queue number out of range: queue="
                    << int(entry.restrictedQueueGet())
                    << " max=" << (AFT_COS_MAX_NUM_QUEUES - 1) << std::endl;
                return false;
            }
            fc++;
        }
        return true;
    }

    ///
    /// Accessor Methods
    ///
    AftStatus  fcEntryGet(u_int8_t                 fc,
                          u_int8_t                 &queue,
                          u_int8_t                 &resQ,
                          AftCosFcPolicingPriority &policingPrio,
                          AftCosFcSpuPriority      &spuPrio);

    u_int8_t   numberEntriesGet() { return _fcEntryVector.size(); }
    std::ostream & descriptionFormatted(std::ostream &os) const;
    std::ostream & descriptionBrief (std::ostream &os) const;

    ///
    /// Virtual AFT Node Functions
    ///
    virtual const std::string nodeType() const { return "AftCosFcToQueue"; };

    virtual std::ostream & description(std::ostream &os) const;
}; // end of AftCosFcToQueueNode

#endif // _AftCosFcToQueueNode_h_
