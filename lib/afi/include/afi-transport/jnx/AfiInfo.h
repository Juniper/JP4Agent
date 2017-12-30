//
//  @file   AfiInfo.h
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

#ifndef __AfiInfo__
#define __AfiInfo__

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "Afi.grpc.pb.h"
#include "AfiInfo.pb.h"
#include <grpc++/impl/codegen/async_stream.h>

#include "jnx/Aft.h"

class AfiInfoRpc {
public:
    typedef std::function<void(AfiNodeInfoReply *iReply, const AftInfoUPtr &info)> AfiInfoEncodeReply;
    typedef std::map<std::string, AfiInfoEncodeReply>  AfiInfoEncodeReplyMap;
    typedef std::function<void(const AfiNodeInfoReply &iReply, AftInfoUPtr &info)> AfiInfoDecodeReply;
    typedef std::map<uint32_t, AfiInfoDecodeReply>  AfiInfoDecodeReplyMap;
    
    static AfiInfoRpc &infoRpc() {
        static AfiInfoRpc _instance;
        return _instance;
    }
    
    void encodeRequest(AfiNodeInfoRequest *eRequest, const AftNodeInfoRequest &infoRequest);
    AftNodeInfoRequest decodeRequest(const AfiNodeInfoRequest &eRequest);

    void encodeReply(AfiNodeInfoReply *eReply, const AftInfoUPtr &info);
    void decodeReply(const AfiNodeInfoReply &eReply, AftInfoUPtr &info);

    void encodeRequest(AfiSandboxInfoRequest *eRequest, const AftSandboxInfoPtr &infoRequest);
    AftSandboxInfoPtr decodeRequest(const AfiSandboxInfoRequest &eRequest);
    
    void encodeReply(AfiSandboxInfoReply *eReply, const AftParameters::UPtr &reply);
    void decodeReply(const AfiSandboxInfoReply &eReply, AftParameters::UPtr &reply);

protected:
    AfiInfoEncodeReplyMap  _encoders;
    AfiInfoDecodeReplyMap  _decoders;
    
private:
    AfiInfoRpc();
    AfiInfoRpc(const AfiInfoRpc &);
    AfiInfoRpc(AfiInfoRpc &&);
    AfiInfoRpc& operator=(const AfiInfoRpc &);
    AfiInfoRpc& operator=(AfiInfoRpc &&);
    ~AfiInfoRpc() {};
};

#endif
