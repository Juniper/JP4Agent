//
// Juniper P4 Agent
//
/// @file  AfiTreeEncap.h
/// @brief Afi tree and packet encapsulation
//
// Created by Sudheendra Gopinath, August 2018
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

#ifndef SRC_AFI_INCLUDE_AFITREEENCAP_H_
#define SRC_AFI_INCLUDE_AFITREEENCAP_H_

#include <memory>
#include "AfiDM.h"
#include "AfiObject.h"

namespace AFIHAL
{
class AfiTreeEncap;
using AfiTreeEncapPtr     = std::shared_ptr<AfiTreeEncap>;
using AfiTreeEncapWeakPtr = std::weak_ptr<AfiTreeEncap>;

class AfiTreeEncap : public AfiObject
{
 public:
    explicit AfiTreeEncap(const AfiJsonResource &jsonRes);

    ~AfiTreeEncap() {}

    virtual bool createChildJsonRes(const uint32_t tId, //P4InfoTablePtr table,
                                    const uint32_t aId, //P4InfoActionPtr action,
                                    const std::vector<AfiTEntryMatchField> &mfs,
                                    const std::vector<AfiAEntry> &aes,
                                    Json::Value& result) override;
    //
    // Debug
    //
    std::ostream &description(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const AfiTreeEncapPtr &afiTreeEncap)
    {
        return afiTreeEncap->description(os);
    }

 protected:
    juniper::afi_tree_encap::AfiTreeEncap _treeEncap;
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFITREEENCAP_H_
