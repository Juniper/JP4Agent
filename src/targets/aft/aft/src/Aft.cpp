//
// Juniper P4 Agent
//
/// @file  Aft.cpp
/// @brief Aft
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

#include "Aft.h"
#include <string>

AFIHAL::AfiDeviceUPtr
createDevice(const std::string &name)
{
    Log(DEBUG) << "___ createDevice___";
    return AFTHALP::AftDevice::create(name);
}

// const std::string _aft_debugmode = "no-aft-server";
const std::string _aft_debugmode = "";
namespace AFTHALP
{
}  // namespace AFTHALP
