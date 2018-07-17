//
// Juniper P4 Agent
//
/// @file  BrcmCap.h
/// @brief Brcm CAP Object
//
// Created by Sudheendra Gopinath, March 2018
// Copyright (c) [2018] Juniper Networks, Inc. All rights reserved.
//
// All rights reserved.
//
// Notice and Disclaimer: This code is licensed to you under the Apache
// License 2.0 (the "License"). You may not use this code except in compliance
// with the License. This code is not an official Juniper product. You can
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Third-Party Code: This code may depend on other components under separate
// copyright notice and license terms. Your use of the source code for those
// components is subject to the terms and conditions of the respective license
// as noted in the Third-Party source code file.
//

#ifndef __BrcmCap__
#define __BrcmCap__

namespace BRCMHALP {

class BrcmCap;
using BrcmCapPtr = std::shared_ptr<BrcmCap>;
using BrcmCapWeakPtr = std::weak_ptr<BrcmCap>;

class BrcmCap: public BrcmObjectTemplate<AFIHAL::AfiCap, BrcmCap>
{
    using BrcmObjectTemplate::BrcmObjectTemplate;
public:
    //BrcmCap (const AfiJsonResource &jsonRes) : AfiCap(jsonRes) {}
    //~BrcmCap () {}

    /// 
    /// @brief  Create the hardware state
    /// 
    void _bind() override;
    
    //
    // @brief  Debug
    //
    std::ostream &description (std::ostream &os) const;
    
    friend std::ostream &operator<< (std::ostream &os,
                                     const BrcmCapPtr &BrcmCap) {
        return BrcmCap->description(os);
    }
};

class BrcmCapMatch;
using BrcmCapMatchPtr = std::shared_ptr<BrcmCapMatch>;
using BrcmCapMatchWeakPtr = std::weak_ptr<BrcmCapMatch>;

class BrcmCapMatch: public BrcmObjectTemplate<AFIHAL::AfiCapMatch, BrcmCapMatch>
{
    using BrcmObjectTemplate::BrcmObjectTemplate;
public:
    /// 
    /// @brief  Create the hardware state
    /// 
    void _bind() { }
    
    //
    // @brief  Debug
    //
    std::ostream &description (std::ostream &os) const {
        os << "" << std::endl;
        return os;
    }
    
    friend std::ostream &operator<< (std::ostream &os,
                                     const BrcmCapMatchPtr &BrcmCapMatch) {
        return BrcmCapMatch->description(os);
    }
};

class BrcmCapAction;
using BrcmCapActionPtr = std::shared_ptr<BrcmCapAction>;
using BrcmCapActionWeakPtr = std::weak_ptr<BrcmCapAction>;

class BrcmCapAction: public BrcmObjectTemplate<AFIHAL::AfiCapAction, BrcmCapAction>
{
    using BrcmObjectTemplate::BrcmObjectTemplate;
public:
    /// 
    /// @brief  Create the hardware state
    /// 
    void _bind() { } 
    
    //
    // @brief  Debug
    //
    std::ostream &description (std::ostream &os) {
        os << "" << std::endl;
        return os;
    }
    
    friend std::ostream &operator<< (std::ostream &os,
                                     const BrcmCapActionPtr &BrcmCapAction) {
        return BrcmCapAction->description(os);
    }
};

}  // namespace BRCMHALP

#endif // __BrcmCap
