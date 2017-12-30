//
/// @file   AftNode.h
/// @brief  General model definitions for base Aft Nodes
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

#ifndef __AftNode__
#define __AftNode__

#include "jnx/AftTypes.h"
#include "jnx/AftData.h"
#include "jnx/AftProto.h"
#include "jnx/AftGroup.h"
#include "jnx/AftField.h"
#include "jnx/AftKey.h"
#include "jnx/AftValidator.h"
#include "jnx/AftParameters.h"

using   std::vector;

#define AFT_NODE_NAME_DEFAULT  "node"               ///< Default node name
#define AFT_NODE_TOKEN_DEFAULT 0UL                  ///< Default token value
#define AFT_NODE_TOKEN_DISCARD 0UL                  ///< Default discard is the same as default token value
#define AFT_NODE_TOKEN_NONE    0xFFFFFFFFFFFFFFFFUL ///< We never allocate this token value
#define AFT_NODE_TOKEN_START   1024UL               ///< Default start of client tokens

static bool AftTokenIsSet (AftNodeToken token) { return token != AFT_NODE_TOKEN_NONE; };

#define AFT_MASK_DEFAULT       0xFFFFFFFFFFFFFFFFLL ///< We perform all operations on all cores

///
/// @defgroup AftNodes   Nodes
/// @brief Classes that define the core Aft forwarding nodes
///

///
/// @class AftInfoRequest
/// @brief Base class used for all Aft info requests
///
class AftNodeInfoRequest {
protected:
    AftNodeToken        _nodeToken;
    AftParameters::UPtr _infoParams;

public:
    ///
    /// @brief                    Constructor for node info requests
    /// @param [in] newNodeToken  Token of node for information request
    /// @param [in] newInfoParams Optional parameters for information request
    ///
    AftNodeInfoRequest(const AftNodeToken newNodeToken   = AFT_NODE_TOKEN_NONE,
                       AftParameters::UPtr newInfoParams = nullptr): _nodeToken(newNodeToken),
                                                                     _infoParams(std::move(newInfoParams)) {};

    /// @returns Node token for information request
    AftNodeToken nodeToken() const { return _nodeToken; };

    /// @returns Optional information block for information request
    const AftParameters::UPtr &infoParams() const { return _infoParams; };
};

///
/// @class AftInfo
/// @brief Base class used for all Aft info blocks
///
class AftInfo {
protected:
    AftNodeToken _nodeToken;

public:
    typedef std::unique_ptr<AftInfo> UPtr; ///< Pointer type for info

    AftInfo(const AftNodeToken newNodeToken = AFT_NODE_TOKEN_NONE): _nodeToken(newNodeToken) {};
    ~AftInfo() {};

    static std::unique_ptr<AftInfo> create(const AftNodeToken newNodeToken) {
        //
        // Create our object and return a unique pointer to it
        //
        return std::unique_ptr<AftInfo>(new AftInfo(newNodeToken));
    }

    /// @returns Node token for encoded information
    AftNodeToken nodeToken() const { return _nodeToken; };

    /// @returns Printable class type of info
    virtual const std::string infoType() const { return "AftInfo"; };
};

///
/// @class   AftNode
/// @brief   Base class for all Aft nodes
/// @ingroup AftNodes
///
/// @details This is the base class for all Aft nodes and forms the basic footprint of fundamental
/// class methods that all nodes must support. We never create AftNode objects directly. The class
/// is instantiated by inheriting from it in specific node classes and constructed via the
/// derived classes own constructor.
///
class AftNode {
protected:
    friend AftInsert;       ///< AftInsert accesses the setNodeName and setNodeToken methods
    friend AftSandboxCore;  ///< AftSandboxCore accesses the setNodeName and setNodeToken methods
    friend AftSandbox;      ///< AftSandbox accesses the setNodeName and setNodeToken methods
    friend AfiNodeRpc;      ///< AfiNodeRpc accesses the setNodeName and setNodeToken methods

    AftNodeToken        _nodeToken;       ///< Unique token value for node
    AftNodeToken        _nodeNext;        ///< Optional value of next node
    AftParameters::UPtr _nodeParams;      ///< Optional node parameters
    AftMask             _nodeMask = AFT_MASK_DEFAULT;   ///< Mask for entry
    AftGroupIndex       _nodeGroup = AFT_GROUP_INDEX_NONE; ///< Optional group index of node
    AftTypeIndex        _nodeTypeIndex;   ///< Scalar index type for node
    std::string         _nodeDebug;       ///< Optional string for debug info

    ///
    /// @brief                Set the token value of the node
    /// @param [in] newToken  Value of the token
    ///
    void setNodeToken(AftNodeToken newToken) { _nodeToken = newToken; };

    ///
    /// @brief                    Set the sandbox node type index
    /// @param [in] newTypeIndex  Node type index
    ///
    void setNodeTypeIndex(AftTypeIndex newTypeIndex) { _nodeTypeIndex = newTypeIndex; };

public:
    ///
    /// @brief           Convenience downcast template. Use this instead of manual casting if required
    /// @param [in] node Node to downcast
    ///
    template <class T>
    static std::shared_ptr<T> downcast(const AftNodePtr &node) {
        return std::static_pointer_cast<T>(node);
    }

    ///
    /// @brief           Copy constructor for a basic node
    /// @param [in] node Node to copy
    ///
    AftNode(const AftNode &node): _nodeToken(node.nodeToken()),
                                  _nodeNext(node.nodeNext()) {};

    ///
    /// @brief Construct an entry for a basic node
    /// @param [in] newToken Unique token for node
    /// @param [in] newNext  Optional Next token for node
    ///
    AftNode(const AftNodeToken newToken = AFT_NODE_TOKEN_NONE,
            const AftNodeToken newNext  = AFT_NODE_TOKEN_NONE): _nodeToken(newToken),
                                                                _nodeNext(newNext) {};

    ~AftNode() {};

    /// @returns Unique token for node
    operator AftNodeToken() const { return _nodeToken; }

    ///
    /// @brief           Define << class operator to append description to an output stream
    /// @param [in] os   Reference to output stream to append to
    /// @param [in] node Reference to node to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftNode &node) {
        return node.description(os);
    }

    void setNodeDebug(const std::string &newNodeDebug) { _nodeDebug = newNodeDebug; };
    const std::string &nodeDebug() const { return _nodeDebug; };

    /// @returns Unique token for node
    AftNodeToken nodeToken() const { return _nodeToken; };

    ///
    /// @brief                  Set the (optional) node group
    /// @param [in] newGroup    Group index for the node
    ///
    void setNodeGroup(AftGroupIndex newGroup) { _nodeGroup = newGroup; };

    /// @returns Searchable group for node
    AftGroupIndex nodeGroup()  const { return _nodeGroup; };

    /// @returns Type index for node
    AftTypeIndex  nodeTypeIndex()  const { return _nodeTypeIndex; };

    ///
    /// @brief                  Set the (optional) next node
    /// @param [in] newNext     Token for the next node
    ///
    void setNodeNext(AftNodeToken newNext) { _nodeNext = newNext; };

    /// @returns Unique token for node
    AftNodeToken nodeNext() const { return _nodeNext; };

    ///
    /// @brief                  Set the (optional) node mask
    /// @param [in] newMask     Mask for the node
    ///
    void setNodeMask(AftMask newMask) { _nodeMask = newMask; };

    /// @returns Mask for node
    AftMask nodeMask() const { return _nodeMask; };

    ///
    /// @brief                  Set the optional parameters of the node
    /// @param [in] newParams   Pointer to parameters for node
    ///
    void setNodeParameters(AftParameters::UPtr newParams) { _nodeParams = std::move(newParams); };

    ///
    /// @brief Returns true if the node has optional parameters
    ///
    bool hasNodeParameters() const { return _nodeParams != nullptr; };

    ///
    /// @brief                 Returns a vector of all of the parameter keys
    /// @param [out] paramKeys Reference to key vector to use
    /// @returns               True if the node has parameters
    ///
    bool nodeParameterKeys(AftParameterKeys &paramKeys) {
        if (_nodeParams == nullptr) return false;
        _nodeParams->parameterKeys(paramKeys);
        return true;
    }

    ///
    /// @brief           Convenience parameter access template. Use this to access node parameters
    /// @param [in] name Parameter name to look for
    /// @returns         Pointer to AftData if parameter is found
    ///
    template <typename T>
    bool nodeParameter(const std::string &name, T &resultValue) {
        if (_nodeParams == nullptr) return false;
        return _nodeParams->parameter<T>(name, resultValue);
    }

    ///
    /// @brief           Convenience parameter access template. Use this to access node parameters
    /// @param [in] name Parameter name to look for
    /// @returns         Pointer to AftData if parameter is found
    ///
    template <class T>
    std::shared_ptr<T> nodeParameterData(const std::string &name) {
        if (_nodeParams == nullptr) return nullptr;
        return _nodeParams->parameterData<T>(name);
    }

    ///
    /// @brief               Convenience parameter setter. Use this to set node parameters
    /// @param [in] name     Parameter name to set
    /// @param [in] newValue Reference to value to associate with name
    ///
    template <typename T>
    void setNodeParameter(const std::string &name, const T &newValue) {
        if (_nodeParams == nullptr) setNodeParameters(AftParameters::create());
        _nodeParams->setParameter<T>(name, newValue);
    }

    ///
    /// @brief           Convenience parameter setter. Use this to set node parameters
    /// @param [in] name Parameter name to set
    /// @param [in] data Reference to data to associate with name
    ///
    void setNodeParameterData(const std::string &name, const AftDataPtr &data) {
        if (_nodeParams == nullptr) setNodeParameters(AftParameters::create());
        _nodeParams->setParameterData(name, data);
    }

    /// @returns Printable class type of node
    virtual const std::string     nodeType() const { return "AftNode"; };

    /// @ returns true if node can take entries
    virtual bool            nodeIsContainer() const { return false; };

    /// @ returns true if node is a delete
    virtual bool            nodeIsDelete() const { return false; };

    /// @ returns true if node is active
    virtual bool            nodeIsActive() const { return false; };

    /// @ returns true if node is valid
    virtual bool            nodeIsValid(const AftValidatorPtr &validator, std::ostream &os) { return true; };

    /// @param [out] nextTokens    Appends valid next tokens to token vector
    virtual void nextNodes (AftTokenVector &nextTokens) {
        if (AftTokenIsSet(_nodeNext)) nextTokens.push_back(_nodeNext);
    };

    enum AftDescriptionFormat {
        Brief,
        Short,
        Detailed
    };
    ///
    /// @brief Append a description of the node to an output stream
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of node appended
    ///
    virtual std::ostream &descriptionFmt (std::ostream &os, AftDescriptionFormat format = Brief) const {
        os << nodeType() << " token:" << _nodeToken << " group:" << _nodeGroup;
        return os;
    }
    virtual std::ostream &description (std::ostream &os) const {
        os << nodeType() << " token:" << _nodeToken << " group:" << _nodeGroup;
        return os;
    }

    virtual AftInfoUPtr nodeInfo() const { return AftInfo::create(_nodeToken); };
};

///
/// @class AftActive
/// @brief Base class used for all Aft active blocks
///
class AftActive {
protected:
    AftNodeToken _nodeToken;
    bool         _nodeState;

public:
    typedef std::unique_ptr<AftActive> UPtr; ///< Pointer type for active

    AftActive(const AftNodeToken newNodeToken = AFT_NODE_TOKEN_NONE,
              bool newNodeState = true): _nodeToken(newNodeToken), _nodeState(newNodeState) {};
    ~AftActive() {};

    static std::unique_ptr<AftActive> create(const AftNodeToken newNodeToken, bool newNodeState) {
        //
        // Create our object and return a unique pointer to it
        //
        return std::unique_ptr<AftActive>(new AftActive(newNodeToken, newNodeState));
    }

    /// @returns Node token for encoded information
    AftNodeToken nodeToken() const { return _nodeToken; };

    /// @returns Node token for encoded information
    bool nodeState() const { return _nodeState; };

    /// @returns Printable class type of info
    virtual const std::string activeType() const { return "AftActive"; };
};

///
/// @class AftNodeActiveRequest
/// @brief Base class used for all Aft active requests
///
class AftNodeActiveRequest: public AftActive {
public:
    using AftActive::AftActive;
};

///
/// @class AftActiveNode
/// @brief Base class used for active state nodes
/// @ingroup AftNodes
///
class AftActiveNode: public AftNode {
protected:
    bool _nodeState;

public:
    typedef std::shared_ptr<AftActiveNode> Ptr; ///< Pointer type for active nodes

    ///
    /// @brief Factory generate aan active object. Only used for testing
    ///
    /// @param [in] newNodeState  Initial node state
    ///
    static AftActiveNode::Ptr create(bool newNodeState) {
        return std::make_shared<AftActiveNode>(newNodeState);
    }

    ///
    /// @brief           Copy constructor for an active node
    /// @param [in] node Active node to copy
    ///
    AftActiveNode(const AftActiveNode &node): AftNode(node) {};

    ///
    /// @brief Construct an active node
    ///
    /// @param [in] newNodeState  Initialization state of node
    ///
    AftActiveNode(const bool newNodeState): AftNode(), _nodeState(newNodeState) {};
    ~AftActiveNode() {};

    ///
    /// @brief                    Set the (optional) node state
    /// @param [in] newNodeState  Node state
    ///
    void setNodeState(const bool newNodeState) { _nodeState = newNodeState; };

    /// @returns State for node
    bool nodeState() const { return _nodeState; };

    virtual const std::string nodeType() const { return "AftActiveNode"; };
    virtual bool              nodeIsActive() const { return true; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " nodeState:" << _nodeState;
        return os;
    }
};

///
/// @class AftDeleteNode
/// @brief Class used to delete the referenced node token
/// @ingroup AftNodes
///
class AftDeleteNode: public AftNode {
public:
    ///
    /// @brief Factory generate a delete object
    ///
    /// @param [in] delToken  Token to delete
    ///
    static AftDeleteNodePtr create(const AftNodeToken delToken) {
        return std::make_shared<AftDeleteNode>(delToken);
    }

    ///
    /// @brief Construct a delete object
    ///
    /// @param [in] delToken  Token to delete
    ///
    AftDeleteNode(const AftNodeToken delToken): AftNode(delToken) {};
    ~AftDeleteNode() {};
    virtual const std::string     nodeType() const { return "AftDeleteNode"; };
    virtual       bool            nodeIsDelete() const { return true; };
};

///
/// @class   AftPort
/// @brief   Base class for all port classes in Aft
/// @ingroup AftNodes
///
// These are the basic definitions for Aft port objects. At the moment, the port objects
// come in two flavors - input and output. The port index is used as the basic selector
// for indexing the port in a sandbox object
//
class AftPort: public AftNode {
protected:
    std::string   _portType;  ///< Type of the port
    AftIndex      _portIndex; ///< Index of the port
    std::string   _portName;  ///< Name of the port used by applications
    std::string   _portIfName;    ///< Name of the actual interface

public:
    ///
    /// @brief           Copy constructor for a port
    /// @param [in] node Port node to copy
    ///
    AftPort(const AftPort &node): AftNode(node),
                                      _portType(node.portType()),
                                      _portIndex(node.portIndex()),
                                      _portName(node.portName()),
                                      _portIfName(node.portIfName()) {};

    ///
    /// @brief Construct a port
    ///
    /// @param [in] newPortType  Type of input port (e.g. "Host")
    /// @param [in] newPortIndex Index of input port
    /// @param [in] newPortName  Name of input port (e.g. "port4")
    /// @param [in] newPortNext  Node token value of next node for port
    ///
    AftPort(const std::string  newPortType,
            const AftIndex     newPortIndex,
            const std::string  newPortName,
            const AftNodeToken newPortNext,
            const std::string  newPortIfName="none"): AftNode(),
                                             _portType(newPortType),
                                             _portIndex(newPortIndex),
                                             _portName(newPortName),
                                             _portIfName(newPortIfName) {
        AftNode::setNodeNext(newPortNext);
    };

