//
/// @file   AftCosWredCurve.h
/// @brief  Model definitions for CoS-specific Aft WRED Curve Node
//
//  Created by Barry Burns on 3/9/17.
//  Copyright © 2017 Juniper Networks Inc. All rights reserved.
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//

#ifndef _AftCosWredCurve_h_
#define _AftCosWredCurve_h_

#include <iostream>
#include <list>
#include <map>
#include "jnx/AftNode.h"
#include "AftCosCommon.h"

class AftCosWredCrvNode;
using AftCosWredCrvNodePtr = std::shared_ptr<AftCosWredCrvNode>;

///
/// A STL Map container Definitions - indexed by fill % (0-100) that
///  contains drop probability (0-100)
///
using AftWredCurve     = std::map<u_int8_t, u_int8_t>;
using AftWredCurvePair = std::pair<u_int8_t, u_int8_t>;

///
/// @class   AftCosWredCrvNode
/// @brief   CoS WRED Curve AFT Node
///
class AftCosWredCrvNode : public AftNode {
protected:

    ///
    /// Class Variables
    ///
    bool               _interpolate;
    AftWredCurve       _curveMap;

public:
#define AFT_COS_WRED_FILL_OR_DROP_MAXVAL   100
    ///
    /// @brief Factory generate a WRED curve Shared Pointer
    ///
    static AftCosWredCrvNodePtr create(AftWredCurve &&c,
                                       bool interpolate = false) {
        return std::make_shared<AftCosWredCrvNode>(std::move(c), interpolate);
    }

    ///
    /// @brief Constructor/Destructor
    ///
    AftCosWredCrvNode(AftWredCurve &&c, bool interpolate = false) :
        _interpolate(interpolate), _curveMap(std::move(c)) { }

    ///
    /// Accessor Methods
    ///
    AftStatus pointGet(u_int8_t fill, u_int8_t & drop_prob);

    AftStatus pointGetFirst(u_int8_t & fill, u_int8_t & drop_prob);
    AftStatus pointGetNext(u_int8_t & fill, u_int8_t & drop_prob);

    u_int8_t numCrvPtGet() { return _curveMap.size(); }

    bool     interpolateGet() { return _interpolate; }
    std::ostream & descriptionFormatted(std::ostream &os) const;
    std::ostream & descriptionBrief(std::ostream &os) const;

    //
    // Virtual AFT Node Functions
    //
    virtual const std::string nodeType() const { return "AftCosWredCrv"; };
    ///
    /// @function nodeIsValid
    /// @brief returns true if node is valid
    ///
    virtual bool nodeIsValid(const AftValidatorPtr &validator,
                             std::ostream          &os) {
        //
        // Validate Node Parameters
        //
        if (_curveMap.size() > (AFT_COS_WRED_FILL_OR_DROP_MAXVAL+1)) {
            AftCosError() << "WRED Map Table too large: act="
                << _curveMap.size() << " max="
                << (AFT_COS_WRED_FILL_OR_DROP_MAXVAL+1) << std::endl;
            return false;
        }

        for (auto &entry : _curveMap) {
            if (entry.first > AFT_COS_WRED_FILL_OR_DROP_MAXVAL) {
                AftCosError() << "Fill="
                    << int(entry.first) << ": out of range: max="
                    << AFT_COS_WRED_FILL_OR_DROP_MAXVAL << std::endl;
                return false;
            }
            if (entry.second > AFT_COS_WRED_FILL_OR_DROP_MAXVAL) {
                AftCosError() << "Drop Probability="
                    << int(entry.second) << ": out of range: max="
                    << AFT_COS_WRED_FILL_OR_DROP_MAXVAL << std::endl;
                return false;
            }
        }
        return true;
    }


    virtual std::ostream & description(std::ostream &os) const;
};

#endif // _AftCosWredCurve_h_
