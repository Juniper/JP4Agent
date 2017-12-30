//
/// @file   AftSandbox.h
/// @brief  AftSandbox class definition
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

#ifndef __AftSandbox__
#define __AftSandbox__

#include "jnx/AftTypes.h"
#include "jnx/AftNode.h"
#include "jnx/AftEntry.h"
#include "jnx/AftTokenTable.h"
#include "jnx/AftNameTable.h"
#include "jnx/AftTypeTable.h"
#include "jnx/AftPortTable.h"
#include "jnx/AftFieldTable.h"
#include "jnx/AftProtoTable.h"
#include "jnx/AftGroupTable.h"
#include "jnx/AftEncapTable.h"
#include "jnx/AftDecapTable.h"
#include "jnx/AftLogV1.h"

#include <assert.h>

///
/// @addtogroup AftSandbox Sandbox
/// @brief Classes for accessing and managing Aft sandboxes
/// @{
///

#define AFT_OPERATION_TIMEOUT 2000 ///< The default asynchronous timeout is 2000ms

///
/// @class   AftReceiver
/// @brief   Virtual base class used as a receiver destination for all sandboxes and transports
///
class AftReceiver {
public:
    ///
    /// @brief Set the transport used by the receiver
    /// @param [in] transport Reference to the transport to use
    ///
    virtual void  setTransport(const AftTransportPtr &transport) = 0;

    /// @returns Transport currently in use by the receiver
    virtual AftTransportPtr transport() = 0;

    /// @returns True if the transport has been set
    virtual bool  hasTransport() = 0;
    
    ///
    /// @brief Set the transport session used by the receiver
    /// @param [in] session Reference to the session to use
    ///
    virtual void  setSession(const AftTransportSessionPtr &session) = 0;
    
    /// @returns Transport session currently in use by the receiver
    virtual AftTransportSessionPtr session() = 0;
    
    /// @returns True if the transport session has been set
    virtual bool  hasSession() = 0;

    ///
    /// @brief Main receiver handler for incoming inserts
    /// @param [in] insert Reference to insert object to receive
    ///
    virtual bool receive(const AftInsertPtr &insert) = 0;

    ///
    /// @brief Main receiver handler for incoming removes
    /// @param [in] remove Reference to remove object to receive
    ///
    virtual bool receive(const AftRemovePtr &remove) = 0;

    ///
    /// @brief Main receiver handler for sandbox info requests
    /// @param [in] info Info request block
    ///
    virtual bool receive(const AftSandboxInfoPtr &info) { return false; };

    ///
    /// @brief Main receiver handler for sandbox find requests
    /// @param [in] find Find request block
    ///
    virtual bool receive(const AftSandboxFindPtr &find) { return false; };

    ///
    /// @brief Main receiver handler for node info requests
    /// @param [in] info Info request block
    ///
    virtual bool receive(const AftNodeInfoPtr &info) { return false; };

    ///
    /// @brief Main receiver handler for node active requests
    /// @param [in] active Active request block
    ///
    virtual bool receive(const AftNodeActivePtr &active) { return false; };

    ///
    /// @brief Main receiver handler for node test requests
    /// @param [in] test Test request block
    ///
    virtual bool receive(const AftNodeTestPtr &test) { return false; };

    ///
    /// @brief Main receiver handler for entry test requests
    /// @param [in] test Test request block
    ///
    virtual bool receive(const AftEntryTestPtr &test) {return false; };
};

///
/// @class   AftSandboxCore
/// @brief   Parent class used for both Aft and Em sandboxes
///
class AftSandboxCore {
protected:
    friend AfiSandboxRpc; ///< AfiSandboxRpc accesses the protected creation methods

