//
/// @file   AftEntry.h
/// @brief  General model definitions for all Aft nodes and entries
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

#ifndef __AftEntry__
#define __AftEntry__

#include "jnx/AftNode.h"
#include <utility>

///
/// @defgroup AftEntries Entries
/// @brief Classes that define the core Aft forwarding entries in lookup nodes
///

///
/// @class   AftEntry
/// @brief   Base class for all Aft entries
/// @ingroup AftEntries
///
class AftEntry {
protected:
    AftNodeToken        _parentNode;      ///< Node token of the container for the entry
    AftParameters::UPtr _entryParams;     ///< Optional parameters for the entry
    AftMask             _entryMask = AFT_MASK_DEFAULT; ///< Mask for entry

public:

    ///
    /// @brief Constructor for initializing parent node
    ///
    AftEntry(const AftNodeToken newParentNode) : _parentNode(newParentNode) {}

    /// @returns Token for parent node of entry
    AftNodeToken parentNode() const { return _parentNode; };

    ///
    /// @brief                  Set the optional parameters of the entry
    /// @param [in] newParams   Pointer to parameters for entry
    ///
    void setEntryParameters(AftParameters::UPtr newParams) { _entryParams = std::move(newParams); };

    ///
    /// @brief                     Set the parent token
    /// @param [in] newParentNode  Parent node token
    ///
    void setParentToken(AftNodeToken newParentNode) { _parentNode = newParentNode; };

    ///
    /// @brief                  Set the (optional) entry mask
    /// @param [in] newMask     Mask for the entry
    ///
    void setEntryMask(AftMask newMask) { _entryMask = newMask; };

    ///
    /// @brief           Convenience parameter access template. Use this to access node parameters
    /// @param [in] name Parameter name to look for
    /// @returns         Pointer to AftData if parameter is found
    ///
    template <typename T>
    bool entryParameter(const std::string &name, T &resultValue) {
        if (_entryParams == nullptr) return false;
        return _entryParams->parameter<T>(name, resultValue);
    }

    ///
    /// @brief           Convenience parameter access template. Use this to access node parameters
    /// @param [in] name Parameter name to look for
    /// @returns         Pointer to AftData if parameter is found
    ///
    template <class T>
    std::shared_ptr<T> entryParameterData(const std::string &name) {
        if (_entryParams == nullptr) return nullptr;
        return _entryParams->parameterData<T>(name);
    }

    ///
    /// @brief               Convenience parameter setter. Use this to set node parameters
    /// @param [in] name     Parameter name to set
    /// @param [in] newValue Reference to value to associate with name
    ///
    template <typename T>
    void setEntryParameter(const std::string &name, const T &newValue) {
        if (_entryParams == nullptr) setEntryParameters(AftParameters::create());
        _entryParams->setParameter<T>(name, newValue);
    }

    ///
    /// @brief           Convenience parameter setter. Use this to set node parameters
    /// @param [in] name Parameter name to set
    /// @param [in] data Reference to data to associate with name
    ///
    void setEntryParameterData(const std::string &name, const AftDataPtr &data) {
        if (_entryParams == nullptr) setEntryParameters(AftParameters::create());
        _entryParams->setParameterData(name, data);
    }

    ///
    /// @brief Returns true if the node has optional parameters
    ///
    bool hasEntryParameters() const { return _entryParams != nullptr; };

    ///
    /// @brief                 Returns a vector of all of the parameter keys
    /// @param [out] paramKeys Reference to key vector to use
    /// @returns               True if the entry has parameters
    ///
    bool entryParameterKeys(AftParameterKeys &paramKeys) {
        if (_entryParams == nullptr) return false;
        _entryParams->parameterKeys(paramKeys);
        return true;
    }

    /// @returns Class of entry as a string
    virtual const std::string entryType() const { return "AftEntry"; };

    /// @ returns true if entry is a delete
    virtual bool              entryIsDelete() const { return false; };

    /// @ returns true if entry is valid
    virtual bool entryIsValid(const AftValidatorPtr &validator, std::ostream &os) {
        return (validator->validate(this->parentNode(), os));
    }

    /// @param [out] nextTokens    Appends valid next tokens to token vector
    /// @brief Appends valid next tokens to token vector
    virtual void nextNodes (AftTokenVector &nextTokens) {}

    /// @param [in]  index      Index of node token to retrieve
    /// @param [out] nodeToken  Value of node token if found
    /// @returns true if a node token is found for index
    /// @brief Returns a specified node token by index
    bool nextNodeByIndex(size_t index, AftNodeToken &nodeToken) {
        AftTokenVector nextTokens;

        this->nextNodes(nextTokens);

        if (index >= nextTokens.size()) return false;

        nodeToken = nextTokens[index];
        return true;
    }

