//
//  AftOperation.h
//  Aft - A Forwarding Toolkit
//
/// @file  AftOperation.h
/// @brief Fundamental type definitions used by Aft operations
//
//  Created by Scott Mackie on 11/12/16.
//  Copyright (c) 2016 Juniper Networks Inc. All rights reserved.
//

#ifndef __AftOperation__
#define __AftOperation__

#include "jnx/AftTypes.h"
#include "jnx/AftLogV1.h"
#include "jnx/AftSandbox.h"
#include <thread>
#include <functional>
#include <mutex>
#include <chrono>
#include <condition_variable>

///
/// @addtogroup AftOperation
/// @{
///

///
/// @class   AftOperation
/// @brief   Base virtual class used for all thread dispatch operations
///

class AftOperation {
public:
    typedef enum {
        AftOperationUnknown = 0,
        AftOperationInsert,
        AftOperationRemove,
        AftOperationSandboxInfo,
        AftOperationSandboxFind,
        AftOperationNodeInfo,
        AftOperationNodeActive,
        AftOperationNodeTest,
        AftOperationEntryTest
    } Type;

protected:
    AftOperation::Type       _type;
    uint64_t                 _sequence;
    bool                     _synchronous;
    uint32_t                 _timeout;
    bool                     _status;
    bool                     _completed;
    std::mutex               _mutex;
    std::condition_variable  _condition;
    AftOperationResponse     _response;

public:
    AftOperation(AftOperation::Type newType,
                 bool newSynchronous = false,
                 uint32_t newTimeout = AFT_OPERATION_TIMEOUT): _type(newType),
                                                               _synchronous(newSynchronous),
                                                               _timeout(newTimeout),
                                                               _status(false),
                                                               _completed(false),
                                                               _response(nullptr) {};
    virtual ~AftOperation() {}

    AftOperation::Type     type()        const { return _type; };
    bool                   status()      const { return _status; };
    bool                   completed()   const { return _completed; };
    bool                   synchronous() const { return _synchronous; };
    uint32_t               timeout()     const { return _timeout; };
    const AftOperationResponse &response() const { return _response; };

    void setStatus(bool newStatus)           { _status = newStatus; } ;
    void setTimeout(uint32_t newTimeout)     { _timeout = newTimeout; } ;
    void setSynchronous(bool newSynchronous) { _synchronous = newSynchronous; } ;
    void setResponse(const AftOperationResponse &newResponse) { _response = newResponse; } ;

    virtual bool execute  (const AftReceiverPtr &receiver, const AftOperationPtr &op) = 0;
    virtual void complete (const AftOperationPtr &op) {
        //
        // Do we have a response handler?
        //
        if (op->response() != nullptr) {
            //
            // We do. Call it.
            //
            op->response()(op);
        }

        //
        // Signal that the operation is completed
        //
        op->signalCompleted();
    }

    ///
    /// @brief              Utility downcast for insert blocks
    ///
    template <class T>
    static std::shared_ptr<T> downcast (const AftOperationPtr &op) {
        return std::static_pointer_cast<T>(op);
    };

    /// @brief Blocks a thread until an operation is completed
    bool wait() {
        std::unique_lock<std::mutex> mlock(_mutex);

        //
        // Do we have a timeout?
        //
        if (_timeout) {
            //
            // We do have a timeout
            //
            return _condition.wait_for(mlock, std::chrono::milliseconds(_timeout), [this]{ return _completed;});
        } else {
            //
            // We don't have a timeout. Wait indefinitely
            //
            _condition.wait(mlock, [this]{ return _completed;});
            return true;
        }
    }

    /// @brief Signals that an operation has completed
    void signalCompleted() {
        std::lock_guard<std::mutex> guard(_mutex);
        _completed = true;
        _condition.notify_one();
    }

};

typedef std::vector<AftGroupTable::Pair> AftGroupPairVector;
typedef std::vector<AftNameTable::Pair> AftNamePairVector;

///
/// @class   AftInsert
/// @brief   Transactional insert used for inserting sandbox state
///
class AftInsert: public AftOperation {
protected:
    friend AftSandbox; ///< AftSandbox accesses the protected vectors
    friend AftRemove;  ///< AftRemove accesses the protected vectors

