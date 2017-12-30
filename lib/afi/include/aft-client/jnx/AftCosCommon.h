///
///  @file   AftCosCommon.h
///  @brief  Aft CoS Common header
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

#ifndef __AftCosCommon_h__
#define __AftCosCommon_h__

#include "sys/param.h"
#include "jnx/AftCommon.h"
#include "jnx/AftLog.h"
#include "jnx/AftStatus.h"
#include <bitset>

//
// Cos Logging stuff
//
extern AftLogv2 _aftCosLog;

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? \
                      __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define AftCosDebug2(mask)                                              \
    AFTLOG_MASK(_aftCosLog, AftLogSeverity::Debug2, "AFT_COS: ", mask)  \
    << __FILENAME__ << '@' << __LINE__ << ':'                           \
    << __FUNCTION__ << ": "
#define AftCosDebug(mask)                                               \
    AFTLOG_MASK(_aftCosLog, AftLogSeverity::Debug, "AFT_COS: ", mask)   \
    << __FILENAME__ << '@' << __LINE__ << ':'                           \
    << __FUNCTION__ << ": "

#define AftCosInfo() AFTLOG(_aftCosLog, AftLogSeverity::Info, "AFT_COS: ")
#define AftCosError() AFTLOG(_aftCosLog, AftLogSeverity::Error, "AFT_COS: ")

enum class AftCosDebugMasks {
    General,
    Gtests,
    //
    // Obviously the last
    //
    NumDebugs
};

namespace CosCommon {
#define COS_INVALID_INDEX                 -1U
#define AFT_COS_MAX_NUM_QUEUES             8
#define AFT_COS_MAX_FORW_CLASS            16
#define COS_MSEC_PER_SEC           (1000ULL)
#define COS_USEC_PER_MSEC          (1000ULL)
#define COS_MSEC_SIZE_GET(rate, ms)                                     \
    (((rate)*(ms)) / (COS_MSEC_PER_SEC * BITS_TO_BYTES))
#define COS_USEC_SIZE_GET(rate, us)                                     \
    (((rate)*(us)) / (COS_MSEC_PER_SEC * COS_MSEC_PER_SEC * BITS_TO_BYTES))

#define DEFAULT_BFR_TIME_MS   (100)
#define COS_MAX_RED_STATS_PROFILES      4

//
// Queue Stats Flag bits
//
#define COS_STATS_TRI_COLOR_SUPPORT      0x1

#define COS_CONV_PERC_TO_VALUE(whole, perc) (((whole) * (perc)) / 100ULL)
#define COS_WEIGHTED_VALUE_GET(tot_value, tot_wgt, wgt) \
    (((tot_value) * (wgt)) / (tot_wgt))

    enum class AftCosLossPriority : u_int8_t {
        High,           /// High
        MediumHigh,     /// Medium High
        MediumLow,      /// Medium Low
        Low,            /// Low
    };

    enum class AftCosPriority : u_int8_t {
        StrictHigh,     /// Strict High
        High,           /// High
        MediumHigh,     /// Medium High
        MediumLow,      /// Medium Low
        Low,            /// Low
        Excess,         /// Excess - i.e. no priority
        Unknown         /// Unknown
    };

    enum class AftCosFcPolicingPriority : u_int8_t {
        Normal,
        Premium
    };

    enum class AftCosFcSpuPriority : u_int8_t {
        Low,
        High
    };

    enum class AftCosTxType : u_int8_t {
        Absolute,        /// Absolute, such as rate is in bps
        Percentage,      /// Value is specified as a percemtage in 10th %
        Remainder,       /// Remainder - assign queue remaining BW
        Unknown          /// Value specified is unknown
    };

    enum class AftCosTxModifier : u_int8_t {
        None,            /// No keyword specified
        Exact,           /// "exact" keyword
        RateLimit        /// "rate-limit" keyword
    };

    enum class AftCosBfrType : u_int8_t {
        Percentage,      /// Value is specified as a percemtage in 10th %
        Temporal,        /// Temporal - value is a time (bfr only)
        Remainder,       /// Buffer size is remainder
        Shared,          /// Shared allows for Bfr to take upto IF
        Unknown          /// Unknown Buffer type
    };