    ~AftPort() {};

    /// @returns String description of the port type
    const std::string  portType()  const { return _portType; };

    /// @returns String description of the port name
    const std::string  portName()  const { return _portName; };

    /// @returns String description of the interface name
    const std::string  portIfName()  const { return _portIfName; };

    /// @returns Index of port in port table
    AftIndex     portIndex() const { return _portIndex; };

    /// @returns Token of next node for port
    AftNodeToken portNext()  const { return _nodeNext; };

    virtual const std::string nodeType() const { return "AftPort"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " type:" << _portType << " index:" << _portIndex << " name:" << _portName << " ifname:" << _portIfName;
        return os;
    }
};

///
/// @class   AftInputPort
/// @brief   Basic input port class
/// @ingroup AftNodes
///
class AftInputPort: public AftPort {
public:
    typedef std::shared_ptr<AftInputPort> Ptr;  ///< Pointer type for input ports

    using AftPort::AftPort;

    ///
    /// @brief Factory generate an input port
    ///
    /// @param [in] newPort  Reference to port object to copy
    ///
    static AftInputPort::Ptr create(const AftInputPort &newPort) {
        return std::make_shared<AftInputPort>(newPort);
    }

    ///
    /// @brief Factory generate an input port
    ///
    /// @param [in] newPortType  Type of input port (e.g. "Host")
    /// @param [in] newPortIndex Index of input port
    /// @param [in] newPortName  Name of input port (e.g. "port4")
    /// @param [in] newPortNext  Token of next node for input port
    ///
    static AftInputPort::Ptr create(const std::string  newPortType,
                                    const AftIndex     newPortIndex,
                                    const std::string  newPortName,
                                    const AftNodeToken newPortNext,
                                    const std::string  newPortIfName = "none") {
        return std::make_shared<AftInputPort>(newPortType, newPortIndex, newPortName, newPortNext, newPortIfName);
    }

    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return validator->validate(_nodeNext, os);
    }

    virtual const std::string nodeType() const { return "AftInputPort"; };
};

///
/// @class AftOutputPort
/// @brief Basic output port class
/// @ingroup AftNodes
///
class AftOutputPort: public AftPort {
public:
    typedef std::shared_ptr<AftOutputPort> Ptr; ///< Pointer type for output ports

    using AftPort::AftPort;

    ///
    /// @brief Factory generate an input port
    ///
    /// @param [in] newPortType  Type of input port (e.g. "Host")
    /// @param [in] newPortIndex Index of input port
    /// @param [in] newPortName  Name of input port (e.g. "port4")
    /// @param [in] newPortNext  Token of next node for input port
    ///
    static AftOutputPort::Ptr create(const std::string  newPortType,
                                     const AftIndex     newPortIndex,
                                     const std::string  newPortName,
                                     const AftNodeToken newPortNext,
                                     const std::string  newPortIfName = "none") {
        return std::make_shared<AftOutputPort>(newPortType, newPortIndex, newPortName, newPortNext, newPortIfName);
    }

    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (_nodeNext == AFT_NODE_TOKEN_NONE) ? true : validator->validate(_nodeNext, os);
    }

    virtual const std::string nodeType() const { return "AftOutputPort"; };
};

//
// Lists
//
// There are two forms of list in Aft. A simple, non-dynamic list (AftList) and a more complicated, dynamic
// "feature" list (AftIndexedList). If you need a simple, plain list of nodes to execute then it's probably
// better to use the simpler AftList. If you want something more dynamic, then AftIndexedList is appropriate
//
///
/// @class AftList
/// @ingroup AftNodes
/// @brief Simple list class
///
/// @details The basic list node allows a sequence of nodes to be listed by their token values. This allows sequential
/// operations to be defined. In the example below, it's used to insert a counter into a flow of operations.
/// When the list is executed, the counter will be incremented and the execution flow handed to the node
/// referenced by the nextToken value.
///
///

class AftList: public AftNode {
protected:
    AftTokenVector _listNodes; ///< List of node tokens

public:
    typedef std::shared_ptr<AftList> Ptr;  ///< Pointer type for lists

    ///
    /// @brief Factory generate a list
    ///
    /// @param [in] newListNodes Vector of node tokens for list
    ///
    static AftList::Ptr create(const AftTokenVector newListNodes) {
        return std::make_shared<AftList>(newListNodes);
    }

    ///
    /// @brief Copy constructor for a list
    ///
    /// @param [in] node  AftList node to copy
    ///
    AftList(const AftList &node): AftNode(node), _listNodes(node.listNodes()) {};

    ///
    /// @brief Construct a list
    ///
    /// @param [in] newListNodes Vector of node tokens for list
    ///
    AftList(const AftTokenVector newListNodes): AftNode(), _listNodes(newListNodes) {};
    ~AftList() {};

    /// @returns Reference to list of node tokens
    const AftTokenVector &listNodes() const{ return _listNodes; };

    //
    // Virtual Functions
    //
    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_listNodes, os));
    }
    virtual const std::string nodeType() const { return "AftList"; };

    virtual void nextNodes(AftTokenVector &nextTokens) {
        for (AftNodeToken aNodeToken: _listNodes)
            nextTokens.push_back(aNodeToken);
    };

    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " nodes={";
        for (AftNodeToken token: _listNodes) {
            os << " " << token;
        }
        os << "}";
        return os;
    }
};

///
/// @class   AftIndexedList
/// @brief   Indexed list class
/// @ingroup AftNodes
///
class AftIndexedList: public AftNode {
protected:
    AftIndex        _listIndexMax;            ///< Maximum number of entries in indexed list.
    AftEntryVector  _list;                    ///< Aft entry list of token entries

public:
    typedef std::shared_ptr<AftIndexedList> Ptr; ///< Pointer type for lists

    ///
    /// @brief Factory generate an indexed list container
    ///
    /// @param [in] newListIndexMax Maximum number of entries in indexed list
    ///
    static AftIndexedList::Ptr create(const AftIndex newListIndexMax) {
        return std::make_shared<AftIndexedList>(newListIndexMax);
    }

    ///
    /// @brief Factory generate an indexed list container
    ///
    /// @param [in] newListIndexMax Maximum number of entries in indexed list
    /// @param [in] newList         List of token entries
    ///
    static AftIndexedList::Ptr create(const AftIndex   newListIndexMax,
                                      const AftEntryVector &&newList)
    {
        return std::make_shared<AftIndexedList>(newListIndexMax, std::move(newList));
    }

    ///
    /// @brief Construct an indexed list container
    ///
    /// @param [in] newListIndexMax Maximum number of entries in indexed list
    ///
    AftIndexedList(const AftIndex newListIndexMax): _listIndexMax(newListIndexMax) {};

    ///
    /// @brief Construct an indexed list container
    ///
    /// @param [in] newListIndexMax Maximum number of entries in indexed list
    /// @param [in] newList         List of token entries
    ///
    AftIndexedList(const AftIndex newListIndexMax,
                   const AftEntryVector &&newList): _listIndexMax(newListIndexMax),
                                                    _list(std::move(newList))  {};

    /// @returns Returns maximum index of indexed list container
    AftIndex listIndexMax() const { return _listIndexMax; };

    /// @return Return reference to vector of token entries
    const AftEntryVector& list() const { return _list; };

    //
    // Virtual Functions
    //
    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        if (_listIndexMax < _list.size()) {
            return false;
        }
        /* FIXME: AftEntry.h is not included, how to fix this?
        for(auto const &entry: _list) {
            if (!entry->EntryIsValid(validator, os)) {
                return false;
            }
        }*/
        return true;
    }

    //
    // Virtual Functions
    //
    virtual const std::string     nodeType() const { return "AftIndexedList"; };
    virtual       bool            nodeIsContainer() const { return true; };

    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " max:" << _listIndexMax;
        return os;
    }
};

//
// Load Balancing
//
// This is a simple load balancer object. The load balancer is dynamic and uses an AftEntry for each leg of the load
// balancer. The fields to be used to generate the hash index into the table are listed in the _loadFields parameter.
//
///
/// @class   AftLoadBalance
/// @brief   Load balancer class
/// @ingroup AftNodes
///
class AftLoadBalance: public AftNode {
protected:
    AftIndex        _loadIndexMax; ///< Maximum number of entries in load balancer
    AftFieldVector  _loadFields;   ///< Fields used by the load balancer to generate hash

public:
    typedef std::shared_ptr<AftLoadBalance> Ptr; ///< Pointer type for load balancers

    ///
    /// @brief Factory generate a load balancer node
    ///
    /// @param [in] newLoadIndexMax Maximum number of entries in load balancer
    /// @param [in] newLoadFields   Vector of fields to use for load balancer
    ///
    static AftLoadBalance::Ptr create(const AftIndex        newLoadIndexMax,
                                      const AftFieldVector  newLoadFields) {
        return std::make_shared<AftLoadBalance>(newLoadIndexMax, newLoadFields);
    }

    ///
    /// @brief Copy constructor for a load balancer
    ///
    /// @param [in] node  AftLoadBalance node to copy
    ///
    AftLoadBalance(const AftLoadBalance &node): AftNode(node),
                                                _loadIndexMax(node.loadIndexMax()),
                                                _loadFields(node.loadFields()) {};
    ///
    /// @brief Construct a load balancer node
    ///
    /// @param [in] newLoadIndexMax Maximum number of entries in load balancer
    /// @param [in] newLoadFields   Vector of fields to use for load balancer
    ///
    AftLoadBalance(const AftIndex        newLoadIndexMax,
                   const AftFieldVector  newLoadFields): AftNode(),
                                                       _loadIndexMax(newLoadIndexMax),
                                                       _loadFields(newLoadFields) {};
    ~AftLoadBalance() {};

    /// @returns Maximum number of entries in load balancer
    AftIndex       loadIndexMax() const { return _loadIndexMax; };

    /// @returns Vector of fields used for load balancing
    AftFieldVector loadFields()   const { return _loadFields; };

    //
    // Virtual Functions
    //
    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_loadFields, os));
    }
    virtual const std::string     nodeType() const { return "AftLoadBalance"; };
    virtual       bool            nodeIsContainer() const { return true; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " fields={";
        for (AftField field: _loadFields) {
            os << " " << field;
        }
        os << "} max:" << _loadIndexMax;
        return os;
    }
};

//
// Counters And Policers
//
///
/// @class   AftCounter
/// @ingroup AftNodes
/// @brief   The basic counter class
///
/// @details Counters are amongst the most basic (and numerous) of Aft nodes. Any time a counter
/// appears in a node execution graph, it's incremented when that graph is executed. Counters
/// are almost always referenced by their names so it's important to make sure that every
/// counter that's created has a name that you can locate later.
///
///
///
class AftCounter: public AftNode {
protected:
    uint64_t   _initialBytes;     ///< Optional initial byte count of counter
    uint64_t   _initialPackets;   ///< Optional initial packet count of counter
    bool       _l3Mode;           ///< Optional L3 mode of counter (if true, counts L3 bytes)

public:
    typedef std::shared_ptr<AftCounter> Ptr; ///< Pointer type for counters

    ///
    /// @brief Construct a counter
    ///
    /// @param [in] newInitialBytes    Optional initial byte count of counter
    /// @param [in] newInitialPackets  Optional initial byte count of counter
    /// @param [in] newL3Mode          Optional L3 mode of counter (if true, counts L3 bytes)
    ///
    static AftCounter::Ptr create(uint64_t newInitialBytes = 0,
                                  uint64_t newInitialPackets = 0,
                                  bool     newL3Mode = false) {
        return std::make_shared<AftCounter>(newInitialBytes, newInitialPackets, newL3Mode);
    }

    ///
    /// @brief Copy constructor for a counter
    ///
    /// @param [in] node  AftCounter to copy
    ///
    AftCounter(const AftCounter &node): AftNode(node),
                                        _initialBytes(node.initialBytes()),
                                        _initialPackets(node.initialPackets()),
                                        _l3Mode(node.l3Mode()) {};

    ///
    /// @brief Construct a counter
    ///
    /// @param [in] newInitialBytes    Optional initial byte count of counter
    /// @param [in] newInitialPackets  Optional initial byte count of counter
    /// @param [in] newL3Mode          Optional L3 mode of counter (if true, counts L3 bytes)
    ///
    AftCounter(uint64_t newInitialBytes = 0,
               uint64_t newInitialPackets = 0,
               bool     newL3Mode = false): AftNode(),
                                            _initialBytes(newInitialBytes),
                                            _initialPackets(newInitialPackets),
                                            _l3Mode(newL3Mode) {};
    ~AftCounter() {};

    /// @returns Initial byte count of  counter
    uint64_t initialBytes()  const { return _initialBytes; };

    /// @returns Initial packet count of  counter
    uint64_t initialPackets()  const { return _initialPackets; };

    /// @returns L3 mode of counter
    bool l3Mode()  const { return _l3Mode; };

    virtual const std::string nodeType() const { return "AftCounter"; };

    ///
    /// @class Info
    /// @brief Information return class for AftCounter
    ///
    class Info: public AftInfo {
    protected:
        uint64_t _packets;
        uint64_t _bytes;
        uint64_t _packetsRate;
        uint64_t _bytesRate;

    public:
        Info(AftNodeToken newNodeToken, uint64_t newPackets, uint64_t newBytes,
             uint64_t newPacketsRate = 0, uint64_t newBytesRate = 0): AftInfo(newNodeToken),
                                                              _packets(newPackets),
                                                              _bytes(newBytes),
                                                              _packetsRate(newPacketsRate),
                                                              _bytesRate(newBytesRate) {};

        Info(AftNodeToken newNodeToken, uint64_t newPackets, uint64_t newBytes, bool rate = false):
            AftInfo(newNodeToken) {
            if (!rate) {
                _packets = newPackets;
                _bytes   = newBytes;
            } else {
                _packetsRate = newPackets;
                _bytesRate   = newBytes;
            }
        };

        ~Info() {};

        uint64_t packets() const { return _packets; };
        uint64_t bytes()   const { return _bytes; };
        uint64_t packetsRate() const { return _packetsRate; };
        uint64_t bytesRate()   const { return _bytesRate; };

        virtual const std::string infoType() const { return "AftCounter"; };
    };

    virtual Info::UPtr nodeInfo(uint64_t newPackets,
                                uint64_t newBytes,
                                uint64_t newPacketsRate,
                                uint64_t newBytesRate) const {
        return std::unique_ptr<AftCounter::Info>(new AftCounter::Info(_nodeToken,
                                                                      newPackets,
                                                                      newBytes,
                                                                      newPacketsRate,
                                                                      newBytesRate));
    };

    virtual Info::UPtr nodeInfo(uint64_t newPackets,
                                uint64_t newBytes,
                                bool rate = false) const {
        return std::unique_ptr<AftCounter::Info>(new AftCounter::Info(_nodeToken,
                                                                      newPackets,
                                                                      newBytes,
                                                                      rate));
    };
};

///
/// @class   AftPolicer
/// @brief   Simple policer class
/// @ingroup AftNodes
///
class AftPolicer: public AftNode {
protected:
    uint64_t _burstSize;  ///< Burst size of policer
    uint64_t _rate;       ///< Rate of policer
    bool     _packetMode; ///< True if policer is packet-oriented

public:
    typedef std::shared_ptr<AftPolicer> Ptr; ///< Pointer type for policers

    ///
    /// @brief Factory generate a policer
    ///
    /// @param [in] newBurstSize   Burst size of policer
    /// @param [in] newRate        Rate of policer
    /// @param [in] newPacketMode  True if policer is packet-oriented
    ///
    static AftPolicer::Ptr create(const uint64_t     newBurstSize,
                                  const uint64_t     newRate,
                                  const bool         newPacketMode) {
        return std::make_shared<AftPolicer>(newBurstSize, newRate, newPacketMode);
    }

