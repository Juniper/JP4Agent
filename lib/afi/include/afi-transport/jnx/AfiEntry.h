//
//  @file   AfiEntry.h
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

#ifndef __AfiEntry__
#define __AfiEntry__

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "Afi.grpc.pb.h"
#include "AfiEntry.pb.h"
#include <grpc++/impl/codegen/async_stream.h>

#include "jnx/Aft.h"

class AfiEntryRpc {
public:
    //
    // Singleton instnace
    //
    static AfiEntryRpc& getEntryRpc()
    {
        static AfiEntryRpc entryRpc;
        return entryRpc;
    }

    //
    // Delete the copy and assignment member functions
    //
    AfiEntryRpc(AfiEntryRpc const&)     = delete;
    void operator=(AfiEntryRpc const&)  = delete;

    typedef std::function<void(AfiEntryRequest *eRequest, const AftEntryPtr &entry)> AfiEntryEncode;
    typedef std::map<std::string, AfiEntryEncode>  AfiEntryEncodeMap;
    typedef std::function<void(const AfiEntryRequest &eRequest, AftEntryPtr &entry)> AfiEntryDecode;
    typedef std::map<uint32_t, AfiEntryDecode>  AfiEntryDecodeMap;

    static AfiEntryRpc &entryRpc() {
        static AfiEntryRpc _instance;
        return _instance;
    }

    void encode(AfiEntryRequest *eRequest, const AftEntryPtr entry, bool deleteEntryEncode = false);
    void decode(const AfiEntryRequest &eRequest, AftEntryPtr &entry);

protected:
    AfiEntryEncodeMap  _encoders;
    AfiEntryEncodeMap  _deleteEncoders;
    AfiEntryDecodeMap  _decoders;

private:
    AfiEntryRpc();
};

#endif