    AftSandboxCorePtr  _sandbox;  ///< Pointer to sandbox to use to allocate tokens
    AftNodeVector      _nodes;    ///< Vector of AftNode objects to insert in sandbox
    AftEntryVector     _entries;  ///< Vector of AftEntry objects to insert sandbox
    AftNamePairVector  _names;    ///< Vector of name table entries to insert in sandbox
    AftGroupPairVector _groups;   ///< Vector of group table entries to insert in sandbox
    bool               _executed; ///< True if the insert has been executed

    ///
    /// @brief Internal constructor for dependent insert blocks
    ///
    AftInsert(): AftOperation(AftOperationInsert), _sandbox(nullptr), _executed(false) {};

public:
    ///
    /// @brief              Factory constructor for insert blocks
    /// @param [in] sandbox Reference to sandbox to use for insertion
    ///
    static AftInsertPtr create(const AftSandboxCorePtr &sandbox) {
        return std::make_shared<AftInsert>(sandbox);
    }

    ///
    /// @brief              Constructor for insert blocks
    /// @param [in] sandbox Reference to sandbox to use for insertion
    ///
    AftInsert(const AftSandboxCorePtr &sandbox): AftOperation(AftOperationInsert), _sandbox(sandbox), _executed(false) {};
    virtual ~AftInsert() {
#ifdef LATER
        //
        // If we have state in the insert block, check to see if we've sent this or not
        //
        if ((!_executed) && (_nodes.size() || _entries.size())) {
            AftLog::error(_sandbox->name(), "AftInsert destroyed with active state");
        }
#endif
    };

    ///
    /// @brief                Push node into insert block
    /// @param [in] node      Reference to node to push into insert block
    /// @param [in] nodeToken Token to use for pushed node
    /// @returns              Allocated token value of pushed node
    ///
    virtual AftNodeToken _push (const AftNodePtr &node, AftNodeToken nodeToken) {
        //
        // Check the type against our type registry
        //
        AftTypeIndex typeIndex = 0;

        if (!_sandbox->findType(node->nodeType(), typeIndex)) {
            assert("Illegal type for sandbox");
        }
        node->setNodeTypeIndex(typeIndex);

        //
        // If the token is invalid, assign one
        //
        if (nodeToken == AFT_NODE_TOKEN_NONE) {
            nodeToken = _sandbox->tokenAllocate();
        }
        node->setNodeToken(nodeToken);

        _nodes.push_back(node);
        return node->nodeToken();
    }

    ///
    /// @brief                Push node into insert block
    /// @param [in] node      Reference to node to push into insert block
    /// @param [in] nodeToken Token to use for pushed node
    /// @returns              Allocated token value of pushed node
    ///
    AftNodeToken push (const AftNodePtr &node, AftNodeToken nodeToken) {
        return _push(node, nodeToken);
    }

    ///
    /// @brief                Push node into the insert block
    /// @param [in] node      Reference to node to push into insert block
    /// @returns              Allocated token value of pushed node
    ///
    AftNodeToken push (const AftNodePtr &node) {
        return _push(node, node->nodeToken());
    }

    ///
    /// @brief                Push entry into the insert block
    /// @param [in] entry     Reference to entry to push into insert block
    ///
    void push (const AftEntryPtr &entry) {
        _entries.push_back(entry);
    }

    ///
    /// @brief                Push name into the insert block
    /// @param [in] namePair  Reference to name tuple to push into insert block
    ///
    void push (const AftNameTable::Pair &namePair) {
        _names.push_back(namePair);
    }

    ///
    /// @brief                   Push name into the insert block
    /// @param [in] newName      Reference to name to push into insert block
    /// @param [in] newNodeToken Reference to name to push into insert block
    ///
    void push (const std::string &newName, const AftNodeToken newNodeToken) {
        _names.push_back(AftNameTable::Pair(newName, newNodeToken));
    }

    ///
    /// @brief                Push group into the insert block
    /// @param [in] groupPair Reference to name tuple to push into insert block
    ///
    AftGroupIndex push (const AftGroupTable::Pair &groupPair) {
        _groups.push_back(groupPair);
        return groupPair.entry();
    }