    ///
    /// @brief Copy constructor for a policer
    ///
    /// @param [in] node AftPolicer to copy
    ///
    AftPolicer(const AftPolicer &node): AftNode(node),
                                        _burstSize(node.burstSize()),
                                        _rate(node.rate()),
                                        _packetMode(node.packetMode()) {};
    ///
    /// @brief Construct a policer
    ///
    /// @param [in] newBurstSize   Burst size of policer
    /// @param [in] newRate        Rate of policer
    /// @param [in] newPacketMode  True if policer is packet-oriented
    ///
    AftPolicer(const uint64_t     newBurstSize,
               const uint64_t     newRate,
               const bool         newPacketMode): AftNode(),
                                                  _burstSize(newBurstSize),
                                                  _rate(newRate),
                                                  _packetMode(newPacketMode) {};
    ~AftPolicer() {};

    /// @returns Burst size of policer
    uint64_t burstSize()  const { return _burstSize; };

    /// @returns Rate of policer
    uint64_t rate()       const { return _rate; };

    /// @returns True if policer is packet-oriented
    bool     packetMode() const { return _packetMode; };

    virtual const std::string nodeType() const { return "AftPolicer"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " burstSize:" << _burstSize;
        os << " rate:" << _rate;
        os << " packetMode:" << _packetMode;
        return os;
    }
};

//
// Basic Actions
//
// These do what they say on the tin - if you need to discard or receive a packet, this is how you do it
//
///
/// @class   AftDiscard
/// @brief   Discard class
/// @ingroup AftNodes
///
class AftDiscard: public AftNode {
public:
    typedef std::shared_ptr<AftDiscard> Ptr; ///< Pointer type for discards

    ///
    /// @brief Factory generate a discard
    ///
    static AftDiscard::Ptr create() {
        return std::make_shared<AftDiscard>();
    }

    ///
    /// @brief Copy constructor for a discard
    ///
    /// @param [in] node AftDiscard to copy
    ///
    AftDiscard(const AftDiscard &node): AftNode(node) {};

    ///
    /// @brief Construct a discard
    ///
    AftDiscard(): AftNode() {};
    ~AftDiscard() {};

    virtual const std::string nodeType() const { return "AftDiscard"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        return os;
    }
};

///
/// @class   AftReceive
/// @brief   Host receive class
/// @ingroup AftNodes
///
class AftReceive: public AftNode {
protected:
    uint32_t      _receiveCode; ///< Receive cause identifier
    uint64_t      _context;     ///< Receive context
public:
    typedef std::shared_ptr<AftReceive> Ptr; ///< Pointer type for receive nodes

    ///
    /// @brief Factory generate a receive node
    ///
    /// @param [in] newReceiveCode Identifier for receive node (used to distinguish the receive cause)
    /// @param [in] newContext for receive node (used for additional information)
    ///
    static AftReceive::Ptr create(const uint32_t newReceiveCode, const uint64_t newContext) {
        return std::make_shared<AftReceive>(newReceiveCode, newContext);
    }

    ///
    /// @brief Copy constructor for a receive node
    ///
    /// @param [in] node AftReceive to copy
    ///
    AftReceive(const AftReceive &node): AftNode(node),
                                        _receiveCode(node.receiveCode()),
                                        _context(node.context()) {};

    ///
    /// @brief Construct a receive node
    ///
    /// @param [in] newReceiveCode Identifier for receive node (used to distinguish the receive cause)
    /// @param [in] newContext for receive node (used for additional information)
    ///
    AftReceive(const uint32_t newReceiveCode, const uint64_t newContext): AftNode(), _receiveCode(newReceiveCode), _context(newContext) {};
    ~AftReceive() {};

    /// @returns Receive cause identifier
    uint32_t receiveCode() const { return _receiveCode; };

    /// @returns Receive context
    uint64_t context() const { return _context; };

    virtual const std::string nodeType() const { return "AftReceive"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " code:" << _receiveCode;
        return os;
    }
};

///
/// @class   AftContinue
/// @brief   Implicit continuation operation
/// @ingroup AftNodes
///
class AftContinue: public AftNode {
protected:
public:
    typedef std::shared_ptr<AftContinue> Ptr; ///< Pointer type for continues

    ///
    /// @brief Factory generate a continue node
    ///
    static AftContinue::Ptr create() {
        return std::make_shared<AftContinue>();
    }

    ///
    /// @brief Copy constructor for a continue node
    ///
    /// @param [in] node AftContinue to copy
    ///
    AftContinue(const AftContinue &node): AftNode(node) {};

    ///
    /// @brief Construct a continue node
    ///
    AftContinue(): AftNode() {};
    ~AftContinue() {};

    virtual const std::string nodeType() const { return "AftContinue"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        return os;
    }
};

//
// Conditional Operations
//
// Aft conditionals fall into three groups, closely mapped to their C++ equivalents
//
// if() else - AftMatch
// switch()  - AftSwitch
//
// AftMatch is pretty straightforward. The AftSwitch is a little more complicated. It's
// just like a standard switch() and isn't dynamic - you set all of the case values when the AftSwitch is
// created. Each switch case value is a 64 bit scalar and node token tuple (held in a map). The switch field
// is a field string. This is a straight replacement for a lot of the old multi-way "PDP" logic in something
// a bit higher level
//

///
/// @class   AftMatch
/// @brief   Simple two-way conditional class
/// @ingroup AftNodes
///
class AftMatch: public AftNode {
public:
    ///
    /// @enum OpEnum
    /// @brief   AftMatch operand enum. Used to indicate the comparison to perform in AftMatch
    /// @ingroup AftNodes
    ///
    typedef enum {
        AftMatchOpEQ = 0, ///< if (a == b)
        AftMatchOpNEQ,    ///< if (a != b)
        AftMatchOpLT,     ///< if (a < b)
        AftMatchOpLET,    ///< if (a <= b)
        AftMatchOpGT,     ///< if (a > b)
        AftMatchOpGET     ///< if (a >= b)
    } OpEnum;

protected:
    AftField          _matchField;   ///< Packet field to match against
    AftMatch::OpEnum  _matchOp;      ///< Conditional operation to perform as match
    AftDataPtr        _matchValue;   ///< Value to match against
    uint32_t          _nbits;        ///< Number of bits to match
    AftNodeToken      _trueNode;     ///< Node to execute if match is true
    AftNodeToken      _falseNode;    ///< Node to execute if match is false
    bool              _falsePresent; ///< True if we have a valid falseNode value

public:
    typedef std::shared_ptr<AftMatch> Ptr; ///< Pointer type for matches

    ///
    /// @brief Construct an if() match conditional
    ///
    /// @param [in] newMatchField    Packet field to match against
    /// @param [in] newMatchOp       Conditional operation to perform
    /// @param [in] newMatchValue    Data value to compare field against
    /// @param [in] newTrueNode      Node to execute if match is true
    /// @param [in] newFalseNode     Node to execute if match is false
    /// @param [in] newFalsePresent  Node to execute if match is false
    ///
    static AftMatch::Ptr create(const AftField         newMatchField,
                                const AftMatch::OpEnum newMatchOp,
                                const AftDataPtr       newMatchValue,
                                const uint32_t         newNbits,
                                const AftNodeToken     newTrueNode,
                                const AftNodeToken     newFalseNode,
                                const bool             newFalsePresent) {
        return std::make_shared<AftMatch>(newMatchField,
                                          newMatchOp,
                                          newMatchValue,
                                          newNbits,
                                          newTrueNode,
                                          newFalseNode,
                                          newFalsePresent);
    }

    ///
    /// @brief Copy constructor for a match conditional
    ///
    /// @param [in] node  AftMatch to copy
    ///
    AftMatch(const AftMatch &node): AftNode(node),
                                    _matchField(node.matchField()),
                                    _matchOp(node.matchOp()),
                                    _matchValue(node.matchValue()),
                                    _nbits(node.nbits()),
                                    _trueNode(node.trueNode()),
                                    _falseNode(node.falseNode()),
                                    _falsePresent(node.falsePresent()) {};

    ///
    /// @brief Construct an if()else match conditional
    ///
    /// @param [in] newMatchField Packet field to match against
    /// @param [in] newMatchOp    Conditional operation to perform
    /// @param [in] newMatchValue Data value to compare field against
    /// @param [in] newTrueNode   Node to execute if match is true
    /// @param [in] newFalseNode  Node to execute if match is false
    /// @param [in] newFalsePresent False node is used by the match
    ///
    AftMatch(const AftField         newMatchField,
             const AftMatch::OpEnum newMatchOp,
             const AftDataPtr       newMatchValue,
             const uint32_t         newNbits,
             const AftNodeToken     newTrueNode,
             const AftNodeToken     newFalseNode,
             const bool             newFalsePresent): AftNode(),
                                                      _matchField(newMatchField),
                                                      _matchOp(newMatchOp),
                                                      _matchValue(newMatchValue),
                                                      _nbits(newNbits),
                                                      _trueNode(newTrueNode),
                                                      _falseNode(newFalseNode),
                                                      _falsePresent(newFalsePresent) {};

    ~AftMatch() {};

    /// @returns Packet field to match against
    const AftField        matchField()       const { return _matchField; };

    /// @returns Conditional operation to perform
    AftMatch::OpEnum  matchOp()        const { return _matchOp; };

    /// @returns Data value to compare field against
    AftDataPtr      matchValue()       const { return _matchValue; };

    /// @returns nbits value to compare field against
    uint32_t        nbits()      const { return _nbits; };

    /// @returns Node to execute if match is true
    AftNodeToken    trueNode()               const { return _trueNode; };

    /// @returns Node to execute if match is false
    AftNodeToken    falseNode()              const { return _falseNode; };

    /// @returns True if falseNode is present
    bool            falsePresent()           const { return _falsePresent; };

    /// @param [out] nextTokens    Appends valid next tokens to token vector
    virtual void nextNodes(AftTokenVector &nextTokens) {
        nextTokens.push_back(_trueNode);
        if(_falsePresent){
            nextTokens.push_back(_falseNode);
        }
    };

    /// @ returns true if node is valid
    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        AftTokenVector nodeVec;
        nextNodes(nodeVec);
        return (matchValue()->byteLength() == sizeof(uint32_t) && (validator->validate(nodeVec, os)));
    }

    virtual const std::string nodeType() const { return "AftMatch"; };
};

///
/// @class   AftSwitch
/// @brief   Static multi-part conditional class
/// @ingroup AftNodes
///

class AftSwitch: public AftNode {
public:
    typedef std::map<uint64_t, AftNodeToken> Cases; ///< Map of case values and match tokens

protected:
    AftField         _switchField;       ///< Field to use for switch comparison
    AftNodeToken     _switchDefaultNode; ///< Default node to execute if no match is made
    AftSwitch::Cases _switchCaseValues;  ///< Map of case value/node token tuples
public:
    typedef std::shared_ptr<AftSwitch> Ptr; ///< Pointer type for cases

    ///
    /// @brief Factory generate a switch conditional
    ///
    /// @param [in] newSwitchField       Packet field to switch against
    /// @param [in] newSwitchDefaultNode Default node to execute if no match made
    /// @param [in] newSwitchCaseValues  Map of switch cases
    ///
    static AftSwitch::Ptr create(const AftField          newSwitchField,
                                 const AftNodeToken      newSwitchDefaultNode,
                                 const AftSwitch::Cases  newSwitchCaseValues) {
        return std::make_shared<AftSwitch>(newSwitchField,
                                           newSwitchDefaultNode,
                                           newSwitchCaseValues);
    }

    ///
    /// @brief Copy constructor for a switch
    ///
    /// @param [in] node  AftSwitch to copy
    ///
    AftSwitch(const AftSwitch &node): AftNode(node),
                                      _switchField(node.switchField()),
                                      _switchDefaultNode(node.switchDefaultNode()),
                                      _switchCaseValues(node.switchCaseValues()) {};

    ///
    /// @brief Construct a switch conditional
    ///
    /// @param [in] newSwitchField       Packet field to switch against
    /// @param [in] newSwitchDefaultNode Default node to execute if no match made
    /// @param [in] newSwitchCaseValues  Map of switch cases
    ///
    AftSwitch(const AftField         newSwitchField,
              const AftNodeToken     newSwitchDefaultNode,
              const AftSwitch::Cases newSwitchCaseValues): AftNode(),
                                                           _switchField(newSwitchField),
                                                           _switchDefaultNode(newSwitchDefaultNode),
                                                           _switchCaseValues(newSwitchCaseValues) {};
    ~AftSwitch() {};

    /// @returns Packet field to match against
    const AftField       switchField()       const { return _switchField; };

    /// @returns Default node to execute if no match made
    AftNodeToken   switchDefaultNode() const { return _switchDefaultNode; };

    /// @returns Map of switch cases
    AftSwitch::Cases switchCaseValues()  const { return _switchCaseValues; };

    virtual void nextNodes(AftTokenVector &nextTokens) {
        nextTokens.push_back(_switchDefaultNode);
    };

    virtual const std::string nodeType() const { return "AftSwitch"; };
};

//
// Trees
//
// This is the core tree support
//
///
/// @class   AftTree
/// @brief   Variable-length key lookup tree container class
/// @ingroup AftNodes
///
class AftTree: public AftNode {
protected:
    AftFieldVector _treeFields;      ///< Fields used to build key for tree
    AftNodeToken   _treeDefaultNode; ///< Default node to execute if no match made
    AftProto       _proto;           ///< Proto family

public:
    typedef std::shared_ptr<AftTree> Ptr; ///< Pointer type for trees

    ///
    /// @brief Factory generate a lookup tree
    ///
    /// @param [in] newTreeField         Packet field used as a lookup key
    /// @param [in] newTreeDefaultNode   Default node to execute if no lookup match found
    ///
    static AftTree::Ptr create(const AftField        newTreeField,
                               const AftNodeToken    newTreeDefaultNode) {
        return std::make_shared<AftTree>(newTreeField, newTreeDefaultNode);
    }

    ///
    /// @brief Factory generate a lookup tree
    ///
    /// @param [in] newTreeFields        Vector of packet fields used as a lookup key
    /// @param [in] newTreeDefaultNode   Default node to execute if no lookup match found
    ///
    static AftTree::Ptr create(const AftFieldVector  newTreeFields,
                               const AftNodeToken    newTreeDefaultNode) {
        return std::make_shared<AftTree>(newTreeFields, newTreeDefaultNode);
    }

    ///
    /// @brief Factory generate a lookup tree
    ///
    /// @param [in] newTreeField         Packet field used as a lookup key
    /// @param [in] newTreeDefaultNode   Default node to execute if no lookup match found
    /// @param [in] newProto             Protocol family
    ///
    static AftTree::Ptr create(const AftField        newTreeField,
                               const AftNodeToken    newTreeDefaultNode,
                               const AftProto       &newProto) {
        return std::make_shared<AftTree>(newTreeField, newTreeDefaultNode, newProto);
    }

    ///
    /// @brief Factory generate a lookup tree
    ///
    /// @param [in] newTreeFields        Vector of packet fields used as a lookup key
    /// @param [in] newTreeDefaultNode   Default node to execute if no lookup match found
    /// @param [in] newProto             Protocol family
    ///
    static AftTree::Ptr create(const AftFieldVector  newTreeFields,
                               const AftNodeToken    newTreeDefaultNode,
                               const AftProto       &newProto) {
        return std::make_shared<AftTree>(newTreeFields, newTreeDefaultNode, newProto);
    }

    ///
    /// @brief Copy constructor for a tree
    ///
    /// @param [in] node  AftTree to copy
    ///
    AftTree(const AftTree &node): AftNode(node),
                                  _treeFields(node.treeFields()),
                                  _treeDefaultNode(node.treeDefaultNode()),
                                  _proto(node.proto()) {};

    ///
    /// @brief Construct a lookup tree
    ///
    /// @param [in] newTreeField         Packet field used as a lookup key
    /// @param [in] newTreeDefaultNode   Default node to execute if no lookup match found
    ///
    AftTree(const AftField       newTreeField,
            const AftNodeToken   newTreeDefaultNode): AftNode(), _treeFields( { newTreeField } ),
                                                                 _treeDefaultNode(newTreeDefaultNode),
                                                                 _proto(AftProto(0)) {};