    ///
    /// @brief Derived class should implement this function to print the derived class details.
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of entry appended
    ///
    virtual std::ostream &description (std::ostream &os) const {
        return os;
    }

    ///
    /// @brief            Define << class operator to append description to an output stream
    /// @param [in] os    Reference to output stream to append to
    /// @param [in] entry Reference to entry to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftEntry &entry) {
        os << "parentToken:" << entry._parentNode << std::hex << " entryMask:" << entry._entryMask << std::dec;
        return entry.description(os);
    }

    /// @param [out] entryKey  Value of entry key as an AftKey
    /// @returns true if the key can be expressed as an AftKey
    /// @brief Returns the AftKey for the entry
    virtual bool keyValue(AftKey &entryKey) { return false; };

    /// @param [out] entryKey  Value of entry key as an AftDataPtr
    /// @returns true if the key can be expressed as an AftDataPtr
    /// @brief Returns the AftDataPtr for the entry
    virtual bool keyValue(AftDataPtr &entryKey) { return false; };

    /// @param [out] entryKey  Value of entry key as an AftIndex
    /// @returns true if the key can be expressed as an AftIndex
    /// @brief Returns the AftIndex for the entry
    virtual bool keyValue(AftIndex &entryKey) { return false; };
};

///
/// @class   AftEntryTemplate
/// @brief   Template class to host key value pair for Aft Entries
///          Keeping this class in the middle of class hierarchy helps in two ways:
///          1. Base class AftEntryPtr can be passed around without specifying the template types.
///          2. Create or Delete entry can be casted to this class type for fetching key.
///
template <typename Key, typename Value>
class AftEntryTemplate: public AftEntry {
protected:
    Key     _key;          ///< Aft entry key
    Value   _value;        ///< Aft entry value

public:
    using Ptr = std::shared_ptr<AftEntryTemplate<Key,Value>>; ///< Pointer type AftEntryTemplate

    ///
    /// @brief Constructor for initializing key value pair
    ///
    template<typename KeyType, typename ValueType>
    AftEntryTemplate(AftNodeToken newParentNode,
                     KeyType      &&newKey,
                     ValueType    &&newValue) : AftEntry(newParentNode),
                                                        _key(std::forward<KeyType>(newKey)),
                                                        _value(std::forward<ValueType>(newValue)) {}

    /// @ returns true if entry is valid
    virtual bool entryIsValid(const AftValidatorPtr &validator, std::ostream &os) { return true;};

    /// @return AftEntry key
    const Key& key() const { return _key; }

    /// @return AftEntry value
    const Value& value() const { return _value;}
};

///
/// @class   AftDeleteEntryTemplate
/// @brief   Template class to delete entries
///
template <typename Key>
class AftDeleteEntryTemplate : public AftEntry {
protected:
    Key _key;  ///< Aft entry key

public:
    using Ptr = std::shared_ptr<AftDeleteEntryTemplate<Key>>; ///< Pointer type AftDeleteEntryTemplate

    ///
    /// @brief Constructor for initializing key
    ///
    template<typename KeyType>
    AftDeleteEntryTemplate(AftNodeToken newParentNode,
                           KeyType      &&newKey): AftEntry(newParentNode), _key(std::forward<KeyType>(newKey)) {}

    /// @ returns true if entry is valid
    virtual bool entryIsValid(const AftValidatorPtr &validator, std::ostream &os) { return true;};

    /// @ returns true if entry is a delete
    virtual bool              entryIsDelete() const { return true; };

    /// @return AftEntry key
    const Key& key() const { return _key; }
};

///
/// @class   AftEntryRoute
/// @brief   Route entry class
///
class AftEntryRoute : public AftEntryTemplate<AftDataPtr, AftNodeToken>
{
    bool _hwFlush;    ///< if true, install route to hardware otherwise it will be cached

public:

    typedef std::shared_ptr<AftEntryRoute> Ptr; ///< Pointer type Aft route entry

    ///
    /// @brief Factory generator for RouteEntry
    ///
    /// @param [in] newParentNode Token for parent node for entry
    /// @param [in] address       IP address
    /// @param [in] newEntryNode  Token for node to execute for this entry
    /// @param [in] newHwFlush    if true, install route to hardware otherwise it will be cached
    ///
    static AftEntryRoute::Ptr create(const AftNodeToken newParentNode,
                                     AftDataPtr         &&address,
                                     AftNodeToken       newEntryNode,
                                     const bool         newHwFlush = true) {
        return std::make_shared<AftEntryRoute>(newParentNode,
                                               std::move(address),
                                               newEntryNode,
                                               newHwFlush);
    }

