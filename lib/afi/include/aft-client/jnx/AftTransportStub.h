//
/// @file   AftTransportStub.h
/// @brief  AftTransportStub class definitions
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

#ifndef __AftTransportStub_h__
#define __AftTransportStub_h__

#include "jnx/AftTransport.h"
#include "jnx/AftTransportSession.h"
#include <assert.h>

///
/// @cond
///

class AftTransportStub;
typedef std::shared_ptr<AftTransportStub> AftTransportStubPtr;     ///< Type of pointer for stub transport
typedef std::weak_ptr<AftTransportStub>   AftTransportStubWeakPtr; ///< Type of weak pointer for stub transport

class AftTransportStubReceiver;
typedef std::shared_ptr<AftTransportStubReceiver> AftTransportStubReceiverPtr; ///< Type of pointer for stub receiver

///
/// @class   AftTransportStubReceiver
/// @brief   Simple logging AftReceiver for stub testing
///
class AftTransportStubReceiver: public AftReceiver {
public:
    static AftTransportStubReceiverPtr create() {
        return std::make_shared<AftTransportStubReceiver>();
    }
        
    //
    // Constructor and destructor
    //
    AftTransportStubReceiver() {};
    ~AftTransportStubReceiver() {};
    
    ///
    /// @brief Set the transport used by the receiver
    /// @param [in] transport Reference to the transport to use
    ///
    virtual void  setTransport(const AftTransportPtr &newTransport) {} ;
    
    /// @returns Transport currently in use by the receiver
    virtual AftTransportPtr transport() { return nullptr; };
    
    /// @returns True if the transport has been set
    virtual bool  hasTransport() { return false; };
    
    ///
    /// @brief Set the transport session used by the receiver
    /// @param [in] session Reference to the transport to use
    ///
    virtual void  setSession(const AftTransportSessionPtr &newSession) {};
    
    /// @returns Transport session currently in use by the receiver
    virtual AftTransportSessionPtr session() { return nullptr; };
    
    /// @returns True if the transport session has been set
    virtual bool  hasSession() { return false; };

    virtual bool receive(const AftInsertPtr &insert) {
        std::cout << "AftTransport::receive\n";
       
        //
        // Loop through the nodes
        //
        for (AftNodePtr node: insert->nodes()) {
            std::cout << " insert node=" << *node << "\n";
        }
        
        //
        // Loop through the entries
        //
        for (AftEntryPtr entry: insert->entries()) {
            std::cout << " insert entry=" << *entry << "\n";
        }
        
        //
        // Loop through the names
        //
        for (AftNameTable::Pair name: insert->names()) {
            std::cout << " insert name=" << name << "\n";
        }
        
        //
        // Loop through the groups
        //
        for (AftGroupTable::Pair group: insert->groups()) {
            std::cout << " insert group=" << group << "\n";
        }

        return true;
    };
    
    virtual bool receive(const AftRemovePtr &remove) {
        std::cout << "AftTransport::receive\n";
        
        //
        // Loop through the nodes
        //
        for (AftNodeToken nodeToken: remove->nodes()) {
            std::cout << " remove nodetoken=" << nodeToken << "\n";
        }
        
        //
        // Loop through the entries
        //
        for (AftEntryPtr entry: remove->entries()) {
            std::cout << " remove entry=" << *entry << "\n";
        }
        
        //
        // Loop through the names
        //
        for (AftNameTable::Pair name: remove->names()) {
            std::cout << " remove name=" << name << "\n";
        }
        
        //
        // Loop through the groups
        //
        for (AftGroupTable::Pair group: remove->groups()) {
            std::cout << " remove group=" << group << "\n";
        }
        
        return true;
    };
    
    virtual bool receive(const AftNodeInfoPtr &info) {
        std::cout << "AftTransport::receive\n";

        for (const AftNodeInfoRequest &infoRequest: info->nodes()) {
            std::cout << " info nodeToken=" << infoRequest.nodeToken() << "\n";
        }
            
        return true;
    };
    
    ///
    /// @brief Main receiver handler for node test requests
    /// @param [in] test Test request block
    ///
    bool receive(const AftNodeTestPtr &test) {
        //
        // We don't handle this
        //
        return false;
    }
    
    ///
    /// @brief Main receiver handler for entry test requests
    /// @param [in] test Test request block
    ///
    bool receive(const AftEntryTestPtr &test) {
        //
        // We don't handle this
        //
        return false;
    }
};