    ///
    /// @brief Set the executed flag
    ///
    void setExecuted (bool isExecuted) { _executed = isExecuted; };

    /// @returns Vector of AftNode objects to insert in sandbox
    const AftNodeVector &nodes() const { return _nodes; };

    /// @returns Vector of AftNode objects to insert in sandbox
    size_t numNodes() const { return _nodes.size(); };

    /// @returns Vector of AftEntry objects to insert in sandbox
    const AftEntryVector &entries() const { return _entries; };

    /// @returns Vector of AftEntry objects to insert in sandbox
    size_t numEntries() const { return _entries.size(); };

    /// @returns Vector of AftNameTable::Pair objects to insert sandbox
    const AftNamePairVector &names() const { return _names; };

    /// @returns Vector of AftGroupTable::Pair objects to insert sandbox
    const AftGroupPairVector &groups() const { return _groups; };

    ///
    /// @brief                 Execute the insert operation
    /// @param [in] receiver   Reference to receiver to perform operation on
    ///
    virtual bool execute (const AftReceiverPtr &receiver, const AftOperationPtr &op) {
        AftInsertPtr insert = AftOperation::downcast<AftInsert>(op);
        _status = receiver->receive(insert);
        _executed = true;
        return _status;
    }
};

///
/// @class   AftRemove
/// @brief   Transactional insert used for removing sandbox state
///
class AftRemove: public AftOperation {
protected:
    friend AftSandbox; ///< AftSandbox accesses protected vectors

    AftTokenVector     _nodes;    ///< Vector of node tokens to delete
    AftEntryVector     _entries;  ///< Vector of entries to delete
    AftNamePairVector  _names;    ///< Vector of name table entries to insert in sandbox
    AftGroupPairVector _groups;   ///< Vector of group table entries to insert in sandbox
    bool               _executed; ///< True if the remove has been executed

public:
    ///
    /// @brief              Factory constructor for remove blocks
    ///
    static AftRemovePtr create() {
        return std::make_shared<AftRemove>();
    }

    ///
    /// @brief              Factory constructor for insert blocks
    /// @param [in] insert  Reference to insert block to convert to remove block
    ///
    static AftRemovePtr create(const AftInsertPtr &insert) {
        return std::make_shared<AftRemove>(insert);
    }

    ///
    /// @brief              Constructor for insert blocks
    /// @param [in] insert  Reference to insert block to convert to remove block
    ///
    AftRemove(const AftInsertPtr &insert): AftOperation(AftOperationRemove), _executed(false) {
        //
        // Loop backwards through the entries we have and build the remove from that
        //
        AftEntryVector::reverse_iterator reit = insert->_entries.rbegin();
        for (; reit != insert->_entries.rend(); ++reit) {
            push(*reit);
        }

        //
        // Loop backwards through the nodes we have and build the remove from that
        //
        AftNodeVector::reverse_iterator rnit = insert->_nodes.rbegin();
        for (; rnit != insert->_nodes.rend(); ++rnit) {
            push(*rnit);
        }
    };

    AftRemove(): AftOperation(AftOperationRemove), _executed(false) {};
    ~AftRemove() {
#ifdef LATER
        //
        // If we have state in the insert block, check to see if we've sent this or not
        //
        if ((!_executed) && (_nodes.size() || _entries.size())) {
            AftLog::error("", "AftRemove destroyed with active state");
            assert(_executed);
        }
#endif
    };

    ///
    /// @brief Used to trim the remove block of any nodes/entries required by an update
    /// @param [in] newRemove Reference to remove block to filter against
    ///
    void update (const AftRemovePtr &newRemove) {
        //
        // Loop through the entries we have and look for matches in newRemove
        //
        for (AftEntryPtr entry: newRemove->_entries) {
            erase(entry);
        }

        for (AftNodeToken nodeToken: newRemove->_nodes) {
            erase(nodeToken);
        }
    }

    ///
    /// @brief                Push token on to remove block
    /// @param [in] nodeToken Token to use for pushed node
    ///
    void push (const AftNodeToken nodeToken) {
        return _nodes.push_back(nodeToken);
    }

    ///
    /// @brief                   Push node on to remove block
    /// @param [in] newNode      Reference to node to push into remove block
    ///
    void push (const AftNodePtr &newNode) {
        return _nodes.push_back(newNode->nodeToken());
    }

