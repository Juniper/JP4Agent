//
// Juniper P4 Agent
//
/// @file  AfiCapEntry.h
/// @brief Afi Content Aware Processor Entry
//
// Created by Sudheendra Gopinath, June 2018
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

#ifndef SRC_AFI_INCLUDE_AFICAPEntry_H_
#define SRC_AFI_INCLUDE_AFICAPEntry_H_

#include <memory>
#include "AfiDM.h"
#include "AfiObject.h"

namespace AFIHAL
{
class AfiCapEntry;
using AfiCapEntryPtr     = std::shared_ptr<AfiCapEntry>;
using AfiCapEntryWeakPtr = std::weak_ptr<AfiCapEntry>;

class AfiCapEntry : public AfiObject
{
 public:
    explicit AfiCapEntry(const AfiJsonResource &jsonRes);

    ~AfiCapEntry() {}

    //
    // Debug
    //
    std::ostream &description(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const AfiCapEntryPtr &aficapEntry)
    {
        return aficapEntry->description(os);
    }

 protected:
    juniper::afi_cap_entry::AfiCapEntry _capEntry;
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFICAPEntry_H_
