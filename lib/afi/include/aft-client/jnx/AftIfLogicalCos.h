///
///  @file   AftIfLogicalCos.h
///  @brief  AFT CoS Logical Interface Bind class
///
///  Created by Barry Burns on 08/03/17.
///  Copyright © 2017 Juniper Networks Inc. All rights reserved.
///

//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftIfLogicalCos__
#define __AftIfLogicalCos__

#include <iostream>
#include <bitset>
#include "jnx/AftNode.h"
#include "AftCosCommon.h"


using namespace CosCommon;


//
// Data Type definitions
//
class AftIfLogicalCos;
using AftIfLogicalCosPtr    = std::shared_ptr<AftIfLogicalCos>;

///
/// @class   AftIfLogicalCos
/// @brief   CoS Interface Bind node
///          Contains a list of AFT CoS node tokens
///          that are bound to a specific interface
///
class AftIfLogicalCos : public AftNode {
protected:
    ///
    /// Class Variables
    ///
    AftNodeToken   _interfaceToken;  // Interface Token to bind to CoS object
    AftNodeToken   _cosObjectToken;  // CoS token like Classifier/Rewrite
    AftCosRewriteProto _rwProto;     // optional qualification for Rewrite
    AftCosRewriteFlagSet _rwFlags;   // optional qualification for Rewrite

public:
    ///
    /// @func  create()
    /// @brief Create Shared Pointer
    ///
    /// @param [in] ifT   - Interface Object's Aft Token.
    ///                     Most likely AftIfLogical
    /// @param [in] cosT  - CoS Token to bind to Interface object.
    ///                     Most likely AftCosClassifierNode or AftCosRewrite
    /// @param [in] rwProto - protocol qualification for AftCosRewrite bind
    /// @param [in] rwFlags - flags qualification for AftCosRewrite bind
    ///
    /// @return  - Shared Pointer to created AftIfLogicalCos object
    ///
    static AftIfLogicalCosPtr create(
            AftNodeToken ifT,
            AftNodeToken cosT,
            AftCosRewriteProto rwProto = AftCosRewriteProto::ANY,
            AftCosRewriteFlagSet rwFlags = 0) {
        return std::make_shared<AftIfLogicalCos>(ifT, cosT, rwProto, rwFlags);
    }

    ///
    /// @brief AftIfLogicalCos Constructor
    ///
    /// @param [in] ifT   - Interface Object's Aft Token.
    ///                     Most likely AftIfLogical
    /// @param [in] cosT  - CoS Token to bind to Interface object.
    ///                     Most likely AftCosClassifierNode or AftCosRewrite
    /// @param [in] rwProto - protocol qualification for AftCosRewrite bind
    /// @param [in] rwFlags - flags qualification for AftCosRewrite bind
    ///
    /// @return  - AftIfLogicalCos object
    ///
    AftIfLogicalCos(AftNodeToken ifT, AftNodeToken cosT,
                    AftCosRewriteProto rwProto, AftCosRewriteFlagSet rwFlags) :
    _interfaceToken(ifT),
    _cosObjectToken(cosT),
    _rwProto(rwProto),
    _rwFlags(rwFlags) { }

    ///
    /// Accessor Methods
    ///
    AftNodeToken interfaceToken() const { return _interfaceToken; }
    AftNodeToken cosObjectToken() const { return _cosObjectToken; }

    AftCosRewriteProto rewriteProto() const { return _rwProto; }
    const AftCosRewriteFlagSet& rewriteFlags() const { return _rwFlags; }

    std::ostream & descriptionFormatted(std::ostream &os) const {
        AftNode::description(os);
        os << ": Cos IF Bind Node: interface token=" << _interfaceToken;
        os << ", CoS Token=" << _cosObjectToken << '\n';

        return os;
    }

    std::ostream & descriptionBrief(std::ostream &os) const {
        descriptionFormatted(os);
        return os;
    }

    //
    // Virtual AFT Node Functions
    //
    virtual const std::string nodeType() const override {
        return "AftIfLogicalCos";
    }

    virtual void nextNodes (AftTokenVector &nextTokens) override {
        if (AftTokenIsSet(_interfaceToken)) {
            nextTokens.push_back(_interfaceToken);
        }
        if (AftTokenIsSet(_cosObjectToken)) {
            nextTokens.push_back(_cosObjectToken);
        }
    }

    virtual std::ostream & description(std::ostream &os) const override {
        descriptionFormatted(os);
        return os;
    }
};

#endif // end __AftIfLogicalCos__
