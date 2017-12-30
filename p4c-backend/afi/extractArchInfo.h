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

#ifndef _BACKENDS_AFI_EXTRACTARCHINFO_H_
#define _BACKENDS_AFI_EXTRACTARCHINFO_H_

#include "ir/ir.h"
#include "ir/visitor.h"
#include "frontends/p4/typeMap.h"
#include "frontends/common/model.h"
#include "frontends/p4/methodInstance.h"
#include "portableSwitch.h"

namespace AFI {

class ExtractArchInfo : public Inspector {
 private:
    P4::TypeMap*       typeMap;
    P4::PortableModel& portable_model;
 public:
    explicit ExtractArchInfo(P4::TypeMap *typeMap)
        : typeMap(typeMap), portable_model(P4::PortableModel::instance) {
        setName("ExtractArchInfo");
    }
 public:
    bool preorder(const IR::Type_Control *node) override;
    bool preorder(const IR::Type_Parser *node) override;
    bool preorder(const IR::Type_Extern *node) override;
    bool preorder(const IR::Type_Package *node) override;
    bool preorder(const IR::P4Program* program) override;
    bool preorder(const IR::Declaration_MatchKind* kind) override;
};

}  // namespace AFI

#endif  /* _BACKENDS_AFI_EXTRACTARCHINFO_H_ */
