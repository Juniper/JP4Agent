//
//  @file   AfiActive.h
//  @brief
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

#ifndef __AfiActive__
#define __AfiActive__

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "Afi.grpc.pb.h"
#include "AfiActive.pb.h"
#include <grpc++/impl/codegen/async_stream.h>

#include "jnx/Aft.h"

class AfiActiveRpc {
public:
    typedef std::function<void(AfiActiveReply *iReply, const AftActiveUPtr &Active)> AfiActiveEncodeReply;
    typedef std::map<std::string, AfiActiveEncodeReply>  AfiActiveEncodeReplyMap;
    typedef std::function<void(const AfiActiveReply &iReply, AftActiveUPtr &Active)> AfiActiveDecodeReply;
    typedef std::map<uint32_t, AfiActiveDecodeReply>  AfiActiveDecodeReplyMap;
    
    static AfiActiveRpc &activeRpc() {
        static AfiActiveRpc _instance;
        return _instance;
    }
    
    void encodeRequest(AfiActiveRequest *aRequest, const AftNodeActiveRequest &activeRequest);
    AftNodeActiveRequest decodeRequest(const AfiActiveRequest &aRequest);

    void encodeReply(AfiActiveReply *aReply, const AftActiveUPtr &active);
    void decodeReply(const AfiActiveReply &aReply, AftActiveUPtr &active);

protected:
    AfiActiveEncodeReplyMap  _encoders;
    AfiActiveDecodeReplyMap  _decoders;
    
private:
    AfiActiveRpc();
    AfiActiveRpc(const AfiActiveRpc &);
    AfiActiveRpc(AfiActiveRpc &&);
    AfiActiveRpc& operator=(const AfiActiveRpc &);
    AfiActiveRpc& operator=(AfiActiveRpc &&);
    ~AfiActiveRpc() {};
};

#endif
