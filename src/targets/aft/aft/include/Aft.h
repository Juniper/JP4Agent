//
// Juniper P4 Agent
//
/// @file  Aft.h
/// @brief Aft includes
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

#ifndef SRC_TARGETS_AFT_AFT_INCLUDE_AFT_H_
#define SRC_TARGETS_AFT_AFT_INCLUDE_AFT_H_

#include <string>

extern const std::string _aft_debugmode;

#include "Afi.h"
#include "AftClient.h"
#include "AftDevice.h"
#include "AftObject.h"
#include "AftTree.h"
#include "AftTreeEntry.h"
#include "Log.h"
#include "Utils.h"

AFIHAL::AfiDeviceUPtr createDevice(const std::string &name);

namespace AFTHALP
{
}  // namespace AFTHALP

#endif  // SRC_TARGETS_AFT_AFT_INCLUDE_AFT_H_
