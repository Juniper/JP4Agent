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


#ifndef _BACKENDS_AFI_PARSER_H_
#define _BACKENDS_AFI_PARSER_H_

#include "ir/ir.h"
#include "lib/json.h"
#include "frontends/p4/typeMap.h"
#include "frontends/common/resolveReferences/referenceMap.h"
#include "backend.h"
#include "expression.h"

namespace AFI {

class JsonObjects;

class ParserConverter : public Inspector {
    Backend* backend;
    P4::ReferenceMap*    refMap;
    P4::TypeMap*         typeMap;
    AFI::JsonObjects*   json;
    ExpressionConverter* conv;
    P4::P4CoreLibrary&   corelib;
    std::map<const IR::P4Parser*, Util::IJson*> parser_map;
    std::map<const IR::ParserState*, Util::IJson*> state_map;
    std::vector<Util::IJson*> context;

 protected:
    void convertSimpleKey(const IR::Expression* keySet, mpz_class& value, mpz_class& mask) const;
    unsigned combine(const IR::Expression* keySet, const IR::ListExpression* select,
                     mpz_class& value, mpz_class& mask) const;
    Util::IJson* stateName(IR::ID state);
    Util::IJson* toJson(const IR::P4Parser* cont);
    Util::IJson* toJson(const IR::ParserState* state);
    Util::IJson* convertParserStatement(const IR::StatOrDecl* stat);
    Util::IJson* convertSelectKey(const IR::SelectExpression* expr);
    Util::IJson* convertPathExpression(const IR::PathExpression* expr);
    Util::IJson* createDefaultTransition();
    std::vector<Util::IJson*> convertSelectExpression(const IR::SelectExpression* expr);

 public:
    bool preorder(const IR::P4Parser* p) override;
    bool preorder(const IR::PackageBlock* b) override;
    explicit ParserConverter(Backend* backend) : backend(backend), refMap(backend->getRefMap()),
    typeMap(backend->getTypeMap()), json(backend->json),
    conv(backend->getExpressionConverter()),
    corelib(P4::P4CoreLibrary::instance) { setName("ParserConverter"); }
};

class ConvertParser final : public PassManager {
 public:
    explicit ConvertParser(Backend* backend) {
        passes.push_back(new ParserConverter(backend));
        setName("ConvertParser");
    }
};

}  // namespace AFI

#endif  /* _BACKENDS_AFI_PARSER_H_ */
