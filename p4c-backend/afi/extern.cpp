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


#include "extern.h"

namespace AFI {

Util::JsonArray*
Extern::addExternAttributes(const IR::Declaration_Instance* di,
                            const IR::ExternBlock* block) {
    auto attributes = new Util::JsonArray();
    auto paramIt = block->getConstructorParameters()->parameters.begin();
    for (auto arg : *di->arguments) {
        auto name = arg->toString();
        if (arg->is<IR::Constant>()) {
            auto cVal = arg->to<IR::Constant>();
            if (arg->type->is<IR::Type_Bits>()) {
                json->add_extern_attribute(name, "hexstr",
                                           stringRepr(cVal->value), attributes);
            } else {
                BUG("%1%: unhandled constant constructor param", cVal->toString());
            }
        } else if (arg->is<IR::Declaration_ID>()) {
            auto declId = arg->to<IR::Declaration_ID>();
            json->add_extern_attribute(name, "string", declId->toString(), attributes);
        } else if (arg->type->is<IR::Type_Enum>()) {
            json->add_extern_attribute(name, "string", arg->toString(), attributes);
        } else {
            BUG("%1%: unknown constructor param type", arg->type);
        }
        ++paramIt;
    }
    return attributes;
}

/// generate extern_instances from instance declarations.
bool Extern::preorder(const IR::Declaration_Instance* decl) {
    LOG1("ExternConv Visiting ..." << dbp(decl));
    // Declaration_Instance -> P4Control -> ControlBlock
    auto grandparent = getContext()->parent->node;
    if (grandparent->is<IR::ControlBlock>()) {
        auto block = grandparent->to<IR::ControlBlock>()->getValue(decl);
        CHECK_NULL(block);
        if (block->is<IR::ExternBlock>()) {
            auto externBlock = block->to<IR::ExternBlock>();
            auto name = decl->name;
            auto type = "";
            if (decl->type->is<IR::Type_Specialized>())
                type = decl->type->to<IR::Type_Specialized>()->baseType->toString();
            else if (decl->type->is<IR::Type_Name>())
                type = decl->type->to<IR::Type_Name>()->path->name.toString();
            else
                P4C_UNIMPLEMENTED("extern support for %1%", decl);
            auto attributes = addExternAttributes(decl, externBlock);
            json->add_extern(name, type, attributes);
        } else {
            BUG("%1% Unsupported block type for extern generation.", block->toString());
        }
    }
    return false;
}

/// Custom visitor to enable traversal on other blocks
bool Extern::preorder(const IR::PackageBlock *block) {
    if (backend->target != Target::PORTABLE)
        return false;

    for (auto it : block->constantValue) {
        if (it.second->is<IR::Block>()) {
            visit(it.second->getNode());
        }
    }
    return false;
}


}  // namespace AFI
