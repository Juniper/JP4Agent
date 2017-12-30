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


#ifndef _BACKENDS_AFI_DEPARSER_H_
#define _BACKENDS_AFI_DEPARSER_H_

#include "ir/ir.h"
#include "lib/json.h"
#include "frontends/p4/typeMap.h"
#include "frontends/common/resolveReferences/referenceMap.h"
#include "expression.h"
#include "backend.h"

namespace AFI {

class ConvertDeparser : public Inspector {
    Backend*               backend;
    P4::ReferenceMap*      refMap;
    P4::TypeMap*           typeMap;
    AFI::JsonObjects*     json;
    ExpressionConverter*   conv;

 protected:
    Util::IJson* convertDeparser(const IR::P4Control* ctrl);
    void convertDeparserBody(const IR::Vector<IR::StatOrDecl>* body, Util::JsonArray* result);
 public:
    bool preorder(const IR::PackageBlock* block);
    bool preorder(const IR::ControlBlock* ctrl);

    explicit ConvertDeparser(Backend* backend) :
        backend(backend), refMap(backend->getRefMap()),
        typeMap(backend->getTypeMap()),
        json(backend->json),
        conv(backend->getExpressionConverter()){ setName("ConvertDeparser"); }
};

}  // namespace AFI

#endif  /* _BACKENDS_AFI_DEPARSER_H_ */
