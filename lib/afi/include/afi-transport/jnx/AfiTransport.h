//
//  @file   AfiTransport.h
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

#ifndef __AfiTransport__
#define __AfiTransport__

#include "jnx/AftTransport.h"
#include <grpc++/grpc++.h>

class AfiClient;
class AfiTransport;
typedef std::shared_ptr<AfiTransport> AfiTransportPtr;
typedef std::weak_ptr<AfiTransport>   AfiTransportWeakPtr;

class AfiTransport: public AftTransport {
protected:
    AfiTransportWeakPtr             _selfPtr; ///< Weak pointer to ourselves for reference
    AftSandboxPtr                   _sandbox; ///< Pointer to local sandbox
    
    std::shared_ptr<grpc::Channel>  _channel; ///< gRPC channel to server
    std::shared_ptr<AfiClient>      _client;  ///< Afi client
    
public:
    //
    // Constructor and destructor
    //
    AfiTransport(const bool newAsync);
    ~AfiTransport();

    static AfiTransportPtr create();
    static AfiTransportPtr create(const bool newAsync);
    static AfiTransportPtr downcast(const AftTransportPtr &transport) {
        return std::dynamic_pointer_cast<AfiTransport>(transport);
    }
   
    //
    // Accessors
    //
    void setSandbox(const AftSandboxPtr &newSandbox) { _sandbox = newSandbox; };
    AftSandboxPtr sandbox() const { return _sandbox; };
    
    //
    // Concrete versions of inherited virtual functions
    //
    bool       open(const std::string &name, AftSandboxPtr &newSandbox);
    void       close();
    bool       alloc(const std::string &engineName,
                     const std::string &name,
                     const uint32_t     inputPorts,
                     const uint32_t     outputPorts);
    bool       release(const std::string &name);

    bool       send(const AftOperationPtr &newOperation);
 
    //
    // Test Interface
    //
    bool        testNodeIsPresent(AftNodeToken nodeToken);
    bool        testNodeHasNext(AftNodeToken nodeToken, AftNodeToken nextToken);
    uint32_t    testNodeNextCount(AftNodeToken nodeToken);
    bool        testEntryIsPresent(const AftEntryPtr &entry);
    bool        testEntryHasNext(const AftEntryPtr &entry, AftNodeToken nextToken);
};

#endif