///
/// @class   AftTransportStub
/// @brief   Simple AftTransport client class used for testing
///
class AftTransportStub: public AftTransport {
protected:
    AftTransportStubWeakPtr     _selfPtr;  ///< Weak reference used to let client refer to itself
    AftSandboxPtr               _sandbox;  ///< Sandbox used to build into by the stub transport
    AftTransportSessionPtr      _session;  ///< Stub session for testing
     
public:
    //
    // Constructor and destructor
    //
    AftTransportStub(): AftTransport() {};
    ~AftTransportStub() {};
    
    bool alloc (const std::string &engineName,
                const std::string &name,
                const uint32_t     inputPorts,
                const uint32_t     outputPorts) {
        AftIndex           portIndex;
        std::stringstream  ss;
        AftInsertPtr portInsert;
        
        //
        // Create a corresponding Aft sandbox to serve as it's interface
        //
        _sandbox = AftSandbox::create(name);
        assert(_sandbox != nullptr);
        
        //
        // Create our insertion context
        //
        AftInsertPtr discardInsert = AftInsert::create(_sandbox);
        
        //
        // Create basic discard
        //
        AftNodePtr discard = AftDiscard::create();
        discardInsert->push(discard, AFT_NODE_TOKEN_DISCARD);
        _sandbox->send(discardInsert);
        
        //
        // Create a insert
        //
        portInsert = AftInsert::create(_sandbox);
        
        //
        // Add some input and output ports. We hardcode these in the stub transport for the moment
        //
        for (portIndex = 0; portIndex < inputPorts; portIndex++) {
            char portName[80];
            
            sprintf(portName, "port-%llu", static_cast<unsigned long long>(portIndex));
            
            //
            // Create an input port that's not connected to anything yet
            //
            AftNodePtr inputPort = AftInputPort::create("input",
                                                        portIndex,
                                                        portName,
                                                        AFT_NODE_TOKEN_DISCARD);
            portInsert->push(inputPort);
        }
        
        for (portIndex = 0; portIndex < outputPorts; portIndex++) {
            char portName[80];
            
            sprintf(portName, "port-%llu", static_cast<unsigned long long>(portIndex));
      
            //
            // Create an output port
            //
            AftNodePtr outputPort = AftOutputPort::create("output",
                                                          portIndex,
                                                          portName,
                                                          AFT_NODE_TOKEN_DISCARD);
            portInsert->push(outputPort);
        }
        
        //
        // Insert everything
        //
        _sandbox->send(portInsert);
        
        //
        // Dump out some debug
        //
        if (_tracing) {
            std::cout << "AftTransport::alloc(name=\"" << name << "\")\n";
            std::cout << "Ports:\n" << _sandbox;
        }
        
        //
        // Let the caller know the sandbox is alloc'd
        //
        return true;
    }

    bool release(const std::string &name) {
        _sandbox = nullptr;
        return true;
    }

    ///
    /// @brief Factory convenience creator for shared_ptr to transport stub client 
    ///
    static AftTransportStubPtr create() {
        AftTransportStubPtr transport = std::make_shared<AftTransportStub>();
        
        //
        // Stash a weak pointer to ourselves
        //
        transport->_selfPtr = transport;
        
        if (transport->_tracing) {
            std::cout << "AftTransport::create()\n";
        }

        //
        // Return the new transport
        //
        return transport;
    }
    
    //
    // Concrete versions of inherited virtual functions for the client
    //
    bool open(const std::string &newName) {
        return false;
    }

    bool open(const std::string &newName, AftSandboxPtr &sandbox) {
        AftTransportPtr transport;

        //
        // Set the sandbox from one we made earlier
        //
        sandbox = _sandbox;
        
        //
        // Allocate transport session
        //
        _session = AftTransportSession::create(newName);
        _session->setReceiver(AftTransportStubReceiver::create());
        
        //
        // Link the receiver to the underlying transport
        //
        transport = _selfPtr.lock();
        sandbox->setTransport(transport);
        
        return true;
    }

    void close() {
        if (_tracing) {
            std::cout << "AftTransport::close()\n";
        }
        
        //
        // Free the session (which will free the stub receiver)
        //
        _session = nullptr;
    };

    bool send (const AftOperationPtr &op) {
        //
        // Give the operation to the server session
        //
        if (_session != nullptr) {
            return _session->receive(op);
        } else {
            return false;
        }
    };

    bool send(const AftOperationPtr &op, AftOperationResponse response) {
        //
        // Give the operation to the server session
        //
        if (_session != nullptr) {
            return _session->receive(op);
        } else {
            return false;
        }
    };
};

///
/// @endcond
///

#endif
