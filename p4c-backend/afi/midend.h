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


#ifndef _BACKENDS_AFI_MIDEND_H_
#define _BACKENDS_AFI_MIDEND_H_

#include "ir/ir.h"
#include "frontends/common/options.h"
#include "frontends/p4/evaluator/evaluator.h"
#include "midend/actionsInlining.h"
#include "midend/inlining.h"
#include "midend/convertEnums.h"
#include "helpers.h"
#include "options.h"

namespace AFI {

class MidEnd : public PassManager {
    P4::InlineWorkList controlsToInline;
    P4::ActionsInlineList actionsToInline;

 public:
    // These will be accurate when the mid-end completes evaluation
    P4::ReferenceMap    refMap;
    P4::TypeMap         typeMap;
    const IR::ToplevelBlock   *toplevel = nullptr;
    P4::ConvertEnums::EnumMapping enumMap;

    explicit MidEnd(AFIOptions& options);
    const IR::ToplevelBlock* process(const IR::P4Program *&program) {
        program = program->apply(*this);
        return toplevel; }
};

}  // namespace AFI

#endif /* _BACKENDS_AFI_MIDEND_H_ */
