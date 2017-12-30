///
///  @file   AftCosRewrite.h
///  @brief  AFT CoS Rewrite Node class
//
///  Created by Barry Burns on 3/9/17.
///  Copyright © 2017 Juniper Networks Inc. All rights reserved.
///

//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftCosRewrite_h__
#define __AftCosRewrite_h__

#include <iostream>
#include "jnx/AftNode.h"
#include "AftCosCommon.h"

///
/// @class   AftCosRewriteLpEntry
/// @brief   CoS Rewrite Loss Priority Entry
///
class AftCosRewriteLossPriorityEntry {
protected:
    ///
    /// Class Variables
    ///
    bool          _enable;
    u_int8_t      _rewriteByte;

public:
    ///
    /// Constructor/ Destructor
    ///
    AftCosRewriteLossPriorityEntry() : _enable(false), _rewriteByte(0) {}
    AftCosRewriteLossPriorityEntry(bool en, u_int8_t rwb) :
        _enable(en), _rewriteByte(rwb) {}

    ///
    /// Accessor Methods
    ///
    void     enableSet(bool en)      { _enable = en; }
    bool     enableGet() const       { return _enable; }
    void     rwByteSet(u_int8_t rwb) { _rewriteByte = rwb; }
    u_int8_t rwByteGet() const       { return _rewriteByte; }

    bool operator==(const AftCosRewriteLossPriorityEntry &t) const;
    bool operator!=(const AftCosRewriteLossPriorityEntry &t) const;

    std::ostream & description(std::ostream &os) const;
};

using  AftCosRwLpMap = std::map<CosCommon::AftCosLossPriority,
                                AftCosRewriteLossPriorityEntry>;

///
/// @class   AftCosRewriteEntry
/// @brief   CoS Rewrite Entry - one per Forwarding class
///
class AftCosRewriteEntry {
protected:
    ///
    /// Class Variables
    ///
    AftCosRwLpMap _lossPriorityMap;   // Loss Priority Map: key=loss priority

public:

    ///
    /// @brief Constructor/Destructor
    ///
    AftCosRewriteEntry() {}
    AftCosRewriteEntry(AftCosRwLpMap &&m) : _lossPriorityMap(std::move(m)) {}

    ///
    /// Accessor Methods
    ///
    const AftCosRwLpMap & rwLpMapGet() const { return _lossPriorityMap; }
    const AftCosRewriteLossPriorityEntry & lpEntryGet(CosCommon::AftCosLossPriority lp);
    void lpEntrySet(CosCommon::AftCosLossPriority   lp,
                    AftCosRewriteLossPriorityEntry &entry);

    ///
    /// Operator Methods
    ///
    bool operator==(const AftCosRewriteEntry &t) const;
    bool operator!=(const AftCosRewriteEntry &t) const;

    std::ostream & description(std::ostream &os) const;
};

using AftCosRewriteEntryMap = std::map<u_int8_t, AftCosRewriteEntry>;

class AftCosRewriteNode;
using AftCosRewriteNodePtr = std::shared_ptr<AftCosRewriteNode>;

///
/// @class   AftCosRewriterNode
/// @brief   CoS Rewrite node
///
class AftCosRewriteNode : public AftNode {
protected:
    ///
    /// Class Variables
    ///
    CosCommon::AftCosClsType _type;     // Identifies the type of Classsifier
    AftCosRewriteEntryMap    _forwardingClassMap;  // Map of Forwarding class entries, key=forwarding class

public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftCosRewriteNode(CosCommon::AftCosClsType t, AftCosRewriteEntryMap &&m) :
        _type(t), _forwardingClassMap(std::move(m)) {}

    ///
    /// @brief Factory generate a Rewite Node Shared Pointer
    ///
    static AftCosRewriteNodePtr create(CosCommon::AftCosClsType t,
                                       AftCosRewriteEntryMap &&m) {
        return std::make_shared<AftCosRewriteNode>(t, std::move(m));
    }


    ///
    /// Accessor Methods
    ///
    CosCommon::AftCosClsType typeGet() { return _type;}
    const AftCosRewriteEntry & entryGet(u_int8_t fc);
    const AftCosRewriteEntryMap & entriesMapGet() { return _forwardingClassMap; }
    std::ostream & descriptionFormatted(std::ostream &os) const;
    std::ostream & descriptionBrief(std::ostream &os) const;

    //
    // Virtual AFT Node Functions
    //
    virtual const std::string nodeType() const {
        return "AftCosRewrite";
    }

    virtual std::ostream & description(std::ostream &os) const;
    bool entryExist (uint8_t forwClass);
};

#endif // __AftCosRewrite_h__
