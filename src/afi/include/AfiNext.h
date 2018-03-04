//
// Juniper P4 Agent
//
/// @file  AfiNext.h
/// @brief Afi Next
//
// Created by Sandesh Kumar Sodhi, December 2017
// Copyright (c) [2017] Juniper Networks, Inc. All rights reserved.
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

#ifndef SRC_AFI_INCLUDE_AFINEXT_H_
#define SRC_AFI_INCLUDE_AFINEXT_H_

#include <memory>
#include <vector>
#include "AfiTypes.h"

namespace AFIHAL
{
class AfiNext;
using AfiEnginePtr = std::shared_ptr<AfiEngine>;

using AfiNextPtr        = std::shared_ptr<AfiNext>;
using AfiNextWeakPtr    = std::weak_ptr<AfiNext>;
using AfiNextWeakVector = std::vector<AfiNextWeakPtr>;

//
// Class for connecting Afi objects.
// It is used by AfiObject and AfiEntry
// to point at "next" object in the forwarding topo.
//

class AfiNext
{
 protected:
    AfiObjectMap _next;

 public:
    void nextInsert(const AfiObjectPtr &nextNode);
    void nextRemove(const AfiObjectPtr &nextNode);
    void nextRemove(const AfiObjectName objectName);
    bool nextFind(AfiObjectName objectName, const AfiObjectPtr &nextNode) const;
    AfiObjectPtr nextFirst() const;
    uint32_t     nextCount() const;
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFINEXT_H_