    std::string            _name;        ///< Name of the sandbox
    AftNameTable::Ptr      _nameTable;   ///< Name lookup table for the sandbox
    AftTypeTable::Ptr      _typeTable;   ///< Type lookup table for the sandbox
    AftFieldTable::Ptr     _fieldTable;  ///< Field name table for the sandbox
    AftProtoTable::Ptr     _protoTable;  ///< Proto name table for the sandbox
    AftGroupTable::Ptr     _groupTable;  ///< Group name table for the sandbox
    AftEncapTable::Ptr     _encapTable;  ///< Table of available encapsulations
    AftDecapTable::Ptr     _decapTable;  ///< Table of available de-encapsulations

    AftPortTablePtr        _inputPorts;  ///< Input port table for the sandbox
    AftPortTablePtr        _outputPorts; ///< Output port table for the sandbox

public:
    AftSandboxCore(const std::string &newName);
    ~AftSandboxCore(void);
    AftSandboxCore(const AftSandboxCore &sandbox);
    
    //
    // Simple accessor
    //
    const std::string &name() const { return _name; };

    /// @returns Reference to input port table
    const AftPortTablePtr  &inputPortTable() const { return _inputPorts; };
    
    /// @returns Reference to output port table
    const AftPortTablePtr  &outputPortTable() const { return _outputPorts; };

    //
    // Sandbox types, field and tokens
    //
    AftNameTable::Pair createName (const std::string &newName, const AftNodeToken newNodeToken) {
        _nameTable->insert(newName, newNodeToken);
        return AftNameTable::Pair(newName, newNodeToken);
    };
    
    AftGroupTable::Pair createGroup(const std::string &newName) {
        AftGroup group = _groupTable->insert(newName);
        return AftGroupTable::Pair(newName, group.index());
    };
    
    //
    // Lookups
    //
    bool findName(const std::string &nodeName, AftNodeToken &nodeToken) { return _nameTable->find(nodeName, nodeToken); };
    bool findProto(const std::string &protoName, AftProtoIndex &protoIndex) { return _protoTable->find(protoName, protoIndex); };
    bool findType(const std::string &typeName, AftTypeIndex &typeIndex) { return _typeTable->find(typeName, typeIndex); };
    bool findGroup(const std::string &groupName, AftGroupIndex &groupIndex) { return _groupTable->find(groupName, groupIndex); };
    
    bool isValidProto(const std::string &protoName) const { return _protoTable->isValid(protoName); };
    bool isValidGroup(const std::string &groupName) const { return _groupTable->isValid(groupName); };
    bool isValidType(const std::string &typeName) const { return _typeTable->isValid(typeName); };
    bool isValidField(const AftField &field) const { return _fieldTable->isValid(field.name()); };

    //
    // Insertions
    //
    void insertName(const std::string &nodeName, const AftNodeToken nodeToken) {
        _nameTable->insert(nodeName, nodeToken);
    }
    
    void insertType(const std::string &typeName, const AftTypeIndex typeIndex) {
        _typeTable->insert(typeName, typeIndex);
    }
    
    void insertType(const std::string &typeName) {
        _typeTable->insert(typeName);
    }
    
    void insertField(const std::string &fieldName, const AftFieldIndex fieldIndex) {
        _fieldTable->insert(fieldName, fieldIndex);
    }
    
    void insertProto(const std::string &protoName, const AftProtoIndex protoIndex) {
        _protoTable->insert(protoName, protoIndex);
    }
    
    void insertGroup(const std::string &groupName, const AftGroupIndex groupIndex) {
        _groupTable->insert(groupName, groupIndex);
    }
    
    AftGroup insertGroup(const std::string &groupName) {
        return std::move(_groupTable->insert(groupName));
    }
    
    void insertEncap(const std::string &encapName, const AftEncapIndex encapIndex) {
        _encapTable->insert(encapName, encapIndex);
    }
    
    void insertDecap(const std::string &decapName, const AftDecapIndex decapIndex) {
        _decapTable->insert(decapName, decapIndex);
    }

