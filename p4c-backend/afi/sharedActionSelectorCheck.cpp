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


#include "sharedActionSelectorCheck.h"

#include <algorithm>

namespace AFI {

const SelectorInput*
SharedActionSelectorCheck::get_selector_input(const IR::Declaration_Instance* selector) {
    auto it = selector_input_map.find(selector);
    if (it == selector_input_map.end()) return nullptr;  // selector never used
    return &it->second;
}

bool
SharedActionSelectorCheck::preorder(const IR::P4Table* table) {
    auto implementation = table->properties->getProperty("implementation");
    if (implementation == nullptr) return false;
    if (!implementation->value->is<IR::ExpressionValue>()) {
        ::error("%1%: expected expression for property", implementation);
        return false;
    }
    auto propv = implementation->value->to<IR::ExpressionValue>();
    if (!propv->expression->is<IR::PathExpression>()) return false;
    auto pathe = propv->expression->to<IR::PathExpression>();
    auto decl = refMap->getDeclaration(pathe->path, true);
    if (!decl->is<IR::Declaration_Instance>()) {
        ::error("%1%: expected a reference to an instance", pathe);
        return false;
    }
    auto dcltype = typeMap->getType(pathe, true);
    if (!dcltype->is<IR::Type_Extern>()) {
        ::error("%1%: unexpected type for implementation", dcltype);
        return false;
    }
    auto type_extern_name = dcltype->to<IR::Type_Extern>()->name;
    if (type_extern_name != AFI::TableImplementation::actionSelectorName) return false;

    auto key = table->getKey();
    SelectorInput input;
    for (auto ke : key->keyElements) {
        auto mt = refMap->getDeclaration(ke->matchType->path, true)->to<IR::Declaration_ID>();
        BUG_CHECK(mt != nullptr, "%1%: could not find declaration", ke->matchType);
        if (mt->name.name != AFI::MatchImplementation::selectorMatchTypeName) continue;
        input.push_back(ke->expression);
    }
    auto decl_instance = decl->to<IR::Declaration_Instance>();
    auto it = selector_input_map.find(decl_instance);
    if (it == selector_input_map.end()) {
        selector_input_map[decl_instance] = input;
        return false;
    }
    // returns true if inputs are the same, false otherwise
    auto cmp_inputs = [](const SelectorInput &i1, const SelectorInput &i2) {
        if (i1.size() != i2.size()) return false;
        return std::equal(i1.begin(), i1.end(), i2.begin(), checkSameKeyExpr);
    };

    if (!cmp_inputs(it->second, input)) {
        ::error(
                "Action selector '%1%' is used by multiple tables with different selector inputs",
                decl);
    }

    return false;
}

}  // namespace AFI
