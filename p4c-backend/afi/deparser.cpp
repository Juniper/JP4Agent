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

#include "backend.h"
#include "deparser.h"

namespace AFI {

void ConvertDeparser::convertDeparserBody(const IR::Vector<IR::StatOrDecl>* body,
                                          Util::JsonArray* result) {
    conv->simpleExpressionsOnly = true;
    for (auto s : *body) {
        if (auto block = s->to<IR::BlockStatement>()) {
            convertDeparserBody(&block->components, result);
            continue;
        } else if (s->is<IR::ReturnStatement>() || s->is<IR::ExitStatement>()) {
            break;
        } else if (s->is<IR::EmptyStatement>()) {
            continue;
        } else if (s->is<IR::MethodCallStatement>()) {
            auto mc = s->to<IR::MethodCallStatement>()->methodCall;
            auto mi = P4::MethodInstance::resolve(mc,
                    refMap, typeMap);
            if (mi->is<P4::ExternMethod>()) {
                auto em = mi->to<P4::ExternMethod>();
                if (em->originalExternType->name.name == backend->getCoreLibrary().packetOut.name) {
                    if (em->method->name.name == backend->getCoreLibrary().packetOut.emit.name) {
                        BUG_CHECK(mc->arguments->size() == 1,
                                  "Expected exactly 1 argument for %1%", mc);
                        auto arg = mc->arguments->at(0);
                        auto type = typeMap->getType(arg, true);
                        if (type->is<IR::Type_Stack>()) {
                            // This branch is in fact never taken, because
                            // arrays are expanded into elements.
                            int size = type->to<IR::Type_Stack>()->getSize();
                            for (int i=0; i < size; i++) {
                                auto j = conv->convert(arg);
                                auto e = j->to<Util::JsonObject>()->get("value");
                                BUG_CHECK(e->is<Util::JsonValue>(),
                                          "%1%: Expected a Json value", e->toString());
                                cstring ref = e->to<Util::JsonValue>()->getString();
                                ref += "[" + Util::toString(i) + "]";
                                result->append(ref);
                            }
                        } else if (type->is<IR::Type_Header>()) {
                            auto j = conv->convert(arg);
                            auto val = j->to<Util::JsonObject>()->get("value");
                            result->append(val);
                        } else {
                            ::error("%1%: emit only supports header and stack arguments, not %2%",
                                    arg, type);
                        }
                    }
                    continue;
                }
            }
        }
        ::error("%1%: not supported with a deparser on this target", s);
    }
    conv->simpleExpressionsOnly = false;
}

Util::IJson* ConvertDeparser::convertDeparser(const IR::P4Control* ctrl) {
    auto result = new Util::JsonObject();
    result->emplace("name", "deparser");  // at least in simple_router this name is hardwired
    result->emplace("id", nextId("deparser"));
    result->emplace_non_null("source_info", ctrl->sourceInfoJsonObj());
    auto order = mkArrayField(result, "order");
    convertDeparserBody(&ctrl->body->components, order);
    return result;
}

bool ConvertDeparser::preorder(const IR::PackageBlock* block) {
    for (auto it : block->constantValue) {
        if (it.second->is<IR::ControlBlock>()) {
            visit(it.second->getNode());
        }
    }
    return false;
}

bool ConvertDeparser::preorder(const IR::ControlBlock* block) {
    auto bt = backend->deparser_controls.find(block->container->name);
    if (bt == backend->deparser_controls.end()) {
        return false;
    }
    const IR::P4Control* cont = block->container;
    auto deparserJson = convertDeparser(cont);
    json->deparsers->append(deparserJson);
    return false;
}

}  // namespace AFI
