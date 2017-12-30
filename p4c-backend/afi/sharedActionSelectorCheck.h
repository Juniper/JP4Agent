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

#ifndef _BACKENDS_AFI_SHAREDACTIONSELECTORCHECK_H_
#define _BACKENDS_AFI_SHAREDACTIONSELECTORCHECK_H_

#include "ir/ir.h"
#include "lib/json.h"
#include "frontends/p4/coreLibrary.h"
#include "frontends/p4/frontend.h"
#include "frontends/p4/typeMap.h"
#include "frontends/p4/typeChecking/typeChecker.h"
#include "frontends/common/resolveReferences/referenceMap.h"
#include "helpers.h"

namespace AFI {

using SelectorInput = std::vector<const IR::Expression *>;

// This pass makes sure that when several match tables share a selector, they use the same input for
// the selection algorithm. This is because AFI  considers that the selection key is part of the
// action_selector while v1model.p4 considers that it belongs to the table match key definition.
class SharedActionSelectorCheck : public Inspector {
    P4::ReferenceMap* refMap;
    P4::TypeMap*      typeMap;
    std::map<const IR::Declaration_Instance *, SelectorInput> selector_input_map{};

  static bool checkSameKeyExpr(const IR::Expression* expr0, const IR::Expression* expr1) {
      if (expr0->node_type_name() != expr1->node_type_name())
          return false;
      if (auto pe0 = expr0->to<IR::PathExpression>()) {
          auto pe1 = expr1->to<IR::PathExpression>();
          return pe0->path->name == pe1->path->name &&
              pe0->path->absolute == pe1->path->absolute;
      } else if (auto mem0 = expr0->to<IR::Member>()) {
          auto mem1 = expr1->to<IR::Member>();
          return checkSameKeyExpr(mem0->expr, mem1->expr) && mem0->member == mem1->member;
      } else if (auto l0 = expr0->to<IR::Literal>()) {
          auto l1 = expr1->to<IR::Literal>();
          return *l0 == *l1;
      } else if (auto ai0 = expr0->to<IR::ArrayIndex>()) {
          auto ai1 = expr1->to<IR::ArrayIndex>();
          return checkSameKeyExpr(ai0->left, ai1->left) && checkSameKeyExpr(ai0->right, ai1->right);
      }
      return false;
  }

 public:
    explicit SharedActionSelectorCheck(P4::ReferenceMap* refMap, P4::TypeMap* typeMap) :
        refMap(refMap), typeMap(typeMap) {}

    const SelectorInput* get_selector_input(const IR::Declaration_Instance* selector);
    bool preorder(const IR::P4Table* table) override;
};

}  // namespace AFI

#endif  /* _BACKENDS_AFI_SHAREDACTIONSELECTORCHECK_H_ */
