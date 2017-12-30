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


#include "extractArchInfo.h"

namespace AFI {

using ::Model::Type_Model;
using ::Model::Param_Model;

bool ExtractArchInfo::preorder(const IR::P4Program* program) {
    for (auto decl : program->declarations) {
        if (decl->is<IR::Type_Package>() || decl->is<IR::Type_Extern>() ||
            decl->is<IR::Declaration_MatchKind>()) {
            visit(decl);
        }
    }
    return true;
}

bool ExtractArchInfo::preorder(const IR::Type_Control *node) {
    if (portable_model.controls.size() != 0) {
        P4::Control_Model *control_m = portable_model.controls.back();
        uint32_t paramCounter = 0;
        for (auto param : node->applyParams->parameters) {
            Type_Model paramTypeModel(param->type->toString());
            Param_Model newParam(param->toString(), paramTypeModel, paramCounter++);
            control_m->elems.push_back(newParam);
        }
    }
    LOG3("...control [" << node << " ]");
    return false;
}

bool ExtractArchInfo::preorder(const IR::Type_Parser *node) {
    if (portable_model.parsers.size() != 0) {
        P4::Parser_Model *parser_m = portable_model.parsers.back();
        uint32_t paramCounter = 0;
        for (auto param : *node->applyParams->getEnumerator()) {
            Type_Model paramTypeModel(param->type->toString());
            Param_Model newParam(param->toString(), paramTypeModel, paramCounter++);
            parser_m->elems.push_back(newParam);
            LOG3("...... parser params [ " << param << " ]");
        }
    }
    return false;
}

bool ExtractArchInfo::preorder(const IR::Type_Package *node) {
    for (auto p : node->getConstructorParameters()->parameters) {
        LOG1("package constructor param: " << p);
        auto ptype = typeMap->getType(p);
        if (ptype == nullptr)
            continue;
        if (ptype->is<IR::P4Parser>()) {
            LOG3("new parser model: " << p->toString());
            portable_model.parsers.push_back(new P4::Parser_Model(p->toString()));
            visit(ptype->to<IR::P4Parser>()->type);
        } else if (ptype->is<IR::P4Control>()) {
            LOG3("new control model: " << p->toString());
            portable_model.controls.push_back(new P4::Control_Model(p->toString()));
            visit(ptype->to<IR::P4Control>()->type);
        }
    }
    LOG3("...package [ " << node << " ]");
    return false;
}

bool ExtractArchInfo::preorder(const IR::Type_Extern *node) {
    P4::Extern_Model *extern_m = new P4::Extern_Model(node->toString());
    for (auto method : node->methods) {
        uint32_t paramCounter = 0;
        P4::Method_Model method_m(method->toString());
        for (auto param : *method->type->parameters->getEnumerator()) {
            Type_Model paramTypeModel(param->type->toString());
            Param_Model newParam(param->toString(), paramTypeModel,
                                 paramCounter++);
            method_m.elems.push_back(newParam);
        }
        extern_m->elems.push_back(method_m);
    }
    portable_model.externs.push_back(extern_m);
    LOG3("...extern [ " << node << " ]");
    return false;
}

bool ExtractArchInfo::preorder(const IR::Declaration_MatchKind* kind) {
    for (auto member : kind->members) {
        Type_Model *match_kind = new Type_Model(member->toString());
        portable_model.match_kinds.push_back(match_kind);
        LOG3("... match kind " << match_kind);
    }
    return false;
}

}  // namespace AFI
