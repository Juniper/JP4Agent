//
/// @file   AftTokenTable.h
/// @brief  AftTokenTable and AftTokenEntry class definitions
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

#ifndef __AftTokenTable__
#define __AftTokenTable__

#include "jnx/AftNode.h"

///
/// @defgroup  AftToken Token Management
/// @brief Classes used as the core of token management for Aft
/// @{
///

class AftTokenEntry {
protected:
    std::string _nodeType;  ///< Type of node
    AftNodePtr  _node;      ///< Optional cache of node pointer
    
public:
    ///
    /// Template share pointer alias
    ///
    using Ptr = std::shared_ptr<AftTokenEntry>;

    ///
    /// @brief Constructor for simple token entries
    ///
    /// @param [in] newNode  Node for token entry
    ///
    AftTokenEntry(const AftNodePtr &newNode): _nodeType(newNode->nodeType()), _node(nullptr) {};
    
    /// @return Type of node for token entry
    const std::string nodeType() const { return _nodeType; };

    /// @return Pointer of cache node (used for debug and single-ended simulation)
    const AftNodePtr node() const { return _node; };

    /// @return Set pointer of cache node (used for debug and single-ended simulation)
    void setNode(const AftNodePtr &newNode) { _node = newNode; };
};

///
/// @class   AftTokenTable
/// @brief   Master class for managing token allocations
///

template <typename TableType, typename EntryType>
class AftTokenTableTemplate {
protected:
    AftNodeToken                      _tokenBase;  ///< Start point of token allocation
    AftNodeToken                      _tokenMax;   ///< Current maximum of token allocation
    std::map<AftNodeToken, EntryType> _tokens;
    
public:
    typedef std::shared_ptr<TableType>        Ptr; ///< Pointer type used to reference token table instances
    typedef std::map<AftNodeToken, EntryType> Map; ///< Base type of map
    
    ///
    /// @brief  Factory convenience creator for shared_ptr to decap table.
    ///
    static std::shared_ptr<TableType> create(AftNodeToken newTokenBase) {
        return std::make_shared<TableType>(newTokenBase);
    }
    
    //
    // Constructor and destructor
    //
    AftTokenTableTemplate(AftNodeToken newTokenBase): _tokenBase(newTokenBase), _tokenMax(newTokenBase) {};
    ~AftTokenTableTemplate() {};

    
    //
    // Size and iterators
    //
    size_t size() const { return _tokens.size(); };
    typename std::map<AftNodeToken, EntryType>::iterator begin() { return _tokens.begin(); };
    typename std::map<AftNodeToken, EntryType>::iterator end() { return _tokens.end(); };
    
    EntryType getNext (AftNodeToken &token) {
        EntryType retVal = nullptr;
        
        if (token == AFT_NODE_TOKEN_NONE) {
            auto it = _tokens.begin();
            if (it != _tokens.end()) {
                retVal = it->second;
            }
        } else {
            auto it = _tokens.find(token);
            if (it != _tokens.end()) {
                it++;
                if (it != _tokens.end()) {
                    retVal = it->second;
                }
            }
        }
        
        return retVal;
    }

    //
    // Token Management
    //
    //
    // Token allocation
    //

    /// @returns Next valid token in the table
    AftNodeToken nextToken() { return ++_tokenMax; };
    
    /// @brief Releases a token back to the table
    void releaseToken() { };
    
    /// @brief Raises the current token count above the supplied value
    void raiseToken(AftNodeToken newToken) {
        if (_tokenMax <= newToken)
            _tokenMax = newToken + 1;
    }

    ///
    /// @brief Insert the specified entry into the table
    /// @param [in] entry Reference to entry to remember in token table
    ///
    void insert (const EntryType &entry) {
        //
        // Remember our token
        //
        _tokens[entry->nodeToken()] = entry;
    };
    
    ///
    /// @brief Remove the specified token from the table
    /// @ingroup AftToken
    /// @param [in]    nodeToken   Value of token
    ///
    void remove (AftNodeToken nodeToken) {
        _tokens.erase(nodeToken);
    };
    
    ///
    /// @brief Check that a token is valid
    /// @ingroup AftToken
    /// @param [in]    nodeToken   Value of token
    /// @retval True   Token is valid
    /// @retval False  Token isn't in table
    ///
    bool isValid(AftNodeToken nodeToken) {
        auto it = _tokens.find(nodeToken);
        return (it != _tokens.end());
    };
    
    ///
    /// @brief Find specified token in table
    /// @ingroup AftToken
    /// @param [in]    nodeToken   Value of token
    /// @param [in]    entry  Reference to entry if successful
    /// @retval True   Token was found
    /// @retval False  Token isn't in table
    ///
    bool find(AftNodeToken nodeToken, EntryType &entry) {
        auto it = _tokens.find(nodeToken);
        if (it != _tokens.end()) {
            entry = it->second;
            return true;
        }
        
        return false;
    }
    
    ///
    /// @brief Check that a token is of a specified type
    /// @ingroup AftToken
    /// @param [in]    nodeToken   Value of token
    /// @param [in]    nodeType    Type of token to check for
    /// @retval True   Token is of specified type
    /// @retval False  Token isn't in table or isn't of the specified type
    ///
    bool isOfType(AftNodeToken nodeToken, std::string nodeType) {
        EntryType entry;
        
        if (find(nodeToken, entry)) {
            return (entry->nodeType() == nodeType);
        }
        return false;
    };
};

class AftTokenTable: public AftTokenTableTemplate<AftTokenTable, AftTokenEntry::Ptr> {
protected:
    bool _isCached = false;
    
public:
    using AftTokenTableTemplate::AftTokenTableTemplate;
    
    void setIsCached(const bool newIsCached) { _isCached = newIsCached; };
    
    void insert (const AftNodePtr &node) {
        AftTokenEntry::Ptr entry = std::make_shared<AftTokenEntry>(node);
        if (_isCached) {
            entry->setNode(node);
        }
        _tokens[node->nodeToken()] = entry;
    }

    AftNodePtr nodeForToken (AftNodeToken nodeToken) {
        AftTokenEntry::Ptr entry;
        if (find(nodeToken, entry)) {
            return entry->node();
        } else {
            return nullptr;
        }
    };
    
    AftNodePtr nodeGetNext (AftNodeToken nodeToken) {
        AftTokenEntry::Ptr entry = getNext(nodeToken);
        return (entry != nullptr) ? entry->node() : nullptr;
    };
};

typedef std::shared_ptr<AftTokenTable> AftTokenTablePtr;

///
/// @}
///

#endif