    ///
    /// @brief Construct a lookup tree
    ///
    /// @param [in] newTreeFields        Vector of packet fields used as a lookup key
    /// @param [in] newTreeDefaultNode   Default node to execute if no lookup match found
    ///
    AftTree(const AftFieldVector newTreeFields,
            const AftNodeToken   newTreeDefaultNode): AftNode(),
                                                      _treeFields(newTreeFields),
                                                      _treeDefaultNode(newTreeDefaultNode),
                                                      _proto(AftProto(0)) {};

    ///
    /// @brief Construct a lookup tree
    ///
    /// @param [in] newTreeField         Packet field used as a lookup key
    /// @param [in] newTreeDefaultNode   Default node to execute if no lookup match found
    /// @param [in] newProto             Protocol family
    ///
    AftTree(const AftField       newTreeField,
            const AftNodeToken   newTreeDefaultNode,
            const AftProto      &newProto): AftNode(),
                                            _treeFields( { newTreeField } ),
                                            _treeDefaultNode(newTreeDefaultNode),
                                            _proto(newProto) {};

    ///
    /// @brief Construct a lookup tree
    ///
    /// @param [in] newTreeFields        Vector of packet fields used as a lookup key
    /// @param [in] newTreeDefaultNode   Default node to execute if no lookup match found
    /// @param [in] newProto             Protocol family
    ///
    AftTree(const AftFieldVector newTreeFields,
            const AftNodeToken   newTreeDefaultNode,
            const AftProto      &newProto): AftNode(),
                                            _treeFields(newTreeFields),
                                            _treeDefaultNode(newTreeDefaultNode),
                                            _proto(newProto) {};


    ~AftTree() {};

    /// @returns Reference to vector of packet fields used as a lookup key
    const AftFieldVector &treeFields()      const { return _treeFields; };

    /// @returns Default node to execute if no lookup match found
    AftNodeToken   treeDefaultNode() const { return _treeDefaultNode; };

    /// @returns Reference to protocol family
    const AftProto&   proto() const { return _proto; };

    //
    // Virtual Functions
    //
    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_treeFields, os) &&
                validator->validate(_treeDefaultNode, os));
    }
    virtual const std::string nodeType() const { return "AftTree"; };
    virtual              bool nodeIsContainer() const { return true; };

    virtual void nextNodes(AftTokenVector &nextTokens) {
        nextTokens.push_back(_treeDefaultNode);
    };

    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " fields={";
        for (AftField field: _treeFields) {
            os << " " << field;
        }
        os << "}, default:" << _treeDefaultNode;
        os << ", proto:" << _proto;
        return os;
    }
};

//
// Tables
//
// This is the core scalar table support
//
///
/// @class   AftTable
/// @brief   Index table container class
/// @ingroup AftNodes
///
class AftTable: public AftNode {
protected:
    AftFieldVector _tableFields;      ///< Fields used to build key for table lookup
    AftIndex       _tableMaximum;     ///< Maximum number of entries in table
    AftNodeToken   _tableDefaultNode; ///< Default node to execute if no match in table made

public:
    typedef std::shared_ptr<AftTable> Ptr; ///< Pointer type for tables

    ///
    /// @brief Factory generate an indexed table lookup
    ///
    /// @param [in] newTableField         Packet field used as a lookup index
    /// @param [in] newTableMaximum       Maximum size of lookup index
    /// @param [in] newTableDefaultNode   Default node to execute if no lookup match found
    ///
    static AftTable::Ptr create(const AftField       newTableField,
                                const AftIndex       newTableMaximum,
                                const AftNodeToken   newTableDefaultNode) {
        return std::make_shared<AftTable>(newTableField,
                                          newTableMaximum,
                                          newTableDefaultNode);
    }

    ///
    /// @brief Factory generate an indexed table lookup
    ///
    /// @param [in] newTableFields        Vector of packet fields used as a lookup index
    /// @param [in] newTableMaximum       Maximum size of lookup index
    /// @param [in] newTableDefaultNode   Default node to execute if no lookup match found
    ///
    static AftTable::Ptr create(const AftFieldVector newTableFields,
                                const AftIndex       newTableMaximum,
                                const AftNodeToken   newTableDefaultNode) {
        return std::make_shared<AftTable>(newTableFields,
                                          newTableMaximum,
                                          newTableDefaultNode);
    }

    ///
    /// @brief Copy constructor for a table
    ///
    /// @param [in] node  AftTable to copy
    ///
    AftTable(const AftTable &node): AftNode(node),
                                    _tableFields(node.tableFields()),
                                    _tableMaximum(node.tableMaximum()),
                                    _tableDefaultNode(node.tableDefaultNode()) {};


    ///
    /// @brief Construct an indexed table lookup
    ///
    /// @param [in] newTableField         Packet field used as a lookup index
    /// @param [in] newTableMaximum       Maximum size of lookup index
    /// @param [in] newTableDefaultNode   Default node to execute if no lookup match found
    ///
    AftTable(const AftField       newTableField,
             const AftIndex       newTableMaximum,
             const AftNodeToken   newTableDefaultNode): AftNode(),
                                                        _tableFields( { newTableField } ),
                                                        _tableMaximum(newTableMaximum),
                                                        _tableDefaultNode(newTableDefaultNode) {};

    ///
    /// @brief Construct an indexed table lookup
    ///
    /// @param [in] newTableFields        Vector of packet fields used as a lookup index
    /// @param [in] newTableMaximum       Maximum size of lookup index
    /// @param [in] newTableDefaultNode   Default node to execute if no lookup match found
    ///
    AftTable(const AftFieldVector newTableFields,
             const AftIndex       newTableMaximum,
             const AftNodeToken   newTableDefaultNode): AftNode(),
                                                        _tableFields(newTableFields),
                                                        _tableMaximum(newTableMaximum),
                                                        _tableDefaultNode(newTableDefaultNode) {};

    ~AftTable() {};

    /// @returns Reference to vector of packet fields used as a lookup key
    const AftFieldVector &tableFields()     const { return _tableFields; };

    /// @returns Maximum size of lookup index
    AftIndex       tableMaximum()     const { return _tableMaximum; };

    /// @returns Default node to execute if no lookup match found
    AftNodeToken   tableDefaultNode() const { return _tableDefaultNode; };

    //
    // Virtual Functions
    //
    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_tableFields, os) &&
                validator->validate(_tableDefaultNode, os));
    }
    virtual const std::string nodeType() const { return "AftTable"; };
    virtual              bool nodeIsContainer() const { return true; };

    virtual void nextNodes(AftTokenVector &nextTokens) {
        nextTokens.push_back(_tableDefaultNode);
    };

    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " fields={";
        for (AftField field: _tableFields) {
            os << " " << field;
        }
        os << "}, default:" << _tableDefaultNode;
        return os;
    }
};

///
/// @class   AftLookup
/// @brief   Key lookup class
/// @ingroup AftNodes
///
class AftLookup: public AftNode {
protected:
    AftFieldVector _lookupFields; ///< Fields used to generate lookup key
    AftNodeToken   _lookupNode;   ///< Container node to match against

public:
    typedef std::shared_ptr<AftLookup> Ptr; ///< Pointer type for lookups

    ///
    /// @brief Factory generate a lookup
    ///
    /// @param [in] newLookupField        Packet field used as a lookup index
    /// @param [in] newLookupNode         Token value of container node (e.g. tree, table, etc)
    ///
    static AftLookup::Ptr create(const AftField       newLookupField,
                                 const AftNodeToken   newLookupNode) {
        return std::make_shared<AftLookup>(newLookupField, newLookupNode);
    }

    ///
    /// @brief Factory generate a lookup
    ///
    /// @param [in] newLookupFields       Vector of packet fields used as a lookup index
    /// @param [in] newLookupNode         Token value of container node (e.g. tree, table, etc)
    ///
    static AftLookup::Ptr create(const AftFieldVector newLookupFields,
                                 const AftNodeToken   newLookupNode) {
        return std::make_shared<AftLookup>(newLookupFields, newLookupNode);
    }

    ///
    /// @brief Copy constructor for a lookup
    ///
    /// @param [in] node AftLookup to copy
    ///
    AftLookup(const AftLookup &node): AftNode(node),
                                      _lookupFields(node.lookupFields()),
                                      _lookupNode(node.lookupNode()) {};

    ///
    /// @brief Construct a lookup
    ///
    /// @param [in] newLookupFields       Vector of packet fields used as a lookup index
    /// @param [in] newLookupNode         Token value of container node (e.g. tree, table, etc)
    ///
    AftLookup(const AftFieldVector newLookupFields,
              const AftNodeToken   newLookupNode): AftNode(),
                                                    _lookupFields(newLookupFields),
                                                    _lookupNode(newLookupNode) {};

    ///
    /// @brief Construct a lookup
    ///
    /// @param [in] newLookupField        Packet field used as a lookup index
    /// @param [in] newLookupNode         Token value of container node (e.g. tree, table, etc)
    ///
    AftLookup(const AftField       newLookupField,
              const AftNodeToken   newLookupNode): AftNode(),
                                                    _lookupFields({newLookupField}),
                                                    _lookupNode(newLookupNode) {};
    ~AftLookup() {};

    /// @returns Token value of container node (e.g. tree, table, etc)
    AftNodeToken    lookupNode()   const { return _lookupNode; };

    /// @returns Reference to vector of packet fields used as a lookup index
    const AftFieldVector &lookupFields() const { return _lookupFields; };

    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_lookupFields, os) &&
                validator->validate(_lookupNode, os));
    }
    virtual const std::string nodeType() const { return "AftLookup"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " fields={";
        for (AftField field: _lookupFields) {
            os << " " << field;
        }
        os << "}, lookup:" << _lookupNode;
        return os;
    }
};

///
/// @class   AftIndirect
/// @brief   Indirect reference class
/// @ingroup AftNodes
///
class AftIndirect: public AftNode {
protected:
    bool _hwInstall;                          //< Flag to install on Hardware
public:
    typedef std::shared_ptr<AftIndirect> Ptr; ///< Pointer type for indirects

    ///
    /// @brief Factory generate an indirect node
    ///
    /// @param [in] newIndirectNode       Token value of next node
    ///
     static AftIndirect::Ptr create(const AftNodeToken newIndirectNode, bool newHwInstall = true) {
        return std::make_shared<AftIndirect>(newIndirectNode, newHwInstall);
     }


    ///
    /// @brief Copy constructor for an indirect
    ///
    /// @param [in] node AftIndirect to copy
    ///
    AftIndirect(const AftIndirect &node, bool newHwInstall):
                AftNode(node), _hwInstall(newHwInstall) {};

    ///
    /// @brief Construct an indirect node
    ///
    /// @param [in] newIndirectNode Token value of next node
    ///
    AftIndirect(const AftNodeToken newIndirectNode, bool newHwInstall):
              AftNode(), _hwInstall(newHwInstall)
              { AftNode::setNodeNext(newIndirectNode); };

    ~AftIndirect() {};

    /// @returns Token value of next node
    AftNodeToken indirectNode() const { return _nodeNext; };

    /// @returns hwInstall flag
    AftNodeToken hwInstall() const { return _hwInstall; };

    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_nodeNext, os));
    }
    virtual const std::string nodeType() const { return "AftIndirect"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " indirect:" << _nodeNext << ", hwInstall " << _hwInstall;
        return os;
    }
};

//
// Packets and Metadata
//

///
/// @class   AftDecap
/// @brief   Packet de-encapsulation class
/// @ingroup AftNodes
///
class AftDecap: public AftNode {
protected:
    std::string  _decapName; ///< Name of de-encapsulation template to use

public:
    typedef std::shared_ptr<AftDecap> Ptr; ///< Pointer type for decaps

    ///
    /// @brief Factory generate an de-encapsulation node
    ///
    /// @param [in] newDecapName          Name of de-encapsulation template from decap table to use
    ///
    static AftDecap::Ptr create(const std::string  newDecapName) {
        return std::make_shared<AftDecap>(newDecapName);
    }

    ///
    /// @brief Construct an de-encapsulation node
    ///
    /// @param [in] newDecapName          Name of de-encapsulation template from decap table to use
    ///
    AftDecap(const std::string  newDecapName): AftNode(), _decapName(newDecapName) {};
    ~AftDecap() {};

    /// @returns Name of de-encapsulation template from decap table to use
    const std::string      decapName() const { return _decapName; };

    virtual const std::string nodeType() const { return "AftDecap"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " name=" << _decapName;
        return os;
    }
};

///
/// @class   AftMplsL3Decap
/// @brief   Mpls L3 Packet de-encapsulation class
/// @ingroup AftNodes
///
class AftMplsL3Decap: public AftDecap {
protected:
    bool      _stripFlowLabel;  ///< True if flow label needs to be removed, otherwise false
    bool      _egressStage;     ///< True if egress processing stage, otherwise false

public:
    typedef std::shared_ptr<AftMplsL3Decap> Ptr; ///< Pointer type for Mpls L3 decaps

    ///
    /// @brief Factory generate an Mpls L3 de-encapsulation node
    ///
    /// @param [in] newStripFlowLabel  True if flow label needs to be removed, otherwise false
    /// @param [in] newEgressStage     True if egress processing stage, otherwise false
    /// @param [in] newNextNodeToken   Target token pointed by fabric entry
    ///
    static AftMplsL3Decap::Ptr create(bool newStripFlowLabel, bool newEgressStage,
                                      AftNodeToken newNextNodeToken) {
        return std::make_shared<AftMplsL3Decap>(newStripFlowLabel, newEgressStage,
                                                newNextNodeToken);
    }

    ///
    /// @brief Construct an Mpls L3 de-encapsulation node
    ///
    /// @param [in] newStripFlowLabel  True if flow label needs to be removed, otherwise false
    /// @param [in] newEgressStage     True if egress processing stage, otherwise false
    /// @param [in] newNextNodeToken   Target token pointed by fabric entry
    ///
    AftMplsL3Decap(bool newStripFlowLabel, bool newEgressStage,
                   AftNodeToken newNextNodeToken) : AftDecap("MplsL3Decap"),
                                                    _stripFlowLabel(newStripFlowLabel),
                                                    _egressStage(newEgressStage)
    {
        AftNode::setNodeNext(newNextNodeToken);
    }

    /// @returns return Strip flow label flag
    bool stripFlowLabel () const { return _stripFlowLabel; }

    /// @returns return Egress Processing stage flag
    bool egressStage () const { return _egressStage; }

    /// Node validator
    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_nodeNext, os));
    }

    /// @brief Appends valid next tokens to token vector
    virtual void nextNodes (AftTokenVector &nextTokens) {
        AftNode::nextNodes(nextTokens);
    };

    virtual const std::string nodeType() const { return "AftMplsL3Decap"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftDecap::description(os);
        os << " stripFlowLabel=" << stripFlowLabel() << " egressStage=" << egressStage();
        return os;
    }
};

///
/// @class   AftEncap
/// @brief   Packet encapsulation class
/// @ingroup AftNodes
///
class AftEncap: public AftNode {
protected:
    std::string  _encapName; ///< Name of encapsulation template to use
    AftKeyVector _encapKeys; ///< Vector of key values to use as variables for template

public:
    typedef std::shared_ptr<AftEncap> Ptr; ///< Pointer type for encaps

    ///
    /// @brief Factory generate an de-encapsulation node
    ///
    /// @param [in] newEncapName          Name of encapsulation template from encap table to use
    /// @param [in] newEncapKeys          Vector of encapsulation keys to use to fill template
    ///
    static AftEncap::Ptr create(const std::string  newEncapName,
                                const AftKeyVector newEncapKeys) {
        return std::make_shared<AftEncap>(newEncapName, newEncapKeys);
    }

    ///
    /// @brief Construct an de-encapsulation node
    ///
    /// @param [in] newEncapName          Name of encapsulation template from encap table to use
    /// @param [in] newEncapKeys          Vector of encapsulation keys to use to fill template
    ///
    AftEncap(const std::string  newEncapName,
             const AftKeyVector newEncapKeys): AftNode(),
                                               _encapName(newEncapName),
                                               _encapKeys(newEncapKeys) {};

