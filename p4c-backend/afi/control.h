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

#ifndef _BACKENDS_AFI_CONTROL_H_
#define _BACKENDS_AFI_CONTROL_H_

#include "ir/ir.h"
#include "lib/json.h"
#include "analyzer.h"
#include "frontends/p4/coreLibrary.h"
#include "frontends/p4/typeMap.h"
#include "frontends/p4/typeChecking/typeChecker.h"
#include "frontends/common/resolveReferences/referenceMap.h"
#include "midend/convertEnums.h"
#include "expression.h"
#include "helpers.h"
#include "sharedActionSelectorCheck.h"
#include "afiSwitch.h"

namespace AFI {

class ControlConverter : public Inspector {
    Backend*               backend;
    P4::ReferenceMap*      refMap;
    P4::TypeMap*           typeMap;
    ExpressionConverter*   conv;
    AFI::JsonObjects*     json;

 protected:
    Util::IJson* convertTable(const CFG::TableNode* node,
                              Util::JsonArray* action_profiles);
    void convertTableEntries(const IR::P4Table *table, Util::JsonObject *jsonTable);
    cstring getKeyMatchType(const IR::KeyElement *ke);
    /// Return 'true' if the table is 'simple'
    bool handleTableImplementation(const IR::Property* implementation, const IR::Key* key,
                                   Util::JsonObject* table, Util::JsonArray* action_profiles);
    Util::IJson* convertIf(const CFG::IfNode* node, cstring prefix);
    Util::IJson* convertControl(const IR::ControlBlock* block, cstring name,
                                Util::JsonArray *counters, Util::JsonArray* meters,
                                Util::JsonArray* registers);

 public:
    bool preorder(const IR::PackageBlock* b) override;
    bool preorder(const IR::ControlBlock* b) override;

    explicit ControlConverter(Backend *backend) : backend(backend),
        refMap(backend->getRefMap()), typeMap(backend->getTypeMap()),
        conv(backend->getExpressionConverter()), json(backend->json)
    { setName("Control"); }
};

class ChecksumConverter : public Inspector {
    Backend* backend;
 public:
    bool preorder(const IR::PackageBlock* b) override;
    bool preorder(const IR::ControlBlock* b) override;
    explicit ChecksumConverter(Backend *backend) : backend(backend)
    { setName("UpdateChecksum"); }
};

class ConvertControl final : public PassManager {
 public:
    explicit ConvertControl(Backend *backend) {
        passes.push_back(new ControlConverter(backend));
        passes.push_back(new ChecksumConverter(backend));
        setName("ConvertControl");
    }
};

}  // namespace AFI

#endif  /* _BACKENDS_AFI_CONTROL_H_ */
