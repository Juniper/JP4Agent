//
/// @file   AftTransport.h
/// @brief  AftTransport class definitions
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

#ifndef __AftTransport_h__
#define __AftTransport_h__

#include "jnx/AftTypes.h"
#include "jnx/AftNode.h"
#include "jnx/AftEntry.h"
#include "jnx/AftOperation.h"

///
/// @addtogroup  AftTransport Transport
/// @brief Classes used to define and manage Aft client/server communications
///
/// @{
///

///
/// @class   AftTransport
/// @brief   Largely virtual base class used for all Aft transport implementations
///
/// AftTransport is used as the primary connection between an Aft client and server. The transport is
/// responsible for handling all the underlying communications between the client and the server (in order to
/// simplify client design).
///
/// The transport has two primary interfaces. A simple interface to allow a sandbox to be created (if permitted),
/// opened, closed and released (again, if permitted). A transport can only have one open sandbox at a time. Once
/// a sandbox has been successfully opened, the client can use the transport to send it operations, which
/// modify the sandbox.
///
/// If a client has a need to drive two (or more) sandboxes at a time, it must create a transport for each sandbox
/// instance.
///
/// The underlying connection to the server can be as simple as a direct function call or a thread work queue.
/// It can also be as complicated as a serializer on top of an RPC mechanism. The goal is to isolate the
/// client from the transport mechanism so that different server orientations can be used without rewriting
/// the client code.
///
/// The transport handles a flow of AftOperation state. Primarily, this state consists of a flow of AftInsert
/// and AftRemove objects (which, in turn, contain lists of nodes and entries to modify). However, test
/// operations can also be inserted into the flow, along with other requests (such as telemetry triggers).
///
/// The underlying transport preserves the temporal ordering of operations. That is to say that a sequence
/// of operations A, B and C will arrive on the server as A, B and C. This is very important for consistency
/// as the node and entry dependencies succeed or fail on this preserved ordering.
///
class AftTransport {
protected:
    AftReceiverPtr        _receiver; ///< Pointer to receiver used to handle events
    bool                  _async;    ///< True if the transport support async client handling
    bool                  _tracing;  ///< True if transport debug tracing is enabled

    AftTransport(const bool newAsync = false, const bool newTracing = false): _async(newAsync), _tracing(newTracing)  {};
    
public:
    ///
    /// @brief               Open a connection(s)
    /// @param [in] name     Name of the sandbox the transport controls
    /// @param [in] sandbox     Reference to the receiver instance used to handle incoming events
    /// @retval true         Open was successful
    /// @retval false        Open failed
    ///
    virtual bool       open(const std::string &name, AftSandboxPtr &sandbox) = 0;

    ///
    /// @brief Close a connection to the server(s)
    ///
    virtual void       close() = 0;
    
    ///
    /// @brief Release the memory and all the nodes associated with a sandbox
    /// @param [in] engineName  Name of the engine that we create the sandbox with
    /// @param [in] name        Name of the sandbox the transport should create
    /// @param [in] inputPorts  Number of input ports to allocate for sandbox
    /// @param [in] outputPorts Number of output ports to allocate for sandbox
    /// @retval true            Alloc was successful
    /// @retval false           Alloc failed
    ///
    virtual bool       alloc(const std::string &engineName,
                             const std::string &name,
                             const uint32_t     inputPorts,
                             const uint32_t     outputPorts) = 0;

    ///
    /// @brief Release the memory and all the nodes associated with a sandbox
    /// @param [in] name     Name of the sandbox the transport controls
    ///
    virtual bool       release(const std::string &name) = 0;
    
    ///
    /// @brief                    Send an AftOperation to the server
    /// @param [in] newOperation  Reference to the AftOperation to send
    ///
    virtual bool       send(const AftOperationPtr &newOperation) = 0;
    
    ///
    /// @brief                    Receive an AftOperation from the server
    /// @param [in] newOperation  Reference to the AftOperation to receive
    ///
    virtual bool       receive(const AftOperationPtr &newOperation) { return false; };

    //
    // Accessors
    //
    
    ///
    /// @brief                   Set the event receiver for the transport
    /// @param [in] newReceiver  Reference to the receiver to use for events
    ///
    void setReceiver(const AftReceiverPtr &newReceiver) { _receiver = newReceiver; };
    
    /// @returns Returns pointer to the event receiver
    AftReceiverPtr receiver() const { return _receiver; };
    
    ///
    /// @brief                      Enable or disable transport tracing
    /// @param [in] tracingEnabled  True if transport tracing should be enabled
    ///
    void setTracing(const bool tracingEnabled) { _tracing = tracingEnabled; };
    
    /// @returns True if transport tracing is enabled
    bool tracing() const { return _tracing; };

    ///
    /// @brief                      Enable or disable whether transport supports async operations
    /// @param [in] asyncEnabled  True if transport supports async operations
    ///
    void setAsync(const bool asyncEnabled) { _async = asyncEnabled; };
    
    /// @returns True if transport supports async operations
    bool async() const { return _async; };
};

///
/// @}
///

#endif
