//
// Juniper P4 Agent
//
/// @file  AfiEncap.h
/// @brief Afi packet encapsulation
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

#ifndef SRC_AFI_INCLUDE_AFIENCAP_H_
#define SRC_AFI_INCLUDE_AFIENCAP_H_

#include <memory>
#include "AfiDM.h"
#include "AfiObject.h"

namespace AFIHAL
{
class AfiEncap;
using AfiEncapPtr     = std::shared_ptr<AfiEncap>;
using AfiEncapWeakPtr = std::weak_ptr<AfiEncap>;

class AfiEncap : public AfiObject
{
 public:
    explicit AfiEncap(const AfiJsonResource &jsonRes);

    ~AfiEncap() {}

    //
    // Debug
    //
    std::ostream &description(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const AfiEncapPtr &afiencap)
    {
        return afiencap->description(os);
    }

 protected:
    juniper::afi_encap::AfiEncap _encap;
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFIENCAP_H_
