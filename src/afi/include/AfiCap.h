//
// Juniper P4 Agent
//
/// @file  AfiCap.h
/// @brief Afi Content Aware Processor
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

#ifndef SRC_AFI_INCLUDE_AFICAP_H_
#define SRC_AFI_INCLUDE_AFICAP_H_

#include <memory>
#include "AfiDM.h"
#include "AfiObject.h"

namespace AFIHAL
{
class AfiCap;
using AfiCapPtr     = std::shared_ptr<AfiCap>;
using AfiCapWeakPtr = std::weak_ptr<AfiCap>;

class AfiCap : public AfiObject
{
 public:
    explicit AfiCap(const AfiJsonResource &jsonRes);

    ~AfiCap() {}

    virtual bool createChildJsonRes(const uint32_t tId, //P4InfoTablePtr table,
                                    const uint32_t aId, //P4InfoActionPtr action,
                                    const std::vector<AfiTEntryMatchField> &mfs,
                                    const std::vector<AfiAEntry> &aes,
                                    Json::Value& result) override;

    ::ywrapper::UintValue gid() { return _cap.group_id(); }
    ::ywrapper::UintValue gp() { return _cap.group_priority(); }

    //
    // Debug
    //
    std::ostream &description(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const AfiCapPtr &aficap)
    {
        return aficap->description(os);
    }

 protected:
    juniper::afi_cap::AfiCap _cap;
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFICAP_H_
