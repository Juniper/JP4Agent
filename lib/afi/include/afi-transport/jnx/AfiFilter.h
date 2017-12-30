//
//  @file   AfiFilter.h
//  @brief  Filter GRPC Encoders/decoders
//
//  Copyright (c) Juniper Networks, Inc., [2017].
//
//  All rights reserved.
//  Created by Ramakant Kasat
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AfiFilter__
#define __AfiFilter__

#include "AfiEntry.h"
#include "jnx/AftFilter.h"

class AfiTermMatchRpc {
private:
    using AfiTermMatchEncode = std::function<void(AfiTermMatch* request,
                               const AftTermMatchUPtr& aftMatch)>;
    using AfiTermMatchEncodeMap = std::map<AftTermMatch::MatchClass, AfiTermMatchEncode>;

    using AfiTermMatchDecode = std::function<void(const AfiTermMatch& request,
                               AftTermMatchUPtr& aftMatch)>;
    using AfiTermMatchDecodeMap = std::map<AftTermMatch::MatchClass, AfiTermMatchDecode>;

protected:
    AfiTermMatchEncodeMap _encoders;
    AfiTermMatchDecodeMap _decoders;

    static void encodeRangeMatch(AfiTermMatch* match,
                                 const AftTermMatchUPtr& aftMatch);
    static void decodeRangeMatch(const AfiTermMatch& match,
                                 AftTermMatchUPtr& aftMatch);
    static void encodePrefixMatch(AfiTermMatch* match,
                                  const AftTermMatchUPtr& aftMatch);
    static void decodePrefixMatch(const AfiTermMatch& match,
                                  AftTermMatchUPtr& aftMatch);
    static void encodeBitOpMatch(AfiTermMatch* match,
                                 const AftTermMatchUPtr& aftMatch);
    static void decodeBitOpMatch(const AfiTermMatch& match,
                                 AftTermMatchUPtr& aftMatch);
    static void encodeFlexMatch(AfiTermMatch* match,
                                const AftTermMatchUPtr& aftMatch);
    static void decodeFlexMatch(const AfiTermMatch& match,
                                AftTermMatchUPtr& aftMatch);
    static void encodeDfwRanges(AfiDfwRangeMatch* rangeData,
                                const AftDfwRangeMatch* aftRangeMatch);
    static void decodeDfwRanges(const AfiDfwRangeMatch& rangeData,
                                AftDfwRangeVec& ranges);
    static void encodeDfwPrefixes(AfiDfwPrefixMatch* pfxData,
                                  const AftDfwPrefixMatch* aftPfxMatch);
    static void decodeDfwPrefixes (const AfiDfwPrefixMatch& pfxData,
                                   AftDfwPrefixVec& prefixes);

public:
    AfiTermMatchRpc();
    ~AfiTermMatchRpc() {};

    void encode(AfiTermMatch* request, const AftTermMatchUPtr& data);
    void decode(const AfiTermMatch& request, AftTermMatchUPtr& data);
};



//
// As of now we dont have much in AfiTermActionRpc but this would evolve
// and we would have something more here
//
class AfiTermActionRpc {
public:
    AfiTermActionRpc();
    ~AfiTermActionRpc() {};

    void encode(AfiTermAction* action,
                const AftTermActionUPtr& aftAction);
    void decode(const AfiTermAction& action,
                AftTermActionUPtr& aftAction);
};


extern AfiTermMatchRpc afiTermMatchRpc;
extern AfiTermActionRpc afiTermActionRpc;

#endif
