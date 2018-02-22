//
// Juniper P4 Agent
//
/// @file  AfiTypes.h
/// @brief Afi types
//
// Created by Sandesh Kumar Sodhi, January 2018
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

#ifndef __AfiTypes__
#define __AfiTypes__

#include <iostream>
#include <cstdint>
#include <set>
#include <map>

namespace AFIHAL {
//
// Core classes
//
class AfiEngine;
class AfiObject;
class AfiEntry;

using AfiJsonResourceId = uint64_t;        ///< AFI json resource Id
using AfiObjectId       = uint64_t;        ///< AFI object Id

using AfiObjectName     = std::string;

//
// Smart pointer type aliases
//
//using AfiEnginePtr      = std::shared_ptr<AfiEngine>;
//using AfiEngineWeakPtr  = std::weak_ptr<AfiEngine>;
using AfiObjectPtr      = std::shared_ptr<AfiObject>;
using AfiObjectWeakPtr  = std::weak_ptr<AfiObject>;
using AfiEntryPtr       = std::shared_ptr<AfiEntry>;
using AfiEntryWeakPtr   = std::weak_ptr<AfiEntry>;

//
// Aliases for maps and vectors
//
using AfiObjectVector   = std::vector<AfiObjectPtr>;
using AfiObjectMap      = std::map<AfiObjectName, AfiObjectPtr>;
using AfiObjectWeakMap  = std::map<AfiObjectName, AfiObjectWeakPtr>;

}  // namespace AFIHAL

#endif // __AfiTypes__
