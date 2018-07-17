//
// Juniper P4 Agent
//
/// @file  BrcmCapEntry.h
/// @brief Brcm CAP Entry Object
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

#ifndef __BrcmCapEntry__
#define __BrcmCapEntry__

namespace BRCMHALP {

class BrcmCapEntry;
using BrcmCapEntryPtr = std::shared_ptr<BrcmCapEntry>;
using BrcmCapEntryWeakPtr = std::weak_ptr<BrcmCapEntry>;

class BrcmCapEntry: public BrcmObjectTemplate<AFIHAL::AfiCapEntry, BrcmCapEntry>
{
    using BrcmObjectTemplate::BrcmObjectTemplate;
public:
    //BrcmCapEntry (const AfiJsonResource &jsonRes) : AfiCapEntry(jsonRes) {}
    //~BrcmCapEntry () {}

    /// 
    /// @brief  Create the hardware state
    /// 
    void _bind() override;
    
    //
    // @brief  Debug
    //
    std::ostream &description (std::ostream &os) const;
    
    friend std::ostream &operator<< (std::ostream &os,
                                     const BrcmCapEntryPtr &BrcmCapEntry) {
        return BrcmCapEntry->description(os);
    }
private:
    FpEntryPtr _fpe;
};

class BrcmCapEntryMatch;
using BrcmCapEntryMatchPtr = std::shared_ptr<BrcmCapEntryMatch>;
using BrcmCapEntryMatchWeakPtr = std::weak_ptr<BrcmCapEntryMatch>;

class BrcmCapEntryMatch: public BrcmObjectTemplate<AFIHAL::AfiCapEntryMatch, BrcmCapEntryMatch>
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
                                     const BrcmCapEntryMatchPtr &BrcmCapEntryMatch) {
        return BrcmCapEntryMatch->description(os);
    }
};

class BrcmCapEntryAction;
using BrcmCapEntryActionPtr = std::shared_ptr<BrcmCapEntryAction>;
using BrcmCapEntryActionWeakPtr = std::weak_ptr<BrcmCapEntryAction>;

class BrcmCapEntryAction: public BrcmObjectTemplate<AFIHAL::AfiCapEntryAction, BrcmCapEntryAction>
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
                                     const BrcmCapEntryActionPtr &BrcmCapEntryAction) {
        return BrcmCapEntryAction->description(os);
    }
};

}  // namespace BRCMHALP

#endif // __BrcmCapEntry