    ///
    /// @brief                   Push entry on to remove block
    /// @param [in] newEntry     Reference to entry to push into remove block
    ///
    void push (const AftEntryPtr &newEntry) {
        _entries.push_back(newEntry);
    }

    ///
    /// @brief               Push name into the insert block
    /// @param [in] namePair Reference to name tuple to push into insert block
    ///
    void push (const AftNameTable::Pair &namePair) {
        _names.push_back(namePair);
    }

    ///
    /// @brief                Push group into the insert block
    /// @param [in] groupPair Reference to group tuple to push into insert block
    ///
    AftGroupIndex push (const AftGroupTable::Pair &groupPair) {
        _groups.push_back(groupPair);
        return groupPair.entry();
    }

    ///
    /// @brief                Erase a node token in a remove block
    /// @param [in] nodeToken Node token to find and erase
    ///
    void erase (const AftNodeToken nodeToken) {
        auto it = std::find(_nodes.begin(), _nodes.end(), nodeToken);
        if (it != _nodes.end())
            _nodes.erase(it);
    }

    ///
    /// @brief                   Erase an entry in a remove block
    /// @param [in] newEntry     Reference to entry to find and erase
    ///
    void erase (const AftEntryPtr &newEntry) {
        //
        // Gulp. Lambda functions. This allows us to compare on the references to shared_ptrs
        // of AftEntry without having to get too fancy. We capture a reference to the supplied
        // entry so we can compare it against what the iterator spits out.
        //
        auto it = std::find_if(_entries.begin(), _entries.end(), [&newEntry](const AftEntryPtr &e) -> bool {
            return (newEntry == e);
        });

        //
        // If we found something, erase it
        //
        if (it != _entries.end())
            _entries.erase(it);
    }

    /// @returns Vector of AftNode objects to remove from sandbox
    const AftTokenVector &nodes() const { return _nodes; };

    /// @returns Vector of AftNode objects to insert in sandbox
    size_t numNodes() const { return _nodes.size(); };

    /// @returns Vector of AftEntry objects to remove from sandbox
    const AftEntryVector &entries() const { return _entries; };

    /// @returns Vector of AftEntry objects to insert in sandbox
    size_t numEntries() const { return _entries.size(); };

    /// @returns Vector of AftNameTable::Pair objects to remove from sandbox
    const AftNamePairVector &names() const { return _names; };

    /// @returns Vector of AftGroupTable::Pair objects to remove from sandbox
    const AftGroupPairVector &groups() const { return _groups; };

    ///
    /// @brief                 Execute the remove operation
    /// @param [in] receiver   Reference to receiver to perform operation on
    ///
    virtual bool execute (const AftReceiverPtr &receiver, const AftOperationPtr &op) {
        AftRemovePtr remove = AftOperation::downcast<AftRemove>(op);
        _status = receiver->receive(remove);
        _executed = true;
        return _status;
    }
};

///
/// @class   AftSandboxInfo
/// @brief   Operation used to control various sandbox info settings
///
class AftSandboxInfo: public AftOperation {
public:
    typedef enum {
        Heartbeat = 0,
        Telemetry,
        Status
    } OpEnum;

protected:
    AftSandboxInfo::OpEnum _infoOp;
    bool                   _infoEnabled;
    AftParameters::UPtr    _infoParams;
    AftParameters::UPtr    _replyParams;

public:
    ///
    /// @brief              Factory constructor for heartbeat operation
    ///
    static AftSandboxInfoPtr createHeartbeat() {
        return std::make_shared<AftSandboxInfo>(AftSandboxInfo::Heartbeat, true);
    }

    ///
    /// @brief              Factory constructor for telemetry operation
    ///
    static AftSandboxInfoPtr createTelemetry(const bool newInfoEnabled) {
        return std::make_shared<AftSandboxInfo>(AftSandboxInfo::Telemetry, newInfoEnabled);
    }

    ///
    /// @brief              Factory constructor for status operation
    ///
    static AftSandboxInfoPtr createStatus(const bool newInfoEnabled) {
        return std::make_shared<AftSandboxInfo>(AftSandboxInfo::Status, newInfoEnabled);
    }