    ~AftEncap() {};

    /// @returns Name of encapsulation template from encap table to use
    const std::string      encapName() const { return _encapName; };

    /// @returns Reference to vector of encapsulation keys to use to fill template
    const AftKeyVector    &encapKeys() const { return _encapKeys; };

    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_encapKeys, os));
    }

    /// @brief Appends valid next tokens to token vector
    virtual void nextNodes (AftTokenVector &nextTokens) {
        AftNode::nextNodes(nextTokens);
    };

    virtual const std::string nodeType() const { return "AftEncap"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " name=" << _encapName;
        os << " keys={";
        for (AftKey key: _encapKeys) {
            os << " " << key;
        }
        os << "}";
        return os;
    }
};

///
/// @class   AftWrite
/// @brief   Metadata field write class
/// @ingroup AftNodes
///
class AftWrite: public AftNode {
public:
    ///
    /// @enum OpEnum
    /// @brief   AftWrite operand enum. Used to indicate the comparison to perform in AftWrite
    /// @ingroup AftNodes
    ///
    typedef enum {
        AftWriteOpSet = 0,  ///< a = b
        AftWriteOpOr,       ///< a | b
        AftWriteOpAnd,      ///< a & b
        AftWriteOpXor,      ///< a ^ b
    } OpEnum;

protected:
    AftKey           _writeKey; ///< AftKey object describing the metadata field to write
    AftWrite::OpEnum _writeOp;  ///< Operation to perform on write

public:
    typedef std::shared_ptr<AftWrite> Ptr; ///< Pointer type for writes

    ///
    /// @brief Factory generate an metadata write node
    ///
    /// @param [in] newWriteKey    Metadata key to write to
    /// @param [in] newWriteOp     Operation to perform during write
    ///
    static AftWrite::Ptr create(const AftKey           newWriteKey,
                                const AftWrite::OpEnum newWriteOp = AftWriteOpSet) {
        return std::make_shared<AftWrite>(newWriteKey, newWriteOp);
    }

    ///
    /// @brief Construct an metadata write node
    ///
    /// @param [in] newWriteKey           Vector of metadata key to write to
    ///
    AftWrite(const AftKey newWriteKey, const AftWrite::OpEnum newWriteOp): AftNode(),
                                                                           _writeKey(newWriteKey),
                                                                           _writeOp(newWriteOp) {};


    ~AftWrite() {};

    /// @returns Reference the metadata key to write
    const AftKey    &writeKey() const { return _writeKey; };

    /// @returns Write operation
    AftWrite::OpEnum writeOp() const { return _writeOp; };

    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_writeKey, os));
    }
    virtual const std::string nodeType() const { return "AftWrite"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " op="  << _writeOp;
        os << " key=" << _writeKey;
        return os;
    }
};

///
/// @class   AftVariable
/// @brief   Sandbox variable class
/// @ingroup AftNodes
///
class AftVariable: public AftNode {
protected:
    AftKey _variableKey; ///< AftKey object describing the variable

public:
    typedef std::shared_ptr<AftVariable> Ptr; ///< Pointer type for variables

    ///
    /// @brief Factory generate a variable node
    ///
    /// @param [in] newVariableKey        Field and data key that defines variable
    ///
    static AftVariable::Ptr create(const AftKey newVariableKey) {
        return std::make_shared<AftVariable>(newVariableKey);
    }

    ///
    /// @brief Construct a variable node
    ///
    /// @param [in] newVariableKey        Field and data key that defines variable
    ///
    AftVariable(const AftKey       &newVariableKey): AftNode(),
                                                    _variableKey(newVariableKey) {};
    ~AftVariable() {};

    /// @returns Field and data key for variable
    const AftKey &variableKey() const { return _variableKey; };

    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_variableKey, os));
    }
    virtual const std::string nodeType() const { return "AftVariable"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " key=" << _variableKey;
        return os;
    }
};

///
/// @class   AftSandboxLink
/// @brief   Node to allow output ports to jump to another AftSandbox
/// @ingroup AftNodes
///
class AftSandboxLink: public AftNode {
protected:
    std::string _sandboxName;     ///< Name of sandbox to jump to
    AftIndex    _sandboxPortIndex; ///< Index of port to use as input

public:
    typedef std::shared_ptr<AftSandboxLink> Ptr; ///< Pointer type for sandbox links

    ///
    /// @brief Factory generate a sandboxlink node
    ///
    /// @param [in] newSandboxName           Name of sandbox to jump to
    /// @param [in] newSandboxPortIndex      Index of sandbox port to use as input
    ///
    static AftSandboxLink::Ptr create(const std::string  newSandboxName,
                                      const AftIndex     newSandboxPortIndex) {
        return std::make_shared<AftSandboxLink>(newSandboxName, newSandboxPortIndex);
    }

    ///
    /// @brief Construct a variable node
    ///
    /// @param [in] newSandboxName           Name of sandbox to jump to
    /// @param [in] newSandboxPortIndex      Index of sandbox port to use as input
    ///
    AftSandboxLink(const std::string  newSandboxName,
                   const AftIndex     newSandboxPortIndex): AftNode(),
                                                            _sandboxName(newSandboxName),
                                                            _sandboxPortIndex(newSandboxPortIndex) {};
    ~AftSandboxLink() {};

    /// @returns Name of sandbox to jump to
    const std::string sandboxName() const { return _sandboxName; };

    /// @returns Index of port to use as input
    AftIndex sandboxPortIndex() const { return _sandboxPortIndex; };

    virtual const std::string nodeType() const { return "AftSandboxLink"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " sandboxName=" << _sandboxName << " sandboxPortIndex=" << _sandboxPortIndex;
        return os;
    }
};

///
/// @class   AftPlaceholder
/// @brief   Node to act as an interim placeholder
/// @ingroup AftNodes
///
class AftPlaceholder: public AftNode {
public:
    typedef std::shared_ptr<AftPlaceholder> Ptr; ///< Pointer type for placeholder links

    ///
    /// @brief Factory generate a placeholder node
    ///
    static AftPlaceholder::Ptr create() {
        return std::make_shared<AftPlaceholder>();
    }

    ///
    /// @brief Construct a placeholder node
    ///
    AftPlaceholder(): AftNode() {};
    ~AftPlaceholder() {};

    virtual const std::string nodeType() const { return "AftPlaceholder"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        return os;
    }
};

///
/// @class   AftIfPhysical
/// @brief   Base class for all physical interface classes in Aft
/// @ingroup AftNodes
///
class AftIfPhysical : public AftNode
{
public:
    enum AttribConfigType {
        CFG_NONE                        = 0,
        CFG_STREAM_ENABLE               = 1,
        CFG_STREAM_RX_ENABLE            = 2,
        CFG_STREAM_TX_ENABLE            = 3,
        CFG_MAC_ENABLE                  = 4,
        CFG_MAC_RX_ENABLE               = 5,
        CFG_MAC_TX_ENABLE               = 6,
        CFG_MAC_ADDR                    = 7,
        CFG_FLOW_CONTROL                = 8,
        CFG_LOCAL_LPBK                  = 9,
        CFG_REMOTE_LPBK                 = 10
    };

    /// @brief Flex Ethernet type for the IFD
    enum FlexEtherType {
        FLEX_E_NONE,
        FLEX_E_MEMBER_LINK,
        FLEX_E_CLIENT
    };

    /// @brief config state
    enum IfPhysicalCfgState {
        IF_PHYSICAL_CFG_DOWN = 0,
        IF_PHYSICAL_CFG_UP   = 1
    };

    /// @brief Link state
    enum IfPhysicalLinkState {
        IF_PHYSICAL_LINK_DOWN = 0,
        IF_PHYSICAL_LINK_UP   = 1
    };


protected:
    std::string          _name;        ///< Name of the physical interface
    uint32_t             _index;       ///< Index of the physical interface
    uint32_t             _stream;      ///< Stream number of the interface
    uint32_t             _type;        ///< Port type of the physical interface
    uint64_t             _flags;       ///< Flags associated with interface
    uint32_t             _mtu;         ///< MTU of physical interface
    uint32_t             _voq;         ///< VOQ base of physical interface
    AftDataPtr           _mac;         ///< Mac address of physical interface
    uint64_t             _speed;       ///< Speed of physical interface
    uint32_t             _pfeNum;      ///< PFE number on the FPC
    FlexEtherType        _flexEType;   ///< Flex Ethernet type of the interface
    AttribConfigType     _configType;  ///< to indicate changing attribute
    uint64_t             _configValue; ///< new value of attribute to be changed
    IfPhysicalCfgState   _cfgState;    ///< Physical interface config state
    IfPhysicalLinkState  _linkState;   ///< Physical interface link state
    AftNodeToken         _aggPhyToken;  ///< Token of physical interface

public:
    using Ptr = std::shared_ptr<AftIfPhysical>;

    ///
    /// @brief Factory generate an physical interface
    ///
    /// @param [in] newName         Name of the physical interface
    /// @param [in] newIndex        Index of the physical interface
    /// @param [in] newStream       Stream number of the physical interface
    /// @param [in] newType         Port type of the physical interface
    /// @param [in] newFlags        Flag of the physical interface
    /// @param [in] newMtu          MTU of the physical interface
    /// @param [in] newVoq          VOQ base of the physical interface
    /// @param [in] newMac          Mac address of the physical interface
    /// @param [in] newSpeed        Speed of the physical interface
    /// @param [in] newCfgState     Config state of the physical interface
    /// @param [in] newLinkState    Link state of the physical interface
    ///
    static AftIfPhysical::Ptr create(const std::string           newName,
                                     const uint32_t              newIndex,
                                     const uint32_t              newStream,
                                     const uint32_t              newType,
                                     const uint64_t              newFlags,
                                     const uint32_t              newMtu,
                                     const uint32_t              newVoq,
                                     const AftDataPtr            newMac,
                                     const uint64_t              newSpeed,
                                     const IfPhysicalCfgState    newCfgState,
                                     const IfPhysicalLinkState   newLinkState,
                                     const AftNodeToken          newAggPhyToken) {
        return std::make_shared<AftIfPhysical>(newName,
                                               newIndex,
                                               newStream,
                                               newType,
                                               newFlags,
                                               newMtu,
                                               newVoq,
                                               newMac,
                                               newSpeed,
                                               newCfgState,
                                               newLinkState,
                                               newAggPhyToken);
    }

    ///
    /// @brief Construct a physical interface
    ///
    /// @param [in] newName         Name of the physical interface
    /// @param [in] newIndex        Index of the physical interface
    /// @param [in] newStream       Stream number of the physical interface
    /// @param [in] newType         Port type of the physical interface
    /// @param [in] newFlags        Flag of the physical interface
    /// @param [in] newMtu          MTU of the physical interface
    /// @param [in] newVoq          VOQ base of the physical interface
    /// @param [in] newMac          Mac address of the physical interface
    /// @param [in] newSpeed        Speed of the physical interface
    /// @param [in] newCfgState     Config state of the physical interface
    /// @param [in] newLinkState    Link state of the physical interface
    ///
    AftIfPhysical(const std::string             newName,
                  const uint32_t                newIndex,
                  const uint32_t                newStream,
                  const uint32_t                newType,
                  const uint64_t                newFlags,
                  const uint32_t                newMtu,
                  const uint32_t                newVoq,
                  const AftDataPtr              newMac,
                  const uint64_t                newSpeed,
                  const IfPhysicalCfgState      newCfgState,
                  const IfPhysicalLinkState     newLinkState,
                  const AftNodeToken            newAggPhyToken
                  ) :
    AftNode(),
    _name(newName),
    _index(newIndex),
    _stream(newStream),
    _type(newType),
    _flags(newFlags),
    _mtu(newMtu),
    _voq(newVoq),
    _mac(newMac),
    _speed(newSpeed),
    _pfeNum(0),
    _flexEType(FLEX_E_NONE),
    _configType(CFG_NONE),
    _configValue(0),
    _cfgState(newCfgState),
    _linkState(newLinkState),
    _aggPhyToken(newAggPhyToken) {}

    ///
    /// @brief           Copy constructor for a physical interface
    /// @param [in] node Physical interface node to copy
    ///
    AftIfPhysical(const AftIfPhysical& node) :
    AftNode(node),
    _name(node._name),
    _index(node._index),
    _stream(node._stream),
    _type(node._type),
    _flags(node._flags),
    _mtu(node._mtu),
    _voq(node._voq),
    _mac(node._mac),
    _speed(node._speed),
    _pfeNum(0),
    _flexEType(FLEX_E_NONE),
    _configType(CFG_NONE),
    _configValue(0),
    _cfgState(node._cfgState),
    _linkState(node._linkState),
    _aggPhyToken(node._aggPhyToken) {}

    ///
    /// Copy operator, Move constructor and operator
    ///
    AftIfPhysical& operator=(const AftIfPhysical& node) = delete;
    AftIfPhysical(AftIfPhysical&&) = delete;
    AftIfPhysical& operator=(AftIfPhysical&&) = delete;

    ///
    /// @brief Destruct AftIfPhysical node
    ///
    ~AftIfPhysical() {}

    /// @brief returns String name of the physical interface
    const std::string name() const { return _name; }

    /// @brief returns Index of the physical interface
    uint32_t index() const { return _index; }

    /// @brief returns Stream number of the physical interface
    uint32_t stream() const { return _stream; }

    /// @brief returns Port type of the physical interface
    uint32_t type() const { return _type; }

    /// @brief returns Flags associated with the physical interface
    uint64_t flags() { return _flags; }

    /// @brief returns MTU of the physical interface
    uint32_t mtu() const { return _mtu; }

    /// @brief returns Speed of the physical interface
    uint64_t speed() const { return _speed; }

    /// @brief returns Mac address of the physical interface
    AftDataPtr mac() const { return _mac; }

    /// @brief returns VOQ base of the physical interface
    uint32_t voq() const { return _voq; }

    /// @brief returns _pfeNum of the physical interface
    uint32_t pfeNum() const { return _pfeNum; }

    /// @brief returns config state of the physical interface
    IfPhysicalCfgState cfgState() const { return _cfgState; }

    /// @brief returns config state of the physical interface in string
    std::string cfgStateStr() const
    {
        return ((_cfgState == IF_PHYSICAL_CFG_UP)?"config up":"config down");
    }

    /// @brief returns link state of the physical interface
    IfPhysicalLinkState linkState() const { return _linkState; }

    /// @brief returns link state of the physical interface in string
    std::string linkStateStr() const
    {
        return ((_linkState == IF_PHYSICAL_LINK_UP)?"link up":"link down");
    }

    /// @brief set a new value of _pfeNum of the physical interface
    void setPfeNum(uint32_t newNum) { _pfeNum = newNum; }

    /// @brief returns _flexEType of the physical interface
    AftIfPhysical::FlexEtherType flexEType() { return _flexEType; }

    /// @brief set a new value of _FlexEType of the physical interface
    void setFlexEType(AftIfPhysical::FlexEtherType newType)
    {
        _flexEType = newType;
    }

    /// @brief returns _configType of the physical interface
    AftIfPhysical::AttribConfigType configType() { return _configType; }

    /// @brief set a new value of _configType of the physical interface
    void setConfigType(AftIfPhysical::AttribConfigType newType)
    {
        _configType = newType;
    }

    /// @brief returns _configValue of the physical interface
    uint64_t configValue() { return _configValue; }

    /// @brief set a new value of _configValue of the physical interface
    void setConfigValue(uint64_t newValue) { _configValue = newValue; }

    /// @brief returns Index of the logical interface
    AftNodeToken aggPhyToken() const { return _aggPhyToken; }

    //
    // Virtual Functions
    //

    /// @brief returns Printable class type of node
    virtual const std::string nodeType() const { return "AftIfPhysical"; };


