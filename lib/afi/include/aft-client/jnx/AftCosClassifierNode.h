///
///  @file   AftCosClassifierNode.h
///  @brief  AFT CoS classifier Node class
///
///
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

#ifndef __AftCosClassifierNode__
#define __AftCosClassifierNode__

#include <iostream>
#include "jnx/AftNode.h"
#include "AftCosCommon.h"

using namespace CosCommon;


///
/// @class   AftCosClassifierEntry
/// @brief   AFT CoS Classifier Entry that contains one classifier code point
///
class AftCosClassifierEntry {
protected:
    ///
    /// Class Variables
    ///
    u_int8_t           _codePoint         = 0; // Matching Code Point
    u_int8_t           _forwardingClass   = 0; // Forwarding Class
    AftCosLossPriority _lossPriorityPoint = AftCosLossPriority::Low;

public:
    ///
    /// @brief Constructor/Destructor
    ///
    AftCosClassifierEntry() {}
    AftCosClassifierEntry(u_int8_t cp, u_int8_t fc, AftCosLossPriority plp) :
        _codePoint(cp), _forwardingClass(fc), _lossPriorityPoint(plp) {}

    ///
    /// Accessor Methods
    ///
    u_int8_t codePointGet() const      { return _codePoint; }
    void     codePointSet(u_int8_t cp) { _codePoint = cp; }
    u_int8_t forwClassGet() const      { return _forwardingClass; }
    void     forwClasSet(u_int8_t fc)  { _forwardingClass = fc; }
    AftCosLossPriority lossPriorityGet() const       { return _lossPriorityPoint; }
    void     lossPrioritySet(AftCosLossPriority lp)  { _lossPriorityPoint = lp; }

    ///
    /// Operator Methods
    ///
    bool operator ==(const AftCosClassifierEntry &t) const;
    bool operator !=(const AftCosClassifierEntry &t) const;

    std::ostream & description(std::ostream &os) const;
};

//
// Data Type definitions
//
class AftCosClassifierNode;
using AftCosClassifierNodePtr    = std::shared_ptr<AftCosClassifierNode>;
using AftCosClassifierEntryMap   = std::map<u_int8_t, AftCosClassifierEntry>;

///
/// @class   AftCosClassifierNode
/// @brief   CoS Classifier node
///
class AftCosClassifierNode : public AftNode {
protected:
    ///
    /// Class Variables
    ///
    AftCosClsType             _type;      // Identifies the type of Classsifier
    AftCosClassifierEntryMap  _entries;   // Map of entries: key=code point

public:
    ///
    /// @brief Create Shared Pointer
    ///
    static AftCosClassifierNodePtr create(CosCommon::AftCosClsType t,
                                          AftCosClassifierEntryMap &&m) {
        return std::make_shared<AftCosClassifierNode>(t, std::move(m));
    }

    ///
    /// @brief CoS Classifier AFT Node Constructor/Destructor
    ///
    AftCosClassifierNode(CosCommon::AftCosClsType t,
                         AftCosClassifierEntryMap &&m) : _type(t),
        _entries(std::move(m)) { }

    ///
    /// Accessor Methods
    ///
    CosCommon::AftCosClsType typeGet() { return _type;}
    const AftCosClassifierEntry & entryGetByCp(u_int8_t cp);
    bool doesCodePointExist(u_int8_t cp);
    u_int32_t maxCodePointGet();
    u_int32_t entriesNumGet() { return (u_int32_t) _entries.size();}
    const AftCosClassifierEntryMap & entriesMapGet() { return _entries; }
    std::ostream & descriptionFormatted(std::ostream &os) const;
    std::ostream & descriptionBrief(std::ostream &os) const;

    //
    // Virtual AFT Node Functions
    //

    ///
    /// @ returns true if node is valid
    ///
    virtual bool nodeIsValid(const AftValidatorPtr &validator,
                             std::ostream          &os) {
        //
        // Validate Node Parameters
        //
        if (_entries.size() > maxCodePointGet()) {
            AftCosError() << "Classifier Map Table "
                << "too large: act=" << _entries.size() << " max="
                << maxCodePointGet() << "for type="
                << classifierTypeString(_type) << std::endl;
            return false;
        }

        for (auto &entry : _entries) {
            if (entry.first >= maxCodePointGet()) {
                AftCosError() << "CodePoint=" << int(entry.first)
                    << ": out of range: max=" << (maxCodePointGet() - 1)
                    << std::endl;
                return false;
            }
            if (entry.first != entry.second.codePointGet()) {
                AftCosError() << "Key=" << int(entry.first)
                    << " does not match codepoint="
                    << int(entry.second.codePointGet())
                    << std::endl;
                return false;
            }
            if (entry.second.forwClassGet() >= AFT_COS_MAX_FORW_CLASS) {
                AftCosError() << "CodePoint=" << entry.first
                    << ": Forwarding Class out of range: fc="
                    << int(entry.second.forwClassGet())
                    << " max=" << (AFT_COS_MAX_FORW_CLASS - 1) << std::endl;
                return false;
            }
        }
        return true;
    }

    virtual const std::string nodeType() const {
        return "AftCosClassifier";
    }

    virtual std::ostream & description(std::ostream &os) const;
};

#endif