    AftSandboxInfo(const AftSandboxInfo::OpEnum newInfoOp,
                   bool                         newInfoEnabled): AftOperation(AftOperationSandboxInfo),
                                                                _infoOp(newInfoOp),
                                                                _infoEnabled(newInfoEnabled),
                                                                _infoParams(nullptr),
                                                                _replyParams(nullptr) {};
    ~AftSandboxInfo() {};

    AftSandboxInfo::OpEnum infoOp() const { return _infoOp; };
    bool infoEnabled() const { return _infoEnabled; };

    /// @returns Optional information block for information request
    const AftParameters::UPtr &infoParams() const { return _infoParams; };

    /// @brief Set information block for information request
    void setInfoParams(AftParameters::UPtr newInfoParams) { _infoParams = std::move(newInfoParams); };

    /// @returns Optional reply block for information request
    const AftParameters::UPtr &replyParams() const { return _replyParams; };

    /// @brief Set optional reply block for information request
    void setReplyParams(AftParameters::UPtr newReplyParams) { _replyParams = std::move(newReplyParams); };

    ///
    /// @brief                 Execute the sandbox info operation
    /// @param [in] receiver   Reference to receiver to perform operation on
    ///
    virtual bool execute (const AftReceiverPtr &receiver, const AftOperationPtr &op) {
        AftSandboxInfoPtr i = AftOperation::downcast<AftSandboxInfo>(op);
        _status = receiver->receive(i);
        return _status;
    }
};

///
/// @class   AftSandboxFind
/// @brief   Operation used to look for various sandbox elements
///
class AftSandboxFind: public AftOperation {
public:
    typedef enum {
        Token = 0,
        Name,
        Regex,
        Type,
        Group
    } OpEnum;

protected:
    AftSandboxFind::OpEnum _findOp;
    AftNodeToken           _findToken   = AFT_NODE_TOKEN_NONE;
    std::string            _findString  = "";
    AftGroupIndex          _findGroup   = AFT_GROUP_INDEX_NONE;
    AftNodeVector          _findReply;

public:
    ///
    /// @brief              Factory constructor for token find
    ///
    static AftSandboxFindPtr createFindToken(const AftNodeToken newFindToken) {
        return std::make_shared<AftSandboxFind>(AftSandboxFind::Token, newFindToken, AFT_GROUP_INDEX_NONE);
    }

    ///
    /// @brief              Factory constructor for name find
    ///
    static AftSandboxFindPtr createFindName(const std::string &newFindString) {
        return std::make_shared<AftSandboxFind>(AftSandboxFind::Name, newFindString);
    }

    ///
    /// @brief              Factory constructor for regex find
    ///
    static AftSandboxFindPtr createFindRegex(const std::string &newFindString) {
        return std::make_shared<AftSandboxFind>(AftSandboxFind::Regex, newFindString);
    }

    ///
    /// @brief              Factory constructor for type find
    ///
    static AftSandboxFindPtr createFindType(const std::string &newFindString) {
        return std::make_shared<AftSandboxFind>(AftSandboxFind::Type, newFindString);
    }

    ///
    /// @brief              Factory constructor for group find
    ///
    static AftSandboxFindPtr createFindGroup(const AftGroupIndex &newFindGroup) {
        return std::make_shared<AftSandboxFind>(AftSandboxFind::Group, AFT_NODE_TOKEN_NONE, newFindGroup);
    }

    AftSandboxFind(const AftSandboxFind::OpEnum newFindOp,
                   std::string                  newFindString): AftOperation(AftOperationSandboxFind),
                                                                _findOp(newFindOp),
                                                                _findString(newFindString) {};

    AftSandboxFind(const AftSandboxFind::OpEnum newFindOp,
                   AftNodeToken                 newFindToken,
                   AftGroupIndex                newFindGroup): AftOperation(AftOperationSandboxFind),
                                                               _findOp(newFindOp),
                                                               _findToken(newFindToken),
                                                               _findGroup(newFindGroup) {};

    AftSandboxFind::OpEnum findOp()     const { return _findOp; };
    std::string            findString() const { return _findString; };
    AftNodeToken           findToken()  const { return _findToken; };
    AftGroupIndex          findGroup()  const { return _findGroup; };
    const AftNodeVector   &findReply()  const { return  _findReply; };

