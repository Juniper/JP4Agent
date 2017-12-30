//
//  @file   AfiNode.h
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

#ifndef __AfiNode__
#define __AfiNode__

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "Afi.grpc.pb.h"
#include "AfiNode.pb.h"
#include <grpc++/impl/codegen/async_stream.h>

#include "jnx/Aft.h"

class AfiNodeRpc {
public:
    typedef std::function<void(AfiNodeRequest *nRequest, const AftNodePtr &node)> AfiNodeEncode;
    typedef std::map<std::string, AfiNodeEncode>  AfiNodeEncodeMap;
    typedef std::function<void(const AfiNodeRequest &nRequest, AftNodePtr &node)> AfiNodeDecode;
    typedef std::map<uint32_t, AfiNodeDecode>  AfiNodeDecodeMap;

    static AfiNodeRpc &nodeRpc() {
        static AfiNodeRpc _instance;
        return _instance;
    }

    void encode(AfiNodeRequest *nRequest, const AftNodePtr node);
    void decode(const AfiNodeRequest &nRequest, AftNodePtr &node);

    void insert(const AfiNodeEncodeMap &insertMap) {
        _encoders.insert(insertMap.begin(), insertMap.end());
    }
    void insert(const AfiNodeDecodeMap &decodeMap) {
        _decoders.insert(decodeMap.begin(), decodeMap.end());
    }

protected:
    AfiNodeEncodeMap  _encoders;
    AfiNodeDecodeMap  _decoders;

private:
    AfiNodeRpc();
    AfiNodeRpc(const AfiNodeRpc &);
    AfiNodeRpc(AfiNodeRpc &&);
    AfiNodeRpc& operator=(const AfiNodeRpc &);
    AfiNodeRpc& operator=(AfiNodeRpc &&);
    ~AfiNodeRpc() {};
};

#endif