    /// @brief operator== overload
    bool operator==(const AftIfPhysical::Ptr &rhs)
    {
        return ((pfeNum()       == rhs->pfeNum()) &&
                (stream()       == rhs->stream()) &&
                (type()         == rhs->type()) &&
                (flags()        == rhs->flags()) &&
                (mtu()          == rhs->mtu()) &&
                (speed()        == rhs->speed()) &&
                (mac()          == rhs->mac()) &&
                (configType()   == rhs->configType()) &&
                (cfgState()     == rhs->cfgState()) &&
                (linkState()    == rhs->linkState()));
    }

    ///
    /// @brief           Define << class operator to append description
    ///                  to an output stream
    /// @param [in] os   Reference to output stream to append to
    /// @param [in] node Reference to node to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os,
                                     const AftIfPhysical::Ptr &ifPhysical)
    {
        return ifPhysical->description(os);
    }


    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);

        os << " interface: "      << _name;
        os << ", pfe: "           << _pfeNum;
        os << ", IFD index: "     << _index;
        os << ", stream: "        << _stream;
        os << ", type: "          << _type;
        os << ", flags: "         << _flags;
        os << ", mtu: "           << _mtu;
        os << ", voq: "           << _voq;
        os << ", speed: "         << _speed;
        os << ", MAC: "           << *_mac;
        os << ", Flex: "          << _flexEType;
        os << ", configType: "    << _configType;
        os << ", configValue: "   << _configValue;
        os << ", cfState: "       << cfgStateStr();
        os << ", linkState: "     << linkStateStr();
        os << ", aggPhyToken: 0x" << std::hex << _aggPhyToken
            << std::dec << std::endl;
        return os;
    }
};


///
/// @class   AftIfLogical
/// @brief   Base class for all logical interface classes in Aft
/// @ingroup AftNodes
///
class AftIfLogical : public AftNode
{
public:
    using Ptr = std::shared_ptr<AftIfLogical>; ///< Pointer type for logical interface

    /// @brief Iterface Logical config state
    enum IfLogicalCfgState {
        IF_LOGICAL_CFG_DOWN  = 0,
        IF_LOGICAL_CFG_UP    = 1
    };

    ///
    /// @brief Factory generate an logical interface
    ///
    /// @param [in] newPhyTkn       Token for parent AftIfPhysical AFT node
    /// @param [in] newIndex        Index of the logical interface
    /// @param [in] newSubunit      Subunit of the logical interface
    /// @param [in] newOuterVlanId  Outer vlan id of the logical interface
    /// @param [in] newInnerVlanId  Inner vlan id of the logical interface
    /// @param [in] newOuterTPID    Outer tag protocol id of the logical interface
    /// @param [in] newInnerTPID    Inner tag protocol id of the logical interface
    /// @param [in[ newVoq          Base VOQ for logical interface
    /// @param [in] newCfgState     logical interface config state
    ///
    static AftIfLogical::Ptr create(const AftNodeToken newPhyTkn,
                                    const uint32_t           newIndex,
                                    const uint32_t           newSubunit,
                                    const uint32_t           newOuterVlanId,
                                    const uint32_t           newInnerVlanId,
                                    const uint32_t           newOuterTPID,
                                    const uint32_t           newInnerTPID,
                                    const uint32_t           newVoq,
                                    const IfLogicalCfgState  newCfgState,
                                    const AftNodeToken       newAggIflToken) {
        return std::make_shared<AftIfLogical>(newPhyTkn,
                                              newIndex,
                                              newSubunit,
                                              newInnerVlanId,
                                              newOuterVlanId,
                                              newOuterTPID,
                                              newInnerTPID,
                                              newVoq,
                                              newCfgState,
                                              newAggIflToken);
    }

    ///
    /// @brief Construct a logical interface
    ///
    /// @param [in] newPhyTkn       Token for parent AftIfPhysical AFT node
    /// @param [in] newIndex        Index of the logical interface
    /// @param [in] newSubunit      Subunit of the logical interface
    /// @param [in] newOuterVlanId  Outer vlan id of the logical interface
    /// @param [in] newInnerVlanId  Inner vlan id of the logical interface
    /// @param [in] newOuterTPID    Outer tag protocol id of the logical interface
    /// @param [in] newInnerTPID    Inner tag protocol id of the logical interface
    /// @param [in[ newVoq          Base VOQ for logical interface
    /// @param [in] newCfgState     logical interface config state
    ///
    AftIfLogical(const AftNodeToken         newPhyTkn,
                 const uint32_t             newIndex,
                 const uint32_t             newSubunit,
                 const uint32_t             newOuterVlanId,
                 const uint32_t             newInnerVlanId,
                 const uint32_t             newOuterTPID,
                 const uint32_t             newInnerTPID,
                 const uint32_t             newVoq,
                 const IfLogicalCfgState    newCfgState,
                 const AftNodeToken         newAggIflToken) :
    AftNode(),
    _ifPhysicalToken(newPhyTkn),
    _index(newIndex),
    _subunit(newSubunit),
    _outerVlanId(newOuterVlanId),
    _innerVlanId(newInnerVlanId),
    _outerTPID(newOuterTPID),
    _innerTPID(newInnerTPID),
    _voq(newVoq),
    _cfgState(newCfgState),
    _aggIflToken(newAggIflToken) { }

    ///
    /// @brief           Copy constructor for a logical interface
    /// @param [in] node Logical interface node to copy
    ///
    AftIfLogical(const AftIfLogical& node) :
    AftNode(node),
    _ifPhysicalToken(node._ifPhysicalToken),
    _index(node._index),
    _subunit(node._subunit),
    _outerVlanId(node._outerVlanId),
    _innerVlanId(node._innerVlanId),
    _outerTPID(node._outerTPID),
    _innerTPID(node._innerTPID),
    _voq(node._voq),
    _cfgState(node._cfgState),
    _aggIflToken(node._aggIflToken) { }

    ///
    /// Copy operator, Move constructor and operator
    ///
    AftIfLogical& operator=(const AftIfLogical&) = delete;
    AftIfLogical(AftIfLogical&&) = delete;
    AftIfLogical& operator=(AftIfLogical&&) = delete;

    //
    /// @brief Destruct AftIfLogical node
    //
    ~AftIfLogical() {}

    /// @brief returns Index of the logical interface
    AftNodeToken ifPhysicalToken() const { return _ifPhysicalToken; }

    /// @brief returns Index of the logical interface
    uint32_t index() const { return _index; }

    /// @brief returns Subunit of the logical interface
    uint32_t subunit() const { return _subunit; }

    /// @brief returns Outer vlan id of the logical interface
    uint32_t outerVlanId() const { return _outerVlanId; }

    /// @brief returns Inner vlan id of the logical interface
    uint32_t innerVlanId() const { return _innerVlanId; }

    /// @brief returns Outer tag protocol id of the logical interface
    uint32_t outerTPID() const { return _outerTPID; }

    /// @brief returns Inner tag protocol id of the logical interface
    uint32_t innerTPID() const { return _innerTPID; }

    /// @brief returns base VOQ of the logical interface
    uint32_t voq() const { return _voq; }

    /// @brief returns config state of the logical interface
    IfLogicalCfgState cfgState() const { return _cfgState; }

    /// @brief returns Aggregate If Logical Node Token
    AftNodeToken aggIflToken() const { return _aggIflToken; }

    /// @brief returns config state of the logical interface in string
    std::string cfgStateStr() const
    {
        return ((_cfgState == IF_LOGICAL_CFG_UP)?"config up":"config down");
    }

    //
    // Virtual Functions
    //
    virtual const std::string nodeType() const { return "AftIfLogical"; }

    virtual bool nodeIsContainer() const { return true; }

    virtual void nextNodes(AftTokenVector &nextTokens)
    {
        if (AftTokenIsSet(_ifPhysicalToken)) {
            nextTokens.push_back(_ifPhysicalToken);
        }
    }

    /// @brief operator== overload
    bool operator==(const AftIfLogical::Ptr &rhs)
    {
        return ((subunit()      == rhs->subunit()) &&
                (outerVlanId()  == rhs->outerVlanId()) &&
                (innerVlanId()  == rhs->innerVlanId()) &&
                (outerTPID()    == rhs->outerTPID()) &&
                (innerTPID()    == rhs->innerTPID()) &&
                (cfgState()     == rhs->cfgState()));
    }

    ///
    /// @brief           Define << class operator to append description to
    ///                  an output stream
    /// @param [in] os   Reference to output stream to append to
    /// @param [in] node Reference to node to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os,
                                     const AftIfLogical::Ptr &ifLogical)
    {
        return ifLogical->description(os);
    }

    //
    /// @brief returns Printable class type of node
    //
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << ", IFL index: "      << _index;
        os << ", subunit: "        << _subunit << std::hex;
        os << ", ovlan: 0x"        << _outerVlanId;
        os << ", ivlan: 0x"        << _innerVlanId;
        os << ", otpid: 0x"        << _outerTPID;
        os << ", itpid: 0x"        << _innerTPID << std::dec;
        os << ", voq: "            << _voq;
        os << ", cfgState: "       << cfgStateStr();
        os << ", IfPhyToken: 0x"   << std::hex << _ifPhysicalToken;
        os << ", aggIflToken: 0x " << _aggIflToken << std::dec << std::endl;

        return os;
    }

protected:
    AftNodeToken          _ifPhysicalToken; ///< Token of physical interface
    uint32_t              _index;           ///< Index of the logical interface
    uint32_t              _subunit;         ///< Logical interface subunit
    uint32_t              _outerVlanId;     ///< Outer vlan id
    uint32_t              _innerVlanId;     ///< Inner vlan id
    uint32_t              _outerTPID;       ///< Outer tag protocol id
    uint32_t              _innerTPID;       ///< Inner tag protocol id
    uint32_t              _voq;             ///< VOQ base of logical interface
    IfLogicalCfgState     _cfgState;        ///< Logical interface config state
    AftNodeToken          _aggIflToken;     ///< Token of physical interface
};

///
/// @brief Express logical interface protocol family related node
///
class AftIfLogicalFamily : public AftNode
{
protected:
    uint32_t              _rttIdx;  ///< Rtt Index
    AftProto              _proto;   ///< Proto family
    AftDataPtr            _mac;     ///< Mac address
    uint32_t              _mtu;     ///< Logical family MTU

public:
    using Ptr = std::shared_ptr<AftIfLogicalFamily>;

    ///
    /// @brief Factory generate an logical interface
    ///
    /// @param [in] newRttIdx       Rtt Index
    /// @param [in] newProto        Protocol family
    /// @param [in] newMac          Interface MAC address
    /// @param [in] newMtu          MTU associated with protocol family
    ///
    static AftIfLogicalFamily::Ptr create(const uint32_t   newRttIdx,
                                          const AftProto   newProto,
                                          const AftDataPtr newMac,
                                          const uint32_t   newMtu) {
        return std::make_shared<AftIfLogicalFamily>(newRttIdx,
                                                    newProto,
                                                    newMac,
                                                    newMtu);
    }

    ///
    /// @brief Construct a logical interface family
    ///
    /// @param [in] newRttIdx       Rtt Index
    /// @param [in] newProto        Protocol family
    /// @param [in] newMac          Interface MAC address
    /// @param [in] newMtu          MTU associated with protocol family
    ///
    AftIfLogicalFamily(const uint32_t    newRttIdx,
                       const AftProto    newProto,
                       const AftDataPtr  newMac,
                       const uint32_t    newMtu) :
    AftNode(),
    _rttIdx(newRttIdx),
    _proto(newProto),
    _mac(newMac),
    _mtu(newMtu) {}

    ///
    /// @brief           Copy constructor for a logical interface family
    /// @param [in] node Logical interface family node to copy
    ///
    AftIfLogicalFamily(const AftIfLogicalFamily& node): AftNode(node),
    _rttIdx(node._rttIdx),
    _proto(node._proto),
    _mac(node._mac),
    _mtu(node._mtu) {}

    ///
    /// @brief Destruct AftIfLogicalFamily node
    ///
    ~AftIfLogicalFamily() {}

    ///
    /// Copy operator, Move constructor and operator
    ///
    AftIfLogicalFamily& operator=(AftIfLogicalFamily&) = delete;
    AftIfLogicalFamily(AftIfLogicalFamily&&) = delete;
    AftIfLogicalFamily& operator=(AftIfLogicalFamily&&) = delete;

    /// @brief returns brief returns rtt index
    uint32_t rttIdx() const { return _rttIdx; }

    /// @brief returns protocol family
    AftProto proto() const { return _proto; }

    /// @brief returns interface mac address
    AftDataPtr mac() const { return _mac; }

    /// @brief returns MTU associated with protocol family
    uint32_t mtu() const { return _mtu; }

    //
    // Virtual Functions
    //

    /// @brief returns Printable class type of node
    virtual const std::string nodeType() const { return "AftIfLogicalFamily"; };

    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << ", rttIndex: " << _rttIdx;
        os << ", proto: "    << _proto;
        os << ", mtu: "      << _mtu;
        os << ", mac: "      << *_mac;
        os << ", IfLogicalToken: 0x" << std::hex << nodeNext()
            << std::dec << std::endl;
        return os;
    }
};

//
// Sampler Entry
//
///
/// @class   AftSample
/// @ingroup AftNodes
/// @brief   The sampler class
///
/// @details Sampler node is used to create/update/delete sampling feature
///
///
///
class AftSample: public AftNode {
public:
    typedef enum {
        AftSampleClassPFE  = 0,    ///
        AftSampleClassSample,      /// jflow sampling class
        AftSampleClassPortMirror,  /// port mirroring
        AftSampleClassSyslog,      ///
        AftSampleClassFlowTap      /// flow tap
    } SampleClassType;

    typedef enum {
        AftSampleProtoIPv4 = 0,  /// IPv4
        AftSampleProtoIPv6,      /// IPv6
        AftSampleProtoAny,       /// Any Family
        AftSampleProtoTag,       /// MPLS
        AftSampleProtoVPLS,      /// VPLS
        AftSampleProtoCCC,       /// CCC
    } SampleProtoType;

protected:
    std::string _sampleInstName;                  ///< Name of sampling instance
    AftSample::SampleClassType  _sampleClass;     ///< sampling class
    AftSample::SampleProtoType  _sampleProto;     ///< protocol family
    AftNodeToken                _sampleNhToken;   ///< Token of nexthop on which sampled/mirrored pakct hits
    uint32_t   _sampleRate;      ///< sampling ratio
    uint32_t   _sampleRunLength; ///< Running Length
    uint32_t   _sampleClipSize;  ///< Clip Size
    bool       _samplePktCap;    ///< Packet Capture flag
    bool       _sampleInline;    ///< No of samples after initial trigger

public:
    typedef std::shared_ptr<AftSample> Ptr; ///< Pointer type for Sampler node

    ///
    /// @brief Construct a sampler entry
    ///
    /// @param [in] newSampleInstName   user defined sampling instance name
    /// @param [in] newSampleClass      sampling class
    /// @param [in] newSampleProto      sampling protocol
    /// @param [in] newSampleNhToken    node token for nexthop id
    /// @param [in] newSampleRate       rate of sampling
    /// @param [in] newSampleRunLength  number of sample after initial trigger
    /// @param [in] newSampleClipSize   maximum length of sampled packet
    /// @param [in] newSamplePktCap     packet capture status
    /// @param [in] newSampleInline     indicates inline sampling
    ///
    static AftSample::Ptr create(std::string newSampleInstName,
                                 AftSample::SampleClassType newSampleClass,
                                 AftSample::SampleProtoType newSampleProto,
				 AftNodeToken newSampleNhToken = AFT_NODE_TOKEN_DISCARD,
				 uint32_t newSampleRate = 1,
				 uint32_t newSampleRunLength = 0,
				 uint32_t newSampleClipSize = 0,
				 bool     newSamplePktCap = false,
				 bool     newSampleInline = false) {
        return std::make_shared<AftSample>(newSampleInstName, newSampleClass, newSampleProto,
	                                   newSampleNhToken, newSampleRate, newSampleRunLength,
					   newSampleClipSize, newSamplePktCap, newSampleInline);
    }

