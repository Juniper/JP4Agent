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

#include "action.h"
#include "backend.h"
#include "control.h"
#include "deparser.h"
#include "errorcode.h"
#include "expression.h"
#include "extern.h"
#include "frontends/common/constantFolding.h"
#include "frontends/p4/evaluator/evaluator.h"
#include "frontends/p4/methodInstance.h"
#include "frontends/p4/simplify.h"
#include "frontends/p4/typeChecking/typeChecker.h"
#include "frontends/p4/unusedDeclarations.h"
#include "midend/actionSynthesis.h"
#include "midend/removeLeftSlices.h"
#include "lower.h"
#include "header.h"
#include "parser.h"
#include "JsonObjects.h"
#include "extractArchInfo.h"

namespace AFI {

/**
This class implements a policy suitable for the SynthesizeActions pass.
The policy is: do not synthesize actions for the controls whose names
are in the specified set.
For example, we expect that the code in the deparser will not use any
tables or actions.
*/
class SkipControls : public P4::ActionSynthesisPolicy {
    // set of controls where actions are not synthesized
    const std::set<cstring> *skip;

 public:
    explicit SkipControls(const std::set<cstring> *skip) : skip(skip) { CHECK_NULL(skip); }
    bool convert(const IR::P4Control* control) const {
        if (skip->find(control->name) != skip->end())
            return false;
        return true;
    }
};

/**
This class implements a policy suitable for the RemoveComplexExpression pass.
The policy is: only remove complex expression for the controls whose names
are in the specified set.
For example, we expect that the code in ingress and egress will have complex
expression removed.
*/
class ProcessControls : public AFI::RemoveComplexExpressionsPolicy {
    const std::set<cstring> *process;

