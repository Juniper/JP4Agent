//
// Juniper P4 Agent
//
/// @file  AfiCapEntryAction.cpp
/// @brief Afi Content Aware Processor Entry Action Set
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

#include "AfiCapEntryAction.h"
#include <cstring>
#include <memory>

#include "Log.h"
#include "Utils.h"
#include "JaegerLog.h"

namespace AFIHAL
{
//
// Description
//
std::ostream &
AfiCapEntryAction::description(std::ostream &os) const
{
    os << "_________ AfiCapEntryAction _______" << std::endl;
    return os;
}

AfiCapEntryAction::AfiCapEntryAction(const AfiJsonResource &jsonRes) : AfiObject(jsonRes)
{
    // TBD: FIXME magic number 5000
    char bytes_decoded[5000];
    memset(bytes_decoded, 0, sizeof(bytes_decoded));
    int num_decoded_bytes =
        base64_decode(jsonRes.objStr(), bytes_decoded, 5000);
    capEntryAction.ParseFromArray(bytes_decoded, num_decoded_bytes);

    Log(DEBUG) << "num_decoded_bytes: " << num_decoded_bytes;
    Log(DEBUG) << "capEntryAction.ByteSize(): " << capEntryAction.ByteSize();
}

}  // namespace AFIHAL