    ///
    /// @brief Copy constructor
    ///
    /// @param [in] node  AftSample to copy
    ///
    AftSample(const AftSample &node): AftNode(node),
                               _sampleInstName(node.sampleInstName()),
                               _sampleClass(node.sampleClass()),
                               _sampleProto(node.sampleProto()),
			       _sampleNhToken(node.sampleNhToken()),
                               _sampleRate(node.sampleRate()),
                               _sampleRunLength(node.sampleRunLength()),
                               _sampleClipSize(node.sampleClipSize()),
                               _samplePktCap(node.samplePktCap()),
                               _sampleInline(node.sampleInline()) {};

    ///
    /// @brief Construct a sampler
    ///
    /// @param [in] newSampleInstName   user defined sampling instance name
    /// @param [in] newSampleClass      sampling class
    /// @param [in] newSampleProto      sampling protocol
    /// @param [in] newSampleNhToken    node token for nexthop id
    /// @param [in] newSampleRate       rate of sampling
    /// @param [in] newSampleRunLength  number of sample after initial trigger
    /// @param [in] newSampleClipSize   maximum length of sampled packet
    /// @param [in] newSamplePktCap     status of packet status
    /// @param [in] newSampleInline     indicates inline sampling
    ///
    AftSample(std::string newSampleInstName,
              AftSample::SampleClassType newSampleClass,
              AftSample::SampleProtoType newSampleProto,
	      AftNodeToken newSampleNhToken = AFT_NODE_TOKEN_DISCARD,
	      uint32_t newSampleRate = 1,
	      uint32_t newSampleRunLength = 0,
	      uint32_t newSampleClipSize = 0,
	      bool     newSamplePktCap = false,
	      bool     newSampleInline = false) :
	      AftNode(),
	      _sampleInstName(newSampleInstName),
              _sampleClass(newSampleClass),
              _sampleProto(newSampleProto),
              _sampleNhToken(newSampleNhToken),
	      _sampleRate(newSampleRate),
	      _sampleRunLength(newSampleRunLength),
	      _sampleClipSize(newSampleClipSize),
	      _samplePktCap(newSamplePktCap),
	      _sampleInline(newSampleInline) {}

    ~AftSample() {};

    /// @brief returns sampling instance name
    std::string sampleInstName()  const { return _sampleInstName; };

    /// @brief returns sampling class
    AftSample::SampleClassType sampleClass()  const { return _sampleClass; };

    /// @brief returns sampling protocol
    AftSample::SampleProtoType sampleProto()  const { return _sampleProto; };

    /// @brief returns token for nexthop id
    AftNodeToken sampleNhToken()  const { return _sampleNhToken; };

    /// @brief returns rate of sampling
    uint32_t sampleRate()  const { return _sampleRate; };

    /// @brief returns run length
    uint32_t sampleRunLength()  const { return _sampleRunLength; };

    /// @brief returns clip size
    uint32_t sampleClipSize()  const { return _sampleClipSize; };


    /// @brief returns sampling packet capture flag
    bool samplePktCap()  const { return _samplePktCap; };

    /// @brief returns sampling inline flag
    bool sampleInline()  const { return _sampleInline; };

    void nextNodes (AftTokenVector &nextTokens) {
        if (AftTokenIsSet(_sampleNhToken)) nextTokens.push_back(_sampleNhToken);
    };

    virtual const std::string nodeType() const { return "AftSample"; };

};

///
/// @class   AftFabricInput
/// @brief   Node to add egress start token to fabric input table(Nexthop token table)
/// @ingroup AftNodes
///
class AftFabricInput: public AftNode {
protected:
    uint32_t      _fabricTableIndex;  ///< Index to fabric table

public:
    typedef std::shared_ptr<AftFabricInput> Ptr; ///< Pointer type for AftFabricInput

    ///
    /// @brief Factory generate a AftFabricInput node
    ///
    /// @param [in] newFabricTableIndex  Fabric table entry index
    /// @param [in] newNextNodeToken     Target token pointed by fabric entry
    ///
    static AftFabricInput::Ptr create(uint32_t     newFabricTableIndex,
                                      AftNodeToken newNextNodeToken) {
        return std::make_shared<AftFabricInput>(newFabricTableIndex, newNextNodeToken);
    }

    ///
    /// @brief Construct a AftFabricInput node
    ///
    /// @param [in] newFabricTableIndex  Nexthop/flabel index
    /// @param [in] newNextNodeToken     Target token pointed by fabric entry
    ///
    AftFabricInput(uint32_t     newFabricTableIndex,
                   AftNodeToken newNextNodeToken): _fabricTableIndex(newFabricTableIndex)
    {
        AftNode::setNodeNext(newNextNodeToken);
    }

    /// @returns Fabric table index
    uint32_t  fabricTableIndex() const { return _fabricTableIndex; };

    /// @returns Token pointed by fabric entry
    AftNodeToken nextNodeToken() const { return AftNode::nodeNext(); };

    /// Node validator
    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(_nodeNext, os));
    }

    /// @brief Appends valid next tokens to token vector
    virtual void nextNodes (AftTokenVector &nextTokens) {
        AftNode::nextNodes(nextTokens);
    };

    virtual const std::string nodeType() const { return "AftFabricInput"; };
    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " fabricTableIndex=" << fabricTableIndex();
        return os;
    }
};

///
/// @brief Selector class
///
class AftSelector : public AftNode {
public:

    ///
    /// @brief Selector created is for  Unicast aggregation or mulicast aggregation
    ///
    enum class AftSelectorAggType { UNICAST,  MULTICAST, NO_REDISTRIBUTE,
                                    COMPOSITE };

    ///
    /// @brief selector type for selectors.
    ///
    enum class AftSelectorType { UNKNOWN, COMPACT, REGULAR, EXTENDED,
                                 STATEFUL, ADAPTIVE, MAX_TYPE };

    ///
    /// @brief selector subtype for non-stateful selectors.
    ///
    enum class AftSelectorSubType {
        /// standard default use
        UNKNOWN,
        /// for consistent hash use
        CONS_HASH,
        /// for isis bbm group use
        SRLG,
        /// Multicast selector for ALB,SLB
        MULTICAST
    };

    //
    // @brief Selector flags for Unilist
    //
    enum AftSelectorFlags {
        ///
        /// Misc Flags
        ///
        ADAPTIVE                    = 0x01,
        SEL_INVALID_IS_NON_DEFAULT  = 0x02,
        ///
        /// Is fast failover supported ?
        ///
        FAST_REROUTE                = 0x04,
        ///
        /// Any Bandwidth distribution that should be done between
        /// all active paths.
        ///
        BALANCES_PRESENT            = 0x08,
        ///
        /// Used to qualify compact selector mode.
        /// This flag when set to TRUE indicates all NH under it has unequal
        /// weights and it can act as primary/backup/protected etc...
        ///
        ALL_WEIGHTS_UNEQUAL         = 0x10,

        WEIGHTED                    = 0x20,
        CASCADED                    = 0x40,
        RESILIENT                   = 0x80,
    };

    ///
    ///@brief User configured parameters
    ///
    struct AftSelectorParams {
        ///
        /// Adaptive LB parameters.
        ///
        bool     p_tolerance_valid;
        uint32_t p_tolerance;
        uint8_t  p_pps_bps_sel;
        uint32_t p_scan_interval;

        ///
        /// User configured local-bias percentage
        ///
        uint8_t  k_local_bias_percentage;
    public:
        ///
        ///@brief Default constructor
        ///
        AftSelectorParams()
        {
            p_tolerance_valid = true;
            p_tolerance = p_pps_bps_sel = p_scan_interval = 0;
            k_local_bias_percentage = 0;
        }

        ///
        /// @brief Copy constructor
        ///
        AftSelectorParams(const AftSelectorParams &params)
                    : p_tolerance_valid(params.p_tolerance_valid),
                      p_tolerance(params.p_tolerance),
                      p_pps_bps_sel(params.p_pps_bps_sel),
                      p_scan_interval(params.p_scan_interval),
                      k_local_bias_percentage(params.k_local_bias_percentage) {}

        ///
        /// @brief           description of the Selector object
        /// @param [in] os   Reference to output stream to append to
        ///
        std::ostream & description (std::ostream &os) const {
            os << " selector-params={";
            os << "tolerance: " << p_tolerance;
            if (p_pps_bps_sel) {
                os << ", pps";
            } else {
                os << ", bps";
            }
            os << ", scan interval: " << p_scan_interval;
            os << ", local-bias percent: " << k_local_bias_percentage;
            os << "}";
            return os;
        }

        ///
        /// @brief           Define << class operator to append description to an output stream
        /// @param [in] os   Reference to output stream to append to
        /// @param [in] node Reference to node to append description of to stream
        ///
        friend std::ostream &operator<< (std::ostream &os, const AftSelectorParams &params)
        {
            return params.description(os);
        }
    };

    ///
    /// @brief SelectorKey base class
    ///
    struct AftSelectorKey {
    public:
        ///
        /// Selector and sub-selector types
        ///
        AftSelectorType     selectorType;
        AftSelectorSubType  selectorSubType;

        ///
        /// Use of this selector is unicast-AE(0) or multicast-AE(1).
        ///
        AftSelectorAggType  selectorAggType;

        uint32_t selectorFlags;

        ///
        /// The actual size of the key.
        ///
        uint32_t selectorSize;

        ///
        /// Bitmask to reflect state of nexthop
        ///
        uint64_t inact_nh_bitmask;

        ///
        /// k_target_id identifies a unique selector used by a group of unilists.
        /// This unique selector's original weights can be independently modified
        /// (i.e., targeted weights).  target_id is set to 0 otherwise.
        ///
        uint32_t k_target_id;

        ///
        /// Parameters for Adaptive LB
        ///
        AftSelectorParams selectorParams;

        ///
        /// All the long arrays
        ///
        AftTokenVector         selectorNodes;   ///< Array of active state nodes
        vector<uint16_t>       selectorWeights; ///< Load balancing weights
        vector<uint16_t>       selectorBalances;///< Load balancing balances
        AftTokenVector         selectorSessions;///< Selector session-ids

        ///
        ///@brief constructor
        ///
        AftSelectorKey(const AftTokenVector       &nodes = AftTokenVector(),
                       const vector<uint16_t>     &weights = vector<uint16_t>(),
                       const vector<uint16_t>     &balances = vector<uint16_t>(),
                       const AftTokenVector       &sessions = AftTokenVector(),
                       const AftSelectorType      type = AftSelectorType::REGULAR,
                       const AftSelectorSubType   subtype = AftSelectorSubType::UNKNOWN,
                       const AftSelectorAggType   aggtype = AftSelectorAggType::UNICAST,
                       const uint32_t             flags = AftSelectorFlags::FAST_REROUTE,
                       const AftSelectorParams    &params = AftSelectorParams(),
                       const uint64_t             bitmask = 0,
                       const uint32_t             target_id = 0)
            : selectorType(type), selectorSubType(subtype),
               selectorAggType(aggtype), selectorFlags(flags),
               inact_nh_bitmask(bitmask), k_target_id(target_id),
               selectorParams(params), selectorNodes(nodes),
               selectorWeights(weights), selectorBalances(balances),
               selectorSessions(sessions)
        {
            selectorSize = selectorNodes.size() + selectorWeights.size()
                            + selectorBalances.size() + selectorSessions.size();
        }

        ///
        /// @brief Copy constructor
        ///
        AftSelectorKey(const AftSelectorKey &key) = default;

        ///
        ///@brief move constructor
        ///
        //AftSelectorKey(AftSelectorKey &&) = default;

        ///
        ///@brief move assignment operator
        ///
        //AftSelectorKey &operator=(AftSelectorKey &&) = default;

        void  set_flags(AftSelectorFlags flags) { selectorFlags |= flags; }

        //
        // equal_to operation for key comparison
        //
        bool operator == (const AftSelectorKey &rhs) const
        {
            return ((selectorType == rhs.selectorType) &&
                    (selectorSubType == rhs.selectorSubType) &&
                    (selectorAggType == rhs.selectorAggType) &&
                    (selectorFlags == rhs.selectorFlags) &&
                    (selectorSize == rhs.selectorSize) &&
                    (k_target_id == rhs.k_target_id) &&
                    (inact_nh_bitmask == rhs.inact_nh_bitmask) &&
                    (selectorParams.p_tolerance_valid == rhs.selectorParams.p_tolerance_valid) &&
                    (selectorParams.p_tolerance == rhs.selectorParams.p_tolerance) &&
                    (selectorParams.p_pps_bps_sel == rhs.selectorParams.p_pps_bps_sel) &&
                    (selectorParams.p_scan_interval == rhs.selectorParams.p_scan_interval) &&
                    (selectorParams.k_local_bias_percentage == rhs.selectorParams.k_local_bias_percentage) &&
                    (selectorNodes == rhs.selectorNodes) &&
                    (selectorWeights == rhs.selectorWeights) &&
                    (selectorBalances == rhs.selectorBalances) &&
                    (selectorSessions == rhs.selectorSessions));
        }
    };

    using Ptr = std::shared_ptr<AftSelector>; ///< Pointer for Selector nodes

    ///
    /// @brief Factory generate a Selector node
    /// @param [in] selectorNodes    ///< Array of active state nodes
    /// @param [in] selectorWeights  ///< Selector weight
    /// @param [in] balances         ///< Balances associated with Sel members
    /// @param [in] sessions         ///< Array of active session nodes
    /// @param [in] params           ///< Selector params
    /// @param [in] type             ///< Selector type
    /// @param [in] subtype          ///< Selector subtype
    /// @param [in] aggtype          ///< Selector aggtype
    /// @param [in] flags            ///< Selector flags
    /// @param [in] bitmask          ///< bitmask
    /// @param [in] target_id        ///< target_id
    ///
    static AftSelector::Ptr create(
              const AftTokenVector      &selectorNodes = AftTokenVector(),
              const vector<uint16_t>    &selectorWeights = std::vector<uint16_t>(),
              const vector<uint16_t>    &balances = std::vector<uint16_t>(),
              const AftTokenVector      &sessions = AftTokenVector(),
              const AftSelectorType     type = AftSelectorType::REGULAR,
              const AftSelectorSubType  subtype = AftSelectorSubType::UNKNOWN,
              const AftSelectorAggType  aggtype = AftSelectorAggType::UNICAST,
              const uint32_t            flags = AftSelectorFlags::FAST_REROUTE,
              const AftSelectorParams   &params = AftSelectorParams(),
              const uint64_t            bitmask         = 0,
              const uint32_t            target_id        = 0)
    {
        return std::make_shared<AftSelector>(selectorNodes, selectorWeights,
                                             balances, sessions, type, subtype,
                                             aggtype, flags, params, bitmask,
                                             target_id);
    }

    static AftSelector::Ptr create(const AftSelectorKey &key)
    {
        return std::make_shared<AftSelector>(key);
    }

    ///
    /// @brief Copy constructor for AftSelector
    /// @param [in] node    AftSelector node to copy
    ///
    AftSelector(const AftSelector &node): AftNode(node),
                                          _selectorKey(node._selectorKey) {}

    ///
    /// @brief Construct AftSelector node
    /// @param [in] actNodes;        ///< Array of active state nodes
    /// @param [in] wts              ///< Weights associated with Sel members
    /// @param [in] balances         ///< Balances associated with Sel members
    /// @param [in] sessions         ///< Array of active session nodes
    /// @param [in] params           ///< Selector params
    /// @param [in] type             ///< Selector type
    /// @param [in] subtype          ///< Selector subtype
    /// @param [in] aggtype          ///< Selector aggtype
    /// @param [in] flags            ///< Selector flags
    /// @param [in] bitmask          ///< bitmask
    /// @param [in] target_id        ///< target_id
    ///
    AftSelector(const AftTokenVector    &sel_nodes = AftTokenVector(),
                const vector<uint16_t>  &sel_weights = std::vector<uint16_t>(),
                const vector<uint16_t>   &sel_balances = std::vector<uint16_t>(),
                const AftTokenVector     &sel_sessions = AftTokenVector(),
                const AftSelectorType    sel_type = AftSelectorType::REGULAR,
                const AftSelectorSubType sel_subtype = AftSelectorSubType::UNKNOWN,
                const AftSelectorAggType sel_aggtype = AftSelectorAggType::UNICAST,
                const uint32_t           sel_flags = AftSelectorFlags::FAST_REROUTE,
                const AftSelectorParams  &params = AftSelectorParams(),
                const uint64_t           bitmask = 0,
                const uint32_t           target_id = 0)
        : AftNode(), _selectorKey(sel_nodes, sel_weights, sel_balances,
                       sel_sessions, sel_type, sel_subtype, sel_aggtype,
                       sel_flags, params, bitmask, target_id)
    {
    }