    void add (const AftNodePtr &node) {
        _findReply.push_back(node);
    }

    size_t findCount() const { return _findReply.size(); };

    ///
    /// @brief                 Execute the sandbox info operation
    /// @param [in] receiver   Reference to receiver to perform operation on
    ///
    virtual bool execute (const AftReceiverPtr &receiver, const AftOperationPtr &op) {
        AftSandboxFindPtr i = AftOperation::downcast<AftSandboxFind>(op);
        _status = receiver->receive(i);
        return _status;
    }
};

///
/// @class   AftNodeTest
/// @brief   Operation used to provide unit test feedback
///
class AftSandbox;
typedef std::shared_ptr<AftNodeTest> AftNodeTestPtr; ///< Shared pointer type for node tests

class AftNodeTest: public AftOperation {
public:
    typedef enum {
        IsPresent = 0,
        HasNext,
        NextCount,
        IsInGroup
    } OpEnum;

protected:
    friend AftSandbox; ///< AftSandbox accesses protected vectors

    AftNodeTest::OpEnum _testOp;
    AftNodeToken        _node     = AFT_NODE_TOKEN_NONE;
    AftNodeToken        _nodeNext = AFT_NODE_TOKEN_NONE;
    uint32_t            _count    = 0;
    AftGroupIndex       _group    = AFT_GROUP_INDEX_NONE;

public:
    ///
    /// @brief                Factory constructor for node tests
    /// @param [in] newNode Reference to node token to check for presence
    ///
    static AftNodeTestPtr createNodeIsPresent(const AftNodeToken newNode) {
        return std::make_shared<AftNodeTest>(AftNodeTest::IsPresent, newNode);
    }

    ///
    /// @brief                   Factory constructor for node tests
    /// @param [in] newNode      Reference to node token to check for next pointer
    /// @param [in] newNodeNext  Reference to next node token to check
    ///
    static AftNodeTestPtr createNodeHasNext(const AftNodeToken newNode, const AftNodeToken newNodeNext) {
        return std::make_shared<AftNodeTest>(AftNodeTest::HasNext, newNode, newNodeNext);
    }

    ///
    /// @brief                Factory constructor for node tests
    /// @param [in] newNode   Reference to node token to check next count
    ///
    static AftNodeTestPtr createNodeNextCount(const AftNodeToken newNode) {
        return std::make_shared<AftNodeTest>(AftNodeTest::NextCount, newNode);
    }

    ///
    /// @brief                Factory constructor for node tests
    /// @param [in] newNode   Reference to node token to check for group
    /// @param [in] newGroup  Group index to check node is present in
    ///
    static AftNodeTestPtr createNodeIsInGroup(const AftNodeToken newNode, const AftGroupIndex newGroup) {
        return std::make_shared<AftNodeTest>(AftNodeTest::IsInGroup, newNode, AFT_NODE_TOKEN_NONE, newGroup);
    }

    AftNodeTest(const AftNodeTest::OpEnum newTestOp,
                const AftNodeToken        newNode,
                const AftNodeToken        newNodeNext = AFT_NODE_TOKEN_NONE,
                const AftGroupIndex       newGroup = AFT_GROUP_INDEX_NONE): AftOperation(AftOperationNodeTest, true, AFT_OPERATION_TIMEOUT),
                                                                            _testOp(newTestOp),
                                                                            _node(newNode),
                                                                            _nodeNext(newNodeNext),
                                                                            _group(newGroup) {};
    ~AftNodeTest() {};

    AftNodeTest::OpEnum testOp()   const { return _testOp; };
    AftNodeToken        node()     const { return _node; };
    AftNodeToken        nodeNext() const { return _nodeNext; };
    AftGroupIndex       group()    const { return _group; };
    uint32_t            count()    const { return _count; };
    void setCount(uint32_t newCount) { _count = newCount; };

    ///
    /// @brief                 Execute the remove operation
    /// @param [in] receiver   Reference to receiver to perform operation on
    ///
    virtual bool execute (const AftReceiverPtr &receiver, const AftOperationPtr &op) {
        AftNodeTestPtr test = AftOperation::downcast<AftNodeTest>(op);
        _status = receiver->receive(test);
        return _status;
    }
};

