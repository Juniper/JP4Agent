//
//  @file   AfiSandbox.h
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

#ifndef __AfiSandbox__
#define __AfiSandbox__

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "Afi.grpc.pb.h"
#include "Afi.pb.h"
#include <grpc++/impl/codegen/async_stream.h>

#include "jnx/Aft.h"

class AfiSandboxRpc {
public:
    void encodeAlloc(AfiSandboxRequest       &request,
                     const std::string       &engineName,
                     const std::string       &name,
                     const AftIndex           maxInputPorts,
                     const AftIndex           maxOutputPorts);
    void decodeAlloc(const AfiSandboxRequest  &request,
                     std::string              &engineName,
                     std::string              &name,
                     AftIndex                 &maxInputPorts,
                     AftIndex                 &maxOutputPorts);
    
    void encodeAlloc(AfiSandboxReply &reply, bool status);
    bool decodeAlloc(const AfiSandboxReply &reply);
    
    void encodeOpen(AfiSandboxRequest &request, const std::string &name);
    void decodeOpen(const AfiSandboxRequest &request, std::string &name);

    void encodeOpen(AfiSandboxReply &reply, bool status, const AftSandboxCorePtr &sandbox);
    bool decodeOpen(const AfiSandboxReply &reply, const AftSandboxPtr &sandbox);
    
    void encodeRelease(AfiSandboxRequest &request, const std::string &name);
    void decodeRelease(const AfiSandboxRequest &request, std::string &name);
    
    void encodeRelease(AfiSandboxReply &reply, bool status);
    bool decodeRelease(const AfiSandboxReply &reply);

    void encodeClose(AfiSandboxRequest &request, const std::string &name);
    void decodeClose(const AfiSandboxRequest &request, std::string &name);
    
    void encodeClose(AfiSandboxReply &reply, bool status);
    bool decodeClose(const AfiSandboxReply &reply);

    void encode(AfiSandboxGroup &request, const AftGroupTable::Pair &group);
    AftGroupTable::Pair decode(const AfiSandboxGroup &request);

    void encode(AfiSandboxName &request, const AftNameTable::Pair &name);
    AftNameTable::Pair decode(const AfiSandboxName &request);
};

#endif