    enum class AftCosSchedMode : u_int8_t {
        Nominal,          /// Nominal Scheduler Mode
        StrictPriority,  /// Strict Prioirity Scheduler mode
        Unknown
    };

    enum class AftCosClsType : u_int8_t {
        DSCP,             /// DSCP bits
        EXP,              /// EXP bits
        IEEE,             /// IEEE 802.1
        IP_PREC,          /// IP Precedence
        FIXED,            /// Fixed classification
        EXP_SPP,          /// SWAP_PUSH_PUSH
        EXP_PPP,          /// TRIPLE_PUSH
        DSCP_IPV6,        /// IPv6 DSCP bits
        FR,               /// Frame-relay DE bit
        IEEE_UNTRUST,     /// Default Untrust IEEE
        IEEE_8021AD,      /// IEEE 802.1ad
        MPLS_DSCP,        /// DSCP bits, bound to MPLS family
        MPLS_DSCP_IPV6,   /// IPv6 DSCP bits, bound to MPLS
        IEEE_8021QCN,     /// IEEE 802.1QCN
        UNKNOWN           /// Unknown - always leave as last
    };

    enum class AftCosBehaviorAggregateType : u_int8_t {
        LAYER2,
        DSCP,
        DSCP_V6,
        IP_EXP,
        FIXED
    };

    enum class AftCosBindAction : u_int8_t {
        ADD,
        UPDATE,
        DELETE
    };
    enum class AftCosExcessPriority : uint8_t {
        Unspecified,
        Low,
        High,
        MediumLow,
        MediumHigh,
        None
    };

    enum class AftCosRewriteProto : u_int8_t {
        ANY,
        IEEE802_INNER,
        DSCP_MPLS,
        IP_PREC_MPLS,
        MPLS_INET_BOTH,
        MPLS_INET_BOTH_NON_VPN,
    };

    enum AftCosRewriteFlags  {
        IMPLICIT_BINDING,
        IP_OUTER_HDR,
        IP_BOTH_HDR,
        _SIZE                   // not a valid flag, must be last
    };

    #define AFT_COS_QUEUE_STATS_TYPE            1u
    #define AFT_COS_VOQ_STATS_TYPE              2u

    using AftCosRewriteFlagSet = std::bitset<AftCosRewriteFlags::_SIZE>;

    //
    // Template function for converting enum class variables
    //
    template <typename Enumeration>
    constexpr auto enumAsValue(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
    {
        static_assert(std::is_enum<Enumeration>::value, "parameter is not of type enum or enum class");
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    //
    // CoS Helper Function Prototypes
    //
    std::stringstream *buffer2Str(const u_int64_t bfrv,
                                  const AftCosBfrType type,
                                  std::stringstream *str,
                                  u_int8_t percision);
    std::string priorityString(AftCosPriority prio);
    bool priorityValid(AftCosPriority p);
    std::string rateTypeString(AftCosTxType type);
    std::string rateModifierString(AftCosTxModifier modifier);
    std::string bufferTypeString(AftCosBfrType type);
    std::string schedModeString(AftCosSchedMode type);
    std::string classifierTypeString(AftCosClsType Type);
    std::string AftLossPriorityString(AftCosLossPriority lp);
    bool AftLossPriorityIsValid(AftCosLossPriority lp);
    std::string AftBaTypeString(AftCosBehaviorAggregateType type);
    std::string AftCosBindActionString(AftCosBindAction action);
    bool excessPriorityValid (CosCommon::AftCosExcessPriority p);
    std::string excessPriorityString (AftCosExcessPriority prio);
    std::string AftCosRewriteProtoString(AftCosRewriteProto proto);
    std::string policingPriorityString (AftCosFcPolicingPriority priority);
    std::string spuPriorityString (AftCosFcSpuPriority priority);
    void AftCosBreakFunc();
} // namespace CosCommon

#endif // __AftCosCommon_h__