    ///
    /// @brief Constructor for AftEntryRoute
    ///
    /// @param [in] newParentNode Token for parent node for entry
    /// @param [in] address       IP address
    /// @param [in] newEntryNode  Token for node to execute for this entry
    /// @param [in] newHwFlush    if true, install route to hardware otherwise it will be cached
    ///
    AftEntryRoute(const AftNodeToken   newParentNode,
                  AftDataPtr           &&address,
                  AftNodeToken         newEntryNode,
                  const bool           newHwFlush) : AftEntryTemplate(newParentNode,
                                                                      std::move(address),
                                                                      newEntryNode),
                                                                      _hwFlush(newHwFlush) {};

    /// @returns Class of entry as a string
    virtual const std::string entryType() const override { return "AftEntryRoute"; };

    /// @param [out] nextTokens    Appends valid next tokens to token vector
    /// @brief Appends valid next tokens to token vector
    virtual void nextNodes (AftTokenVector &nextTokens) override {
        nextTokens.push_back(value());
    };

    /// @ returns true if entry is valid
    virtual bool entryIsValid(const AftValidatorPtr &validator, std::ostream &os) override {
        return (validator->validate(this->parentNode(), os) &&
                validator->validate(this->value(), os));
    }

    /// @return hardware flush flag
    bool hwFlush() const { return _hwFlush; }

    ///
    /// @brief Print class description
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of entry appended
    ///
    virtual std::ostream &description (std::ostream &os) const override {
        os << " hwFlush:" << hwFlush() << " address:" << *key();
        os << " nextToken:"  << value() << std::endl;
        return os;
    }

    virtual bool keyValue(AftDataPtr &entryKey) override { entryKey = key(); return true; };
};

class AftDeleteEntryRoute : public AftDeleteEntryTemplate<AftDataPtr> {
    bool _hwFlush; ///< if true, uninstall route from hardware otherwise
                   ///< it will be removed from route table cache.

public:
    using Ptr = std::shared_ptr<AftDeleteEntryRoute>; ///< Pointer type AftDeleteEntryRoute

    ///
    /// @brief Factory generate a deletion entry for a route table container node
    ///
    /// @param [in] delParentNode Token for parent node for entry
    /// @param [in] delEntryKey   Unique key for entry
    ///
    static AftDeleteEntryRoute::Ptr create(const AftNodeToken   delParentNode,
                                           AftDataPtr           &&delEntryKey,
                                           bool                 newHwFlush = true) {
        return std::make_shared<AftDeleteEntryRoute>(delParentNode, std::move(delEntryKey), newHwFlush);
    }

    ///
    /// @brief Construct a deletion entry for a route table container node
    ///
    /// @param [in] delParentNode Token for parent node for entry
    /// @param [in] delEntryKey   Unique key for entry
    ///
    AftDeleteEntryRoute(const AftNodeToken   delParentNode,
                        AftDataPtr           &&delEntryKey,
                        bool                 newHwFlush) : AftDeleteEntryTemplate(delParentNode,
                                                                                  std::move(delEntryKey)),
                                                                                  _hwFlush(newHwFlush) {};

    /// @return hardware flush flag
    bool hwFlush() const { return _hwFlush; }

    /// @returns Class of entry as a string
    virtual const std::string entryType() const override { return "AftDeleteEntryRoute"; };

    virtual bool keyValue(AftDataPtr &entryKey) override { entryKey = key(); return true; };

    ///
    /// @brief Print class description
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of entry appended
    ///
    virtual std::ostream &description (std::ostream &os) const override {
        os << " address:" << *key() << std::endl;
        return os;
    }
};

///
/// @class   AftEntryIndex
/// @brief   Index entry class
///
class AftEntryIndex : public AftEntryTemplate<AftIndex, AftNodeToken>
{
public:
    using Ptr = std::shared_ptr<AftEntryIndex>; ///< Pointer type Aft index entry
    using AftEntryTemplate::AftEntryTemplate;

    ///
    /// @brief Factory generator for IndexEntry
    ///
    /// @param [in] newParentNode Token for parent node for entry
    /// @param [in] index         Index to which the entry will be inserted
    /// @param [in] newEntryNode  Token for node to execute for this entry
    ///
    static AftEntryIndex::Ptr create(const AftNodeToken newParentNode,
                                     AftIndex           index,
                                     AftNodeToken       newEntryNode) {
        return std::make_shared<AftEntryIndex>(newParentNode, index, newEntryNode);
    }

    /// @returns Class of entry as a string
    virtual const std::string entryType() const override { return "AftEntryIndex"; };

    /// @param [out] nextTokens    Appends valid next tokens to token vector
    /// @brief Appends valid next tokens to token vector
    virtual void nextNodes (AftTokenVector &nextTokens) override {
        nextTokens.push_back(value());
    };

