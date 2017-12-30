//
// Juniper P4 compiler 
//
/// @file  backend.cpp
/// @brief P4 compiler for Juniper AFI
//
// Created by Sandesh Kumar Sodhi, December, 2017
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

#ifndef _BACKENDS_AFI_HEADER_H_
#define _BACKENDS_AFI_HEADER_H_

#include <list>
#include "ir/ir.h"
#include "lib/json.h"
#include "frontends/p4/typeMap.h"
#include "frontends/common/resolveReferences/referenceMap.h"
#include "helpers.h"
#include "JsonObjects.h"

namespace AFI {

class Backend;

class ConvertHeaders : public Inspector {
    Backend*             backend;
    cstring              scalarsName;
    cstring              scalarsTypeName;
    P4::ReferenceMap*    refMap;
    P4::TypeMap*         typeMap;
    JsonObjects*         json;
    std::set<cstring>    visitedHeaders;

    const unsigned       boolWidth = 1;    // convert booleans to 1-bit integers
    const unsigned       errorWidth = 32;  // convert errors to 32-bit integers
    unsigned             scalars_width = 0;

 protected:
    Util::JsonArray* pushNewArray(Util::JsonArray* parent);
    void addHeaderType(const IR::Type_StructLike* st);
    void addHeaderField(const cstring& header, const cstring& name, int size, bool is_signed);

 public:
    void addTypesAndInstances(const IR::Type_StructLike* type, bool meta);
    void addHeaderStacks(const IR::Type_Struct* type);
    bool isHeaders(const IR::Type_StructLike* st);

    Visitor::profile_t init_apply(const IR::Node* node) override;
    void end_apply(const IR::Node* node) override;

    bool preorder(const IR::PackageBlock* b) override;
    bool preorder(const IR::Parameter* param) override;

    ConvertHeaders(Backend* backend, cstring scalarsName);
};

}  // namespace AFI

#endif /* _BACKENDS_AFI_HEADER_H_ */
