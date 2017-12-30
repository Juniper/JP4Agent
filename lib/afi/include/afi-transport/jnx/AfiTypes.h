//
//  @file   AfiTypes.h
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

#ifndef __AfiTypes__
#define __AfiTypes__

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "Afi.grpc.pb.h"
#include "Afi.pb.h"
#include <grpc++/impl/codegen/async_stream.h>

#include "jnx/Aft.h"

class AfiDataRpc {
    typedef std::function<void(AfiData &request, const AftDataPtr &data)> AfiDataEncode;
    typedef std::map<std::string, AfiDataEncode>  AfiDataEncodeMap;
    
protected:
    AfiDataEncodeMap  _encoders;

    static void encodeBool(AfiData &request, const AftDataPtr data);
    void decodeBool(const AfiData &request, AftDataPtr &data);

    static void encodeInt(AfiData &request, const AftDataPtr data);
    void decodeInt(const AfiData &request, AftDataPtr &data);

    static void encodeString(AfiData &request, const AftDataPtr data);
    void decodeString(const AfiData &request, AftDataPtr &data);

    static void encodePrefix(AfiData &request, const AftDataPtr data);
    void decodePrefix(const AfiData &request, AftDataPtr &data);

    static void encodeIP4Addr(AfiData &request, const AftDataPtr data);
    void decodeIP4Addr(const AfiData &request, AftDataPtr &data);

    static void encodeIP6Addr(AfiData &request, const AftDataPtr data);
    void decodeIP6Addr(const AfiData &request, AftDataPtr &data);

    static void encodeEtherAddr(AfiData &request, const AftDataPtr data);
    void decodeEtherAddr(const AfiData &request, AftDataPtr &data);

    static void encodeLabel(AfiData &request, const AftDataPtr data);
    void decodeLabel(const AfiData &request, AftDataPtr &data);

    static void encodeLabelOnly(AfiData &request, const AftDataPtr data);
    void decodeLabelOnly(const AfiData &request, AftDataPtr &data);
public:
    AfiDataRpc();
    ~AfiDataRpc() {};
    
    void encode(AfiData &request, const AftDataPtr data);
    void decode(const AfiData &request, AftDataPtr &data);
};

class AfiKeyRpc {
public:
    AfiKeyRpc() {};
    ~AfiKeyRpc() {};
    
    void encode(AfiKey &request, const AftKey &key);
    void decode(const AfiKey &request, AftKey &key);
};

class AfiParamRpc {
public:
    AfiParamRpc() {};
    ~AfiParamRpc() {};
    
    void encode(AfiParam &request, const std::string &name, const AftDataPtr &value);
    void decode(const AfiParam &request, std::string &name, AftDataPtr &value);
};

class AfiProtoRpc {
public:
    AfiProtoRpc() {};
    ~AfiProtoRpc() {};
    
    void encode(AfiProto &request, const AftProto &proto);
    void decode(const AfiProto &request, AftProto &proto);
};

extern AfiDataRpc   afiDataRpc;
extern AfiKeyRpc    afiKeyRpc;
extern AfiParamRpc  afiParamRpc;
extern AfiProtoRpc  afiProtoRpc;

#endif