    ///
    /// @brief Construct AftSelector node
    /// @param [in] selectorKey          ///< Selector key
    ///
    AftSelector(const AftSelectorKey    &selectorKey)
        : AftNode(), _selectorKey(selectorKey) {
    }

    ~AftSelector() {};

    const std::vector<uint16_t> & weights()   const { return _selectorKey.selectorWeights; }
    const std::vector<uint16_t> & balances()  const { return _selectorKey.selectorBalances; }
    const AftTokenVector        & sessions()  const { return _selectorKey.selectorSessions; }
    const AftTokenVector        & nextNodes() const { return _selectorKey.selectorNodes; }

          uint32_t                size()      const { return nextNodes().size(); }
    const AftSelectorParams     & cfg_params() const { return _selectorKey.selectorParams; }
    const AftSelectorKey        & s_key() const { return _selectorKey; }

    AftSelectorType         type()      const { return _selectorKey.selectorType;}
    AftSelectorSubType      subtype()   const { return _selectorKey.selectorSubType;}
    AftSelectorAggType      aggtype()   const { return _selectorKey.selectorAggType;}
    uint32_t                flags()     const { return _selectorKey.selectorFlags;}

    //
    // type-check
    //
    bool  is_compact() { return (type() == AftSelectorType::COMPACT); }
    bool  is_regular() { return (type() == AftSelectorType::REGULAR); }
    bool  is_extended() { return (type() == AftSelectorType::EXTENDED); }
    bool  is_stateful() { return (type() == AftSelectorType::STATEFUL); }
    bool  is_adaptive() { return (type() == AftSelectorType::ADAPTIVE); }
    bool  is_default() { return (type() == AftSelectorType::UNKNOWN); }

    //
    // subtype-check
    //
    bool  is_consistent() { return (subtype() == AftSelectorSubType::CONS_HASH); }
    bool  is_srlg()       { return (subtype() == AftSelectorSubType::SRLG); }
    bool  is_multicast()  { return (subtype() == AftSelectorSubType::MULTICAST); }

    //
    // Agg-type check
    //
    bool  is_unicast() { return (aggtype() == AftSelectorAggType::UNICAST); }
    bool  is_composite() { return (aggtype() == AftSelectorAggType::COMPOSITE); }
    bool  is_agg_mcast() { return (aggtype() == AftSelectorAggType::MULTICAST); }

    //
    // Flag check
    //
    bool  need_adaptive() { return (flags() & AftSelectorFlags::ADAPTIVE); }
    bool  need_reroute() { return (flags() & AftSelectorFlags::FAST_REROUTE); }
    bool  use_balances() { return (flags() & AftSelectorFlags::BALANCES_PRESENT); }
    bool  is_weighted()  { return (flags() & AftSelectorFlags::WEIGHTED); }
    bool  is_cascaded()  { return (flags() & AftSelectorFlags::CASCADED); }
    bool  is_resilent()  { return (flags() & AftSelectorFlags::RESILIENT); }

    //
    // adaptive params
    //
    bool  is_adaptive_tolerance_configured() { return cfg_params().p_tolerance_valid; }
    uint32_t get_adaptive_tolerance() { return cfg_params().p_tolerance_valid; }
    uint32_t get_adaptive_scan_interval() { return cfg_params().p_scan_interval; }

    //
    // local bias
    //
    uint32_t get_local_bias_percent() { return cfg_params().k_local_bias_percentage; }

    //
    // Virtual Functions
    //

    virtual const std::string nodeType() const { return "AftSelector"; }

    virtual bool nodeIsValid (const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(nextNodes(), os));
    }

    virtual void nextNodes(AftTokenVector &nextTokens) {
        for (AftNodeToken aNodeToken: nextNodes())
            nextTokens.push_back(aNodeToken);
    };

    ///
    /// @brief           description of the Selector object
    /// @param [in] os   Reference to output stream to append to
    ///
    virtual std::ostream & description (std::ostream &os) const {
        AftNode::description(os);
        os << " nodes={";
        for (AftNodeToken token: _selectorKey.selectorNodes) {
            os << " " << token;
        }
        os << "}";
        return os;
    }

    ///
    /// @brief           Define << class operator to append description to an output stream
    /// @param [in] os   Reference to output stream to append to
    /// @param [in] node Reference to node to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftSelectorSubType &subtype)
    {
        switch (subtype) {
            case AftSelectorSubType::UNKNOWN :
                os << " Selector type : Unknown";
                break;
            case AftSelectorSubType::CONS_HASH:
                os << " Selector type : Consistent";
                break;
            case AftSelectorSubType::SRLG:
                os << " Selector type : SRLG";
                break;
            case AftSelectorSubType::MULTICAST:
                os << " Selector type : Multicast";
                break;
        }
        return os;
    }

    ///
    /// @brief           Define << class operator to append description to an output stream
    /// @param [in] os   Reference to output stream to append to
    /// @param [in] node Reference to node to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftSelectorAggType &aggtype)
    {
        switch (aggtype) {
            case AftSelectorAggType::UNICAST:
                os << " Selector Aggtype : Unicast";
                break;
            case AftSelectorAggType::MULTICAST:
                os << " Selector Aggtype : Consistent";
                break;
            case AftSelectorAggType::NO_REDISTRIBUTE:
                os << " Selector Aggtype : No Redistribute";
                break;
            case AftSelectorAggType::COMPOSITE:
                os << " Selector Aggtype : Composite";
                break;
        }
        return os;
    }

    ///
    /// @brief           Define << class operator to append description to an output stream
    /// @param [in] os   Reference to output stream to append to
    /// @param [in] node Reference to node to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftSelectorType &type)
    {
        switch (type) {
            case AftSelectorType::UNKNOWN:
                os << " Selector type : Unknown";
                break;
            case AftSelectorType::COMPACT:
                os << " Selector type : Compact";
                break;
            case AftSelectorType::REGULAR:
                os << " Selector type : Regular";
                break;
            case AftSelectorType::EXTENDED:
                os << " Selector type : Extended";
                break;
            case AftSelectorType::STATEFUL:
                os << " Selector type : Stateful";
                break;
            case AftSelectorType::ADAPTIVE:
                os << " Selector type : Adaptive";
                break;
            default:
                os << " Selector type : Invalid";
                break;
        }
        return os;
    }

    ///
    /// @brief           Define << class operator to append description to an output stream
    /// @param [in] os   Reference to output stream to append to
    /// @param [in] node Reference to node to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftSelector::Ptr &selector)
    {
        return selector->description(os);
    }

private:
    AftSelectorKey         _selectorKey;
};

///
/// Not used as of now, but enabling this if needed in future
///
class AftPfeMlist {
public:
    AftNodeToken    pfeNodeToken;
    AftTokenVector  mlistToken;
};

using AftPfeMlistVector = std::vector<AftPfeMlist>;

using AftRepPathIdVector = std::vector<AftIndex>;

///
/// @class   AftReplicate
/// @brief   Packet Replication class
/// @ingroup AftNodes
///
class AftReplicate: public AftNode {
public:
    ///
    /// @enum OpEnum
    /// @brief  Application types which uses AftReplicate Node
    ///
    typedef enum {
        Mcast,
        P2mp,
        Flood,
    } AftRepApp;

    ///
    /// @enum OpEnum
    /// @brief  Supported packet Replication Algorithms
    ///
    typedef enum {
        BinaryTree,
        FlatTree,
    } AftRepAlgo;

protected:
    AftIndex            _repCount; ///< Maximum replication entries count
    AftRepFlags         _repFlags; ///< Flag specific to AftNode e.g. host recieve is needed
    AftRepApp           _repApp;   ///< Replication Application e.g. Multicast, P2MP, Flood etc.
    AftRepAlgo          _repAlgo;  ///< Replication Algorithm e.g. None, Binary Tree, Flat Tree etc.
    AftProto            _proto;    ///< Node Protocol
    AftRepPathIdVector  _repPathIdVec; ///< Replication flow path id or derived fabric token id
                                       /// can we use AftNode setNodeParameter for this???
    AftEntryVector      _repEntryVec;  ///< Replication Node Member Entries
    //AftPfeMlistVector   _pfeMlistToken; ///< PFE node Token and Mlist Token vector

public:
    typedef std::shared_ptr<AftReplicate> Ptr; ///< Pointer type for Replication Node

    ///
    /// @brief Factory generate an replication list container
    ///
    /// @param [in] newRepCount Maximum number of entries in replication list
    ///
    static AftReplicate::Ptr create(const AftIndex      newRepCount,
                                    const AftRepFlags   newRepFlags,
                                    const AftRepApp     newRepApp,
                                    const AftRepAlgo    newRepAlgo,
                                    const AftProto      newProto,
                                    const AftRepPathIdVector    newRepPathId,
                                    const AftEntryVector        newRepEntry) {
        return std::make_shared<AftReplicate>(newRepCount,
                                              newRepFlags,
                                              newRepApp,
                                              newRepAlgo,
                                              newProto,
                                              newRepPathId,
                                              newRepEntry);
    }

    ///
    /// @brief Copy constructor for an indexed list entry
    ///
    /// @param [in] node  AftIndexList node to copy
    ///
    AftReplicate(const AftReplicate &node) = default;

    ///
    /// @brief Construct an replication list container
    ///
    /// @param [in] newRepCount Maximum number of entries in replication list
    ///
    AftReplicate(const AftIndex       newRepCount,
                 const AftRepFlags    newRepFlags,
                 const AftRepApp      newRepApp,
                 const AftRepAlgo     newRepAlgo,
                 const AftProto       newProto,
                 const AftRepPathIdVector   newRepPathIdVec,
                 const AftEntryVector       newRepEntry):
                        AftNode(), _repCount(newRepCount),
                        _repFlags(newRepFlags), _repApp(newRepApp),
                        _repAlgo(newRepAlgo), _proto(newProto),
                        _repPathIdVec(newRepPathIdVec),
                        _repEntryVec(newRepEntry) {};

    ~AftReplicate() {};

    /// @returns Returns total number of replication entries
    AftIndex repCount() const { return _repCount; };

    /// @returns Returns Replication Node flags
    AftRepFlags repFlags() const { return _repFlags; };

    /// @returns Returns replication Application i.e. Mcast, P2MP
    AftRepApp repApp() const { return _repApp; };

    /// @returns Returns Packet replication algorithm
    AftRepAlgo repAlgo() const { return _repAlgo; };

    /// @returns Returns Packet replication node proto
    AftProto proto() const { return _proto; };

    /// @return Return reference to vector of token entries
    const AftRepPathIdVector& repPathIdVec() const { return _repPathIdVec; };

    /// @return Return reference to vector of token entries
    const AftEntryVector& repEntryVec() const { return _repEntryVec; };

    //
    // Virtual Functions
    //
    virtual const std::string     nodeType() const { return "AftReplicate"; };
    virtual       bool            nodeIsContainer() const { return true; };

    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << " rep count:" << _repCount;
        return os;
    }

};

class AftOam : public AftNode
{
protected:
    uint32_t     _portNum;
    uint32_t     _outerVlanId;
    uint32_t     _innerVlanId;
    uint32_t     _upMegLevels;         /* MEG levels that are configured for UP MEP */
    uint32_t     _mipMegLevels;        /* MEG levels that are configured for MIP */
    uint32_t     _downMegLevels;       /* MEG levels that are configured for DOWN MEP*/
    uint32_t     _devNum;
    uint32_t     _iflIndex;
    AftDataPtr   _macAddr;
    AftNodeToken _oamToken;
public:
    using Ptr = std::shared_ptr<AftOam>;


    static AftOam::Ptr create(const uint32_t portNum,
                              const uint32_t outerVlanId,
                              const uint32_t innerVlanId,
                              const uint32_t upMegLevels,
                              const uint32_t mipMegLevels,
                              const uint32_t downMegLevels,
                              const uint32_t devNum,
                              const uint32_t iflIndex,
                              const AftDataPtr macAddr,
                              const AftNodeToken oamToken) {
        return std::make_shared<AftOam>(portNum,
                                       outerVlanId,
                                       innerVlanId,
                                       upMegLevels,
                                       mipMegLevels,
                                       downMegLevels,
                                       devNum,
                                       iflIndex,
                                       macAddr,
                                       oamToken);
    }


    AftOam(const uint32_t newPortNum,
           const uint32_t newOuterVlanId,
           const uint32_t newInnerVlanId,
           const uint32_t newUpMegLevels,
           const uint32_t newMipMegLevels,
           const uint32_t newDownMegLevels,
           const uint32_t newDevNum,
           const uint32_t newIflIndex,
           const AftDataPtr newMacAddr,
           const AftNodeToken newOamToken) :
    AftNode(),
    _portNum(newPortNum),
    _outerVlanId(newOuterVlanId),
    _innerVlanId(newInnerVlanId),
    _upMegLevels(newUpMegLevels),
    _mipMegLevels(newMipMegLevels),
    _downMegLevels(newDownMegLevels),
    _devNum(newDevNum),
    _iflIndex(newIflIndex),
    _macAddr(newMacAddr),
    _oamToken(newOamToken) {}

    AftOam(const AftOam& node): AftNode(node),
    _portNum(node._portNum),
    _outerVlanId(node._outerVlanId),
    _innerVlanId(node._innerVlanId),
    _upMegLevels(node._upMegLevels),
    _mipMegLevels(node._mipMegLevels),
    _downMegLevels(node._downMegLevels),
    _devNum(node._devNum),
    _iflIndex(node._iflIndex),
    _macAddr(node._macAddr),
    _oamToken(node._oamToken) {}

    ~AftOam() {}

    ///
    /// Copy operator, Move constructor and operator
    ///
    AftOam& operator=(AftOam&) = delete;
    AftOam(AftOam&&) = delete;
    AftOam& operator=(AftOam&&) = delete;

   /// @brief returns brief returns port num
    uint32_t portNum() const { return _portNum; }

    /// @brief returns inner vlan ID.
    uint32_t innerVlanId() const { return _innerVlanId; }

    /// @brief returns outer Vlan ID.
    uint32_t outerVlanId() const { return _outerVlanId; }

    uint32_t upMegLevels() const { return _upMegLevels; }

    uint32_t mipMegLevels() const { return _mipMegLevels; }

    uint32_t downMegLevels() const { return _downMegLevels; }

    uint32_t devNum() const { return _devNum; }

    uint32_t iflIndex() const { return _iflIndex; }

    AftDataPtr macAddr() const { return _macAddr;}

    AftNodeToken oamToken() const { return _oamToken;}
    //
    // Virtual Functions
    //

    /// @brief returns Printable class type of node
    virtual const std::string nodeType() const { return "AftOam"; };

    virtual std::ostream &description (std::ostream &os) const {
        AftNode::description(os);
        os << ", portNum = " << _portNum;
        os << ", innerVlanId = " << _innerVlanId;
        os << ", outerVlanId = " << _outerVlanId;
        os << ", upMegLevels = " << _upMegLevels;
        os << ", mipMegLevels = " << _mipMegLevels;
        os << ", downMegLevels = " << _downMegLevels;
        os << ", devNum = " << _devNum;
        os << ", iflIndex = " << _iflIndex;
        os << ", macAddr = " << std::hex << _macAddr << std::dec;
        os << ", oamToken = " << _oamToken;
        return os;
    }
};
#endif