 public:
    explicit ProcessControls(const std::set<cstring> *process) : process(process) {
        CHECK_NULL(process);
    }
    bool convert(const IR::P4Control* control) const {
        if (process->find(control->name) != process->end())
            return true;
        return false;
    }
};

void
Backend::process(const IR::ToplevelBlock* tlb, AFIOptions& options) {
    CHECK_NULL(tlb);
    auto evaluator = new P4::EvaluatorPass(refMap, typeMap);
    if (tlb->getMain() == nullptr)
        return;  // no main

    if (options.arch != Target::UNKNOWN)
        target = options.arch;

    if (target == Target::SIMPLE) {
        simpleSwitch->setPipelineControls(tlb, &pipeline_controls, &pipeline_namemap);
        simpleSwitch->setNonPipelineControls(tlb, &non_pipeline_controls);
        simpleSwitch->setUpdateChecksumControls(tlb, &update_checksum_controls);
        simpleSwitch->setVerifyChecksumControls(tlb, &verify_checksum_controls);
        simpleSwitch->setDeparserControls(tlb, &deparser_controls);
    } else if (target == Target::PORTABLE) {
        P4C_UNIMPLEMENTED("PSA architecture is not yet implemented");
    }

    addPasses({
        new P4::SynthesizeActions(refMap, typeMap, new SkipControls(&non_pipeline_controls)),
        new P4::MoveActionsToTables(refMap, typeMap),
        new P4::TypeChecking(refMap, typeMap),
        new P4::SimplifyControlFlow(refMap, typeMap),
        new LowerExpressions(typeMap),
        new P4::ConstantFolding(refMap, typeMap, false),
        new P4::TypeChecking(refMap, typeMap),
        new RemoveComplexExpressions(refMap, typeMap, new ProcessControls(&pipeline_controls)),
        new P4::SimplifyControlFlow(refMap, typeMap),
        new P4::RemoveAllUnusedDeclarations(refMap),
        new DiscoverStructure(&structure),
        new ErrorCodesVisitor(&errorCodesMap),
        new ExtractArchInfo(typeMap),
        evaluator,
        new VisitFunctor([this, evaluator]() { toplevel = evaluator->getToplevelBlock(); }),
    });
    tlb->getProgram()->apply(*this);
}

/// AFI Backend that takes the top level block and converts it to a JsonObject
void Backend::convert(AFIOptions& options) {
    jsonTop.emplace("program", options.file);
    jsonTop.emplace("__meta__", json->meta);
    jsonTop.emplace("header_types", json->header_types);
    jsonTop.emplace("headers", json->headers);
    jsonTop.emplace("header_stacks", json->header_stacks);
    jsonTop.emplace("header_union_types", json->header_union_types);
    jsonTop.emplace("header_unions", json->header_unions);
    jsonTop.emplace("header_union_stacks", json->header_union_stacks);
    field_lists = mkArrayField(&jsonTop, "field_lists");
    jsonTop.emplace("errors", json->errors);
    jsonTop.emplace("enums", json->enums);
    jsonTop.emplace("parsers", json->parsers);
    jsonTop.emplace("deparsers", json->deparsers);
    meter_arrays = mkArrayField(&jsonTop, "meter_arrays");
    counters = mkArrayField(&jsonTop, "counter_arrays");
    register_arrays = mkArrayField(&jsonTop, "register_arrays");
    jsonTop.emplace("calculations", json->calculations);
    learn_lists = mkArrayField(&jsonTop, "learn_lists");
    AFI::nextId("learn_lists");
    jsonTop.emplace("actions", json->actions);
    jsonTop.emplace("pipelines", json->pipelines);
    jsonTop.emplace("checksums", json->checksums);
    force_arith = mkArrayField(&jsonTop, "force_arith");
    jsonTop.emplace("extern_instances", json->externs);
    jsonTop.emplace("field_aliases", json->field_aliases);

    json->add_program_info(options.file);
    json->add_meta_info();

    // convert all enums to json
    for (const auto &pEnum : *enumMap) {
        auto name = pEnum.first->getName();
        for (const auto &pEntry : *pEnum.second) {
            json->add_enum(name, pEntry.first, pEntry.second);
        }
    }
    if (::errorCount() > 0)
        return;

    /// generate error types
    for (const auto &p : errorCodesMap) {
        auto name = p.first->toString();
        auto type = p.second;
        json->add_error(name, type);
    }

    cstring scalarsName = refMap->newName("scalars");

    // This visitor is used in multiple passes to convert expression to json
    conv = new ExpressionConverter(this, scalarsName);

    // if (psa) tlb->apply(new ConvertExterns());
    PassManager codegen_passes = {
        new ConvertHeaders(this, scalarsName),
        new ConvertExterns(this),  // only run when target == PSA
        new ConvertParser(this),
        new ConvertActions(this),
        new ConvertControl(this),
        new ConvertDeparser(this),
    };

    codegen_passes.setName("CodeGen");
    CHECK_NULL(toplevel);
    auto main = toplevel->getMain();
    if (main == nullptr)
        return;
    main->apply(codegen_passes);
}

bool Backend::isStandardMetadataParameter(const IR::Parameter* param) {
    if (target == Target::SIMPLE) {
        auto parser = simpleSwitch->getParser(getToplevelBlock());
        auto params = parser->getApplyParameters();
        if (params->size() != 4) {
            simpleSwitch->modelError("%1%: Expected 4 parameter for parser", parser);
            return false;
        }
        if (params->parameters.at(3) == param)
            return true;

        auto ingress = simpleSwitch->getIngress(getToplevelBlock());
        params = ingress->getApplyParameters();
        if (params->size() != 3) {
            simpleSwitch->modelError("%1%: Expected 3 parameter for ingress", ingress);
            return false;
        }
        if (params->parameters.at(2) == param)
            return true;

        auto egress = simpleSwitch->getEgress(getToplevelBlock());
        params = egress->getApplyParameters();
        if (params->size() != 3) {
            simpleSwitch->modelError("%1%: Expected 3 parameter for egress", egress);
            return false;
        }
        if (params->parameters.at(2) == param)
            return true;

        return false;
    } else {
        P4C_UNIMPLEMENTED("PSA architecture is not yet implemented");
    }
}

}  // namespace AFI