    /// @ returns true if entry is valid
    virtual bool entryIsValid(const AftValidatorPtr &validator, std::ostream &os) override {
        return (validator->validate(this->parentNode(), os) &&
                validator->validate(this->value(), os));
    }

    ///
    /// @brief Print class description
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of entry appended
    ///
    virtual std::ostream &description (std::ostream &os) const override {
        os << " index:" << key() << " nextToken:"  << value() << std::endl;
        return os;
    }

    virtual bool keyValue(AftIndex &entryKey) override { entryKey = key(); return true; };
};

class AftDeleteEntryIndex : public AftDeleteEntryTemplate<AftIndex> {
public:
    using Ptr = std::shared_ptr<AftDeleteEntryIndex>; ///< Pointer type AftDeleteEntryIndex
    using AftDeleteEntryTemplate::AftDeleteEntryTemplate;

    ///
    /// @brief Factory generate a deletion entry for a index table container node
    ///
    /// @param [in] delParentNode Token for parent node for entry
    /// @param [in] delEntryKey   Unique key for entry
    ///
    static AftDeleteEntryIndex::Ptr create(const AftNodeToken   delParentNode,
                                           AftIndex             delEntryKey) {
        return std::make_shared<AftDeleteEntryIndex>(delParentNode, delEntryKey);
    }

    /// @returns Class of entry as a string
    virtual const std::string entryType() const override { return "AftDeleteEntryIndex"; };

    virtual bool keyValue(AftIndex &entryKey) override { entryKey = key(); return true; };

    ///
    /// @brief Print class description
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of entry appended
    ///
    virtual std::ostream &description (std::ostream &os) const override {
        os << " index:" << key() << std::endl;
        return os;
    }
};

///
/// @class   AftEntryKeyField
/// @brief   Aft entry class which supports key type AftKey, with which field type can
///          be specified for key.
///
class AftEntryKeyField : public AftEntryTemplate<AftKey, AftNodeToken> {
public:
    using Ptr = std::shared_ptr<AftEntryKeyField>; ///< Pointer type AftEntryKeyField
    using AftEntryTemplate::AftEntryTemplate;

    ///
    /// @brief Factory generator for AftEntryKeyField
    ///
    /// @param [in] newParentNode Token for parent node for entry
    /// @param [in] newKey        AftKey which has field name and key
    /// @param [in] newEntryNode  Token for node to execute for this entry
    ///
    static AftEntryKeyField::Ptr create(const AftNodeToken   newParentNode,
                                        AftKey               &&newKey,
                                        AftNodeToken         newEntryNode) {
        return std::make_shared<AftEntryKeyField>(newParentNode, std::move(newKey), newEntryNode);
    }

    /// @returns Class of entry as a string
    virtual const std::string entryType() const override { return "AftEntryKeyField"; };

    /// @param [out] nextTokens    Appends valid next tokens to token vector
    /// @brief Appends valid next tokens to token vector
    virtual void nextNodes (AftTokenVector &nextTokens) override {
        nextTokens.push_back(value());
    };

    /// @ returns true if entry is valid
    virtual bool entryIsValid(const AftValidatorPtr &validator, std::ostream &os) override {
        return (validator->validate(this->parentNode(), os) &&
                validator->validate(this->value(), os),
                validator->validate(this->key(), os));
    }

    ///
    /// @brief Print class description
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of entry appended
    ///
    virtual std::ostream &description (std::ostream &os) const override {
        os << " AftKey:" << key() << " nextToken:"  << value() << std::endl;
        return os;
    }

    virtual bool keyValue(AftKey &entryKey) override { entryKey = key(); return true; };
};

class AftDeleteEntryKeyField : public AftDeleteEntryTemplate<AftKey> {
public:
    using Ptr = std::shared_ptr<AftDeleteEntryKeyField>; ///< Pointer type AftDeleteEntryKeyField
    using AftDeleteEntryTemplate::AftDeleteEntryTemplate;

    ///
    /// @brief Factory generate a deletion entry for keyType AftKey
    ///
    /// @param [in] delParentNode Token for parent node for entry
    /// @param [in] delEntryKey   Unique key for entry
    ///
    static AftDeleteEntryKeyField::Ptr create(const AftNodeToken   delParentNode,
                                              AftKey               &&delEntryKey) {
        return std::make_shared<AftDeleteEntryKeyField>(delParentNode, std::move(delEntryKey));
    }

    /// @returns Class of entry as a string
    virtual const std::string entryType() const override { return "AftDeleteEntryKeyField"; };

    virtual bool keyValue(AftKey &entryKey) override { entryKey = key(); return true; };

    ///
    /// @brief Print class description
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of entry appended
    ///
    virtual std::ostream &description (std::ostream &os) const override {
        os << " AftKey:" << key() << std::endl;
        return os;
    }
};

#endif