    //
    // Port Management
    //
    void setMaxInputPort(AftIndex maxIndex);
    AftIndex inputPortCount();
    AftIndex inputPortMax() { return _inputPorts->max(); };
    bool outputPortByIndex(AftIndex portIndex, AftNodeToken &portToken);
    bool outputPortByName(const std::string &portName, AftNodeToken &portToken);
    
    void setMaxOutputPort(AftIndex maxIndex);
    AftIndex outputPortCount();
    AftIndex outputPortMax() { return _outputPorts->max(); };
    bool inputPortByIndex(AftIndex portIndex, AftNodeToken &portToken);
    bool inputPortByName(const std::string &portName, AftNodeToken &portToken);
    AftPortPtr setInputPortByIndex(AftIndex portIndex, AftNodeToken nextToken);
    AftPortPtr setInputPortByName(const std::string &portName, AftNodeToken nextToken);
    
    //
    // Node and token interface
    //
    virtual AftNodePtr   nodeForToken(AftNodeToken nodeToken) { return nullptr; };
    virtual AftNodePtr   nodeGetNext(AftNodeToken nodeToken) { return nullptr; };
    virtual AftNodeToken tokenMax() const { return 0; };
    virtual AftNodeToken tokenAllocate() = 0;
};

///
/// @class   AftSandbox
/// @brief   Primary class for managing Aft nodes and entries
///
class AftSandbox: public AftSandboxCore, public AftReceiver, public AftValidator {
protected:
    friend AfiSandboxRpc; ///< AfiSandboxRpc accesses the protected creation methods

    AftSandboxWeakPtr      _selfPtr;           ///< Weak pointer to itself
    AftTransportPtr        _transport;         ///< Transport responsible for the server connection
    AftTransportSessionPtr _session;           ///< Transport session for the sandbox
    AftTokenTablePtr       _tokenTable;        ///< Token allocation table for the sandbox
    AftTelemetryResponse   _telemetryResponse; ///< Set response handler for telemetry
    
    bool                   _validation;  ///< Controls whether validation should be performed

    void _insert(const AftNodePtr &node);
    void _remove(AftNodeToken nodeToken);
            
public:
    //
    // Constructor and destructor
    //
    ///
    /// @brief              Default constructor for sandboxes
    /// @param [in] newName Unique name of new sandbox
    ///
    AftSandbox(const std::string &newName);
    ~AftSandbox();
    
    ///
    /// @brief              Default copy constructor for sandboxes
    /// @param [in] sandbox Sandbox to use as template for new sandbox
    ///
    AftSandbox(const AftSandbox &sandbox);

    ///
    /// @brief              Default copy constructor for sandboxes
    /// @param [in] sandbox Sandbox to use as template for new sandbox
    ///
    AftSandbox(const AftSandboxCore &sandbox);

    //
    // Shared pointer management
    //
    static AftSandboxPtr create(const std::string &newName);
    static AftSandboxPtr create(const AftSandboxPtr &sandbox);
    static AftSandboxPtr createFromCore(const AftSandboxCorePtr &sandbox);
    
    //
    // Sandbox types, field and tokens
    //
    bool isValidToken(AftNodeToken nodeToken);
    bool isOfType(AftNodeToken nodeToken, const std::string &typeName);
    
    //
    // Insertion and removal management
    //
    bool send(const AftInsertPtr &insert);
    bool send(const AftInsertPtr &insert, AftRemovePtr &remove);
    bool send(const AftRemovePtr &remove);
    bool send(const AftOperationPtr &op);
    
    bool send(const AftOperationPtr &op, AftOperationResponse response);
    bool sendAndWait(const AftOperationPtr &op, uint32_t timeout = AFT_OPERATION_TIMEOUT);
    
    AftNodeToken insert(const AftNodePtr &node);
    bool         insert(const AftNodePtr &node, AftNodeToken nodeToken);
    bool         insert(const AftEntryPtr &entry);
    bool         remove(AftNodeToken nodeToken);
    bool         remove(const AftEntryPtr &entry);
    
