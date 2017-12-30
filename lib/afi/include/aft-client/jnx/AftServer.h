//
/// @file   AftServer.h
/// @brief
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

#ifndef __AftServer__
#define __AftServer__

#include "jnx/AftTypes.h"
#include "jnx/AftNode.h"
#include "jnx/AftEntry.h"
#include "jnx/AftSandbox.h"

///
/// @addtogroup AftServer Server
/// @brief Classes used to define the underlying server classes and architecture
/// @{
///

///
/// @class   AftServer
/// @brief   Base virtual class used for all servers
///

class AftServer {
public:
    ///
    /// @brief Allocate a new sandbox on the nominated engine
    ///
    /// @param [in] engineName  Name of engine to use to allocate sandbox
    /// @param [in] name        Name of sandbox to allocate
    /// @param [in] inputPorts  Maxiumum number of input ports the sandbox can have
    /// @param [in] outputPorts Maxiumum number of output ports the sandbox can have
    /// @returns True if the allocation was successful
    ///
    virtual bool alloc(const std::string &engineName,
                       const std::string &name,
                       const uint16_t     inputPorts,
                       const uint16_t     outputPorts) = 0;

    ///
    /// @brief Open an existing sandbox on the server
    ///
    /// @param [in]  name          Name of sandbox to open
    /// @param [in]  session       Session that the sandbox open request arrived on
    /// @param [out] newSandbox    Reference to sandbox that was opened if the open() is successful
    /// @returns True if the open was successful
    ///
    virtual bool open(const std::string &name, const AftTransportSessionPtr &session, AftSandboxCorePtr &newSandbox) = 0;

    ///
    /// @brief Release an existing sandbox and all associated resources on the server
    ///
    /// @param [in] name        Name of sandbox to release
    /// @returns True if the release was successful
    ///
    virtual bool release(const std::string &name) = 0;

    ///
    /// @brief Close an open sandbox and unbind the transport
    ///
    /// @param [in] name        Name of sandbox to close
    ///
    virtual void close(const std::string &name) = 0;
    
    ///
    /// @brief Find session for named sandbox
    ///
    /// @param [in]  name        Name of sandbox
    /// @param [out] session   Reference to session for sandbox
    /// @returns True if transport found
    virtual bool find(const std::string &name, AftTransportSessionPtr &session) = 0;
};

typedef std::shared_ptr<AftServer> AftServerPtr; ///< Pointer type for all AftServer objects

#endif
