//
// Juniper P4 Agent
//
/// @file  AfiCapMatch.h
/// @brief Afi Content Aware Processor Match Set
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

#ifndef SRC_AFI_INCLUDE_AFICAPMATCH_H_
#define SRC_AFI_INCLUDE_AFICAPMATCH_H_

#include <memory>
#include "AfiDM.h"
#include "AfiObject.h"

namespace AFIHAL
{
class AfiCapMatch;
using AfiCapMatchPtr     = std::shared_ptr<AfiCapMatch>;
using AfiCapMatchWeakPtr = std::weak_ptr<AfiCapMatch>;

class AfiCapMatch : public AfiObject
{
 public:
    explicit AfiCapMatch(const AfiJsonResource &jsonRes);

    ~AfiCapMatch() {}

    //
    // Debug
    //
    std::ostream &description(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const AfiCapMatchPtr &aficapmatch)
    {
        return aficapmatch->description(os);
    }

#ifdef SUD
 // attached object
 protected:
#endif // SUD
    juniper::afi_cap_match::AfiCapMatch capMatch;
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFICAPMATCH_H_