///
/// @class   AftEntryTest
/// @brief   Operation used to provide unit test feedback
///
class AftEntryTest: public AftOperation {
public:
    typedef enum {
        IsPresent = 0,
        HasNext,
    } OpEnum;

protected:
    friend AftSandbox; ///< AftSandbox accesses protected vectors

    AftEntryTest::OpEnum _testOp;
    AftEntryPtr          _entry;
    AftNodeToken         _nodeNext;

public:
    ///
    /// @brief                Factory constructor for node tests
    /// @param [in] newEntry  Reference to entry to check for presence
    ///
    static AftEntryTestPtr createEntryIsPresent(const AftEntryPtr &newEntry) {
        return std::make_shared<AftEntryTest>(AftEntryTest::IsPresent, newEntry);
    }

    ///
    /// @brief                  Factory constructor for node tests
    /// @param [in] newEntry    Reference to entry to check for presence
    /// @param [in] newNodeNext Reference to next node token to check
    ///
    static AftEntryTestPtr createEntryHasNext(const AftEntryPtr &newEntry, const AftNodeToken newNodeNext) {
        return std::make_shared<AftEntryTest>(AftEntryTest::HasNext, newEntry, newNodeNext);
    }

    AftEntryTest(const AftEntryTest::OpEnum  newTestOp,
                 const AftEntryPtr          &newEntry,
                 const AftNodeToken          newNodeNext = AFT_NODE_TOKEN_NONE): AftOperation(AftOperationEntryTest, true, AFT_OPERATION_TIMEOUT),
                                                                                 _testOp(newTestOp),
                                                                                 _entry(newEntry),
                                                                                 _nodeNext(newNodeNext) {};
    ~AftEntryTest() {};

    AftEntryTest::OpEnum testOp()   const { return _testOp; };
    const AftEntryPtr    &entry()   const { return _entry; };
    AftNodeToken         nodeNext() const { return _nodeNext; };

    ///
    /// @brief                 Execute the remove operation
    /// @param [in] receiver   Reference to receiver to perform operation on
    ///
    virtual bool execute (const AftReceiverPtr &receiver, const AftOperationPtr &op) {
        AftEntryTestPtr test = AftOperation::downcast<AftEntryTest>(op);
        _status = receiver->receive(test);
        return _status;
    }
};

typedef std::vector<AftNodeInfoRequest>      AftNodeInfoRequestVector;
typedef std::map<AftNodeToken, AftInfoUPtr>  AftNodeInfoReplyMap;

///
/// @class   AftNodeInfo
/// @brief   Operation used to request live info from node
///
class AftNodeInfo: public AftOperation {
public:

protected:
    friend AftSandbox; ///< AftSandbox accesses protected vectors

    AftNodeInfoRequestVector _infoRequest;
    AftNodeInfoReplyMap      _infoReply;

public:
    ///
    /// @brief                      Factory constructor for node info requests
    ///
    static AftNodeInfoPtr create() {
        return std::make_shared<AftNodeInfo>();
    }

    AftNodeInfo(): AftOperation(AftOperationNodeInfo) {};
    ~AftNodeInfo() {};

    const AftNodeInfoRequestVector &nodes()   const { return _infoRequest; };
    const AftNodeInfoReplyMap      &info()    const { return _infoReply; };

    void add (AftNodeInfoRequest &&infoRequest) {
        _infoRequest.emplace_back(std::move(infoRequest));
    }

    ///
    /// @brief                   Push info reply
    /// @param [in] newInfo      Reference to info block to push
    ///
    void push (AftInfoUPtr &newInfo) {
        if (newInfo != nullptr) {
            _infoReply.insert(std::make_pair(newInfo->nodeToken(), std::move(newInfo)));
        }
    }

