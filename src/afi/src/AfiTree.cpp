//
// Juniper P4 Agent
//
/// @file  AfiTree.cpp
/// @brief Afi Tree
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

#include "AfiTree.h"
#include <jaegertracing/Tracer.h>
#include <cstring>
#include <memory>

#include "Log.h"
#include "Utils.h"

extern std::unique_ptr<opentracing::v1::Span> span;

namespace AFIHAL
{
//
// Description
//
std::ostream &
AfiTree::description(std::ostream &os) const
{
    os << "_________ AfiTree _______" << std::endl;
    return os;
}

AfiTree::AfiTree(const AfiJsonResource &jsonRes) : AfiObject(jsonRes)
{
    // TBD: FIXME magic number 5000

    char bytes_decoded[5000];
    memset(bytes_decoded, 0, sizeof(bytes_decoded));
    int num_decoded_bytes =
        base64_decode(jsonRes.objStr(), bytes_decoded, 5000);

    Log(DEBUG) << "bytes_decoded: " << bytes_decoded;
    Log(DEBUG) << "num_decoded_bytes: " << num_decoded_bytes;

    _tree.ParseFromArray(bytes_decoded, num_decoded_bytes);

    Log(DEBUG) << "tree.ByteSize(): " << _tree.ByteSize();
    ::ywrapper::StringValue key_field = _tree.key_field();
    Log(DEBUG) << "key_field: " << key_field.value();

    std::stringstream ks;
    ks << key_field.value();
    opentracing::string_view key("AFI:AFITree:Key Field");
    opentracing::string_view key_val(ks.str());
    span->SetBaggageItem(key, key_val);
}

}  // namespace AFIHAL
