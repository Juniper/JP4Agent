//
// Juniper P4 Agent
//
/// @file  AfiTreeEntry.cpp
/// @brief Afi tree entry
//
// Created by Sandesh Kumar Sodhi, February 2018
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

#include "AfiTreeEntry.h"
#include <jaegertracing/Tracer.h>
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
AfiTreeEntry::description(std::ostream &os) const
{
    os << "_________ AfiTreeEntry _______" << std::endl;
    return os;
}

AfiTreeEntry::AfiTreeEntry(const AfiJsonResource &jsonRes) : AfiObject(jsonRes)
{
    // FIXME magic number 5000
    char bytes_decoded[5000];
    memset(bytes_decoded, 0, sizeof(bytes_decoded));
    int num_decoded_bytes =
        base64_decode(jsonRes.objStr(), bytes_decoded, 5000);

    Log(DEBUG) << "bytes_decoded: " << bytes_decoded;
    Log(DEBUG) << "num_decoded_bytes: " << num_decoded_bytes;

    _treeEntry.ParseFromArray(bytes_decoded, num_decoded_bytes);

    Log(DEBUG) << "tree.ByteSize(): " << _treeEntry.ByteSize();

    ::ywrapper::StringValue entry_name = _treeEntry.name();
    Log(DEBUG) << "entry_name: " << entry_name.value();

    ::ywrapper::StringValue parent_name = _treeEntry.parent_name();
    Log(DEBUG) << "parent_name: " << parent_name.value();

    ::ywrapper::StringValue target_afi_object = _treeEntry.target_afi_object();
    Log(DEBUG) << "target_afi_object: " << target_afi_object.value();

    // ::ywrapper::StringValue prefix = _treeEntry.name();
    // Log(DEBUG) << "prefix: " << prefix.value();

    ::ywrapper::UintValue prefix_length = _treeEntry.prefix_length();
    Log(DEBUG) << "prefix length: " << prefix_length.value();

    std::stringstream es;
    es << entry_name.value();
    opentracing::string_view name("AFI:AFITreeEntry:Name");
    opentracing::string_view name_val(es.str());
    span->SetBaggageItem(name, name_val);

    std::stringstream ps;
    ps << parent_name.value();
    opentracing::string_view parent("AFI:AFITreeEntry:Parent Name");
    opentracing::string_view parent_val(ps.str());
    span->SetBaggageItem(parent, parent_val);

    std::stringstream as;
    as << target_afi_object.value();
    opentracing::string_view afi("AFI:AFITreeEntry:Target AFI Object");
    opentracing::string_view afi_val(as.str());
    span->SetBaggageItem(afi, afi_val);
}

}  // namespace AFIHAL