    ///
    /// @brief                   Get info reply for token
    /// @param [in] nodeToken    Node token to get info reply for
    /// @returns                 Reference to node information, if it exists
    ///
    template <class T>
    bool pull (const AftNodeToken nodeToken, std::unique_ptr<T> &newInfo) {
        auto it = _infoReply.find(nodeToken);

        //
        // If we didn't find the token, return false
        //
        if (it == _infoReply.end()) return false;

        //
        // We found the token. Move the unique_ptr out of the iterator
        // and erase it - the pull consumes it
        //
        auto ptr = std::move(it->second);
        _infoReply.erase(it);

        //
        // Cast the unique pointer to our reference and return true
        //
        newInfo = std::unique_ptr<T>(static_cast<T *>(ptr.release()));
        return true;
    }

    ///
    /// @brief                   Get first info reply
    /// @returns                 Reference to node information, if it exists
    ///
    template <class T>
    bool pull (std::unique_ptr<T> &newInfo) {
        auto it = _infoReply.begin();

        //
        // If we didn't find the token, return false
        //
        if (it == _infoReply.end()) return false;

        //
        // We found the token. Move the unique_ptr out of the iterator
        // and erase it - the pull consumes it
        //
        auto ptr = std::move(it->second);
        _infoReply.erase(it);

        //
        // Cast the unique pointer to our reference and return true
        //
        newInfo = std::unique_ptr<T>(static_cast<T *>(ptr.release()));
        return true;
    }

    ///
    /// @brief                 Execute the info request operation
    /// @param [in] receiver   Reference to receiver to perform operation on
    /// @param [in] op         Reference to operation
    ///
    virtual bool execute (const AftReceiverPtr &receiver, const AftOperationPtr &op) {
        AftNodeInfoPtr i = AftOperation::downcast<AftNodeInfo>(op);
        _status = receiver->receive(i);
        return _status;
    }
};

typedef std::vector<AftNodeActiveRequest>      AftNodeActiveRequestVector;
typedef std::map<AftNodeToken, AftActiveUPtr>  AftNodeActiveReplyMap;

///
/// @class   AftNodeActive
/// @brief   Operation used to set active state info on node
///
class AftNodeActive: public AftOperation {
public:

protected:
    friend AftSandbox; ///< AftSandbox accesses protected vectors

    AftNodeActiveRequestVector _activeRequest;
    AftNodeActiveReplyMap      _activeReply;

public:
    ///
    /// @brief                      Factory constructor for node info requests
    ///
    static AftNodeActivePtr create() {
        return std::make_shared<AftNodeActive>();
    }

    AftNodeActive(): AftOperation(AftOperationNodeActive) {};
    ~AftNodeActive() {};

    const AftNodeActiveRequestVector &nodes()    const { return _activeRequest; };
    const AftNodeActiveReplyMap      &state()    const { return _activeReply; };

    void add (AftNodeActiveRequest &&activeRequest) {
        _activeRequest.emplace_back(std::move(activeRequest));
    }

    ///
    /// @brief                   Push info reply
    /// @param [in] newActive    Reference to active block to push
    ///
    void push (AftActiveUPtr &newActive) {
        if (newActive != nullptr) {
            _activeReply.insert(std::make_pair(newActive->nodeToken(), std::move(newActive)));
        }
    }

    ///
    /// @brief                   Get info reply for token
    /// @param [in] nodeToken    Node token to get active reply for
    /// @param [out] newActive   Reference to returned active block, if successful
    /// @returns                 Reference to active  reply, if it exists
    ///
    template <class T>
    bool pull (const AftNodeToken nodeToken, std::unique_ptr<T> &newActive) {
        auto it = _activeReply.find(nodeToken);

        //
        // If we didn't find the token, return false
        //
        if (it == _activeReply.end()) return false;

        //
        // We found the token. Move the unique_ptr out of the iterator
        // and erase it - the pull consumes it
        //
        auto ptr = std::move(it->second);
        _activeReply.erase(it);

        //
        // Cast the unique pointer to our reference and return true
        //
        newActive = std::unique_ptr<T>(static_cast<T *>(ptr.release()));
        return true;
    }

    ///
    /// @brief                 Execute the active request operation
    /// @param [in] receiver   Reference to receiver to perform operation on
    /// @param [in] op         Reference to operation
    ///
    virtual bool execute (const AftReceiverPtr &receiver, const AftOperationPtr &op) {
        AftNodeActivePtr active = AftOperation::downcast<AftNodeActive>(op);
        _status = receiver->receive(active);
        return _status;
    }
};

///
/// @}
///

#endif
