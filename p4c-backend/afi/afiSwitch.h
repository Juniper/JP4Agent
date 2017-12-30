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


#ifndef _BACKENDS_AFI_SIMPLESWITCH_H_
#define _BACKENDS_AFI_SIMPLESWITCH_H_

#include <algorithm>
#include <cstring>
#include "frontends/p4/fromv1.0/v1model.h"
#include "sharedActionSelectorCheck.h"

namespace AFI {

class Backend;

}  // namespace AFI

namespace P4V1 {

class SimpleSwitch {
    AFI::Backend* backend;
    V1Model&       v1model;


 protected:
    void addToFieldList(const IR::Expression* expr, Util::JsonArray* fl);
    int createFieldList(const IR::Expression* expr, cstring group,
                        cstring listName, Util::JsonArray* field_lists);
    cstring convertHashAlgorithm(cstring algorithm);
    cstring createCalculation(cstring algo, const IR::Expression* fields,
                              Util::JsonArray* calculations, const IR::Node* node);

 public:
    void modelError(const char* format, const IR::Node* place) const;
    void convertExternObjects(Util::JsonArray *result, const P4::ExternMethod *em,
                              const IR::MethodCallExpression *mc, const IR::StatOrDecl *s);
    void convertExternFunctions(Util::JsonArray *result, const P4::ExternFunction *ef,
                                const IR::MethodCallExpression *mc, const IR::StatOrDecl* s);
    void convertExternInstances(const IR::Declaration *c,
                                const IR::ExternBlock* eb, Util::JsonArray* action_profiles,
                                AFI::SharedActionSelectorCheck& selector_check);
    void convertChecksum(const IR::BlockStatement* body, Util::JsonArray* checksums,
                         Util::JsonArray* calculations, bool verify);

    void setPipelineControls(const IR::ToplevelBlock* blk, std::set<cstring>* controls,
                             std::map<cstring, cstring>* map);
    void setNonPipelineControls(const IR::ToplevelBlock* blk, std::set<cstring>* controls);
    void setUpdateChecksumControls(const IR::ToplevelBlock* blk, std::set<cstring>* controls);
    void setVerifyChecksumControls(const IR::ToplevelBlock* blk, std::set<cstring>* controls);
    void setDeparserControls(const IR::ToplevelBlock* blk, std::set<cstring>* controls);

    const IR::P4Control* getIngress(const IR::ToplevelBlock* blk);
    const IR::P4Control* getEgress(const IR::ToplevelBlock* blk);
    const IR::P4Parser*  getParser(const IR::ToplevelBlock* blk);

    explicit SimpleSwitch(AFI::Backend* backend) :
        backend(backend), v1model(V1Model::instance)
    { CHECK_NULL(backend); }
};

}  // namespace P4V1

#endif /* _BACKENDS_AFI_SIMPLESWITCH_H_ */