    //
    // Accessors
    //
    
    ///
    /// @brief Set name of sandbox
    /// @param [in] newName String to set sandbox name to
    ///
    void setName(const std::string &newName) { _name = newName; };
    
    //
    // Token management
    //
    void    tokenSetIsCached(const bool newIsCached) { _tokenTable->setIsCached(newIsCached); };
    virtual AftNodeToken tokenAllocate() { return _tokenTable->nextToken(); };
    virtual AftNodePtr   nodeForToken(AftNodeToken nodeToken) { return _tokenTable->nodeForToken(nodeToken); };
    virtual AftNodePtr   nodeGetNext(AftNodeToken nodeToken) { return _tokenTable->nodeGetNext(nodeToken); };
    virtual AftNodeToken tokenMax() const { return 0; };

    //
    // Receiver methods
    //
    ///
    /// @brief Sets the underlying transport to be used by the sandbox
    /// @param [in] newTransport Reference to transport pointer
    ///
    void  setTransport(const AftTransportPtr &newTransport) { _transport = newTransport; };
    AftTransportPtr transport() { return _transport; };
    bool  hasTransport() { return _transport != nullptr; };
    void  setSession(const AftTransportSessionPtr &newSession) { _session = newSession; };
    AftTransportSessionPtr session() { return _session; };
    bool  hasSession() { return _session != nullptr; };
    
    bool  receive(const AftInsertPtr &insert);
    bool  receive(const AftRemovePtr &remove);
    bool  receive(const AftNodeInfoPtr &info);
    bool  receive(const AftNodeTestPtr &test);
    bool  receive(const AftEntryTestPtr &test);
    
    /// @cond
    bool testNodeIsPresent(AftNodeToken nodeToken);
    bool testNodeIsPresent(const AftNodePtr &node) { return testNodeIsPresent(node->nodeToken()); };
    bool testNodeIsInGroup(AftNodeToken nodeToken, AftGroupIndex groupIndex);
    bool testNodeIsInGroup(const AftNodePtr &node, AftGroupIndex groupIndex) { return testNodeIsInGroup(node->nodeToken(), groupIndex); };
    bool testNodeHasNext(AftNodeToken nodeToken, AftNodeToken nextToken);
    bool testNodeHasNext(const AftNodePtr &node, AftNodeToken nextToken) { return testNodeHasNext(node->nodeToken(), nextToken); };
    uint32_t testNodeNextCount(AftNodeToken nodeToken);
    uint32_t testNodeNextCount(const AftNodePtr &node) { return testNodeNextCount(node->nodeToken()); };
    
    bool testEntryIsPresent(const AftEntryPtr &entry);
    bool testEntryHasNext(const AftEntryPtr &entry, AftNodeToken nextToken);
    /// @endcond
    
    //
    // Validator Interface
    //
    void setValidation(const bool newValidation) { _validation = newValidation; };
    
    bool validate(const AftNodePtr &node);
    bool validate(const AftEntryPtr &node);
    
    bool validate(AftNodeToken nodeToken, std::ostream &os);
    bool validate(const AftTokenVector &nodeTokens, std::ostream &os);
    bool validate(const AftField &field, std::ostream &os);
    bool validate(const AftFieldVector &fields, std::ostream &os);
    bool validate(const AftKey &key, std::ostream &os);
    bool validate(const AftKeyVector &keys, std::ostream &os);
    
    //
    // Telemetry
    //
    void setTelemetry(AftTelemetryResponse response);
    
    //
    // Debug
    //
    std::ostream &description (std::ostream &os) const;

    ///
    /// @brief           Define << class operator to append description to an output stream
    /// @param [in] os   Reference to output stream to append description to
    /// @param [in] sandbox Reference to sandbox to generate description for
    /// @returns         Reference to output stream with text description of sandbox appended
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftSandboxPtr &sandbox) {
        return sandbox->description(os);
    }
};

///
/// @}
///

#endif
