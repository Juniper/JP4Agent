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


#include "metermap.h"

namespace AFI {

/**
 * @returns direct meter information from the direct meter map
 */
DirectMeterMap::DirectMeterInfo* DirectMeterMap::createInfo(const IR::IDeclaration* meter) {
    auto prev = ::get(directMeter, meter);
    BUG_CHECK(prev == nullptr, "Already created");
    auto result = new DirectMeterMap::DirectMeterInfo();
    directMeter.emplace(meter, result);
    return result;
}

DirectMeterMap::DirectMeterInfo* DirectMeterMap::getInfo(const IR::IDeclaration* meter) {
    return ::get(directMeter, meter);
}

/**
 * Set the table that a direct meter is attached to.
 */
void DirectMeterMap::setTable(const IR::IDeclaration* meter, const IR::P4Table* table) {
    auto info = getInfo(meter);
    CHECK_NULL(info);
    if (info->table != nullptr)
        ::error("%1%: Direct meters cannot be attached to multiple tables %2% and %3%",
                meter, table, info->table);
    info->table = table;
}

/**
 * Helper function to check if two expressions are syntactically identical
 */
static bool checkSame(const IR::Expression* expr0, const IR::Expression* expr1) {
    if (expr0->node_type_name() != expr1->node_type_name())
        return false;
    if (auto pe0 = expr0->to<IR::PathExpression>()) {
        auto pe1 = expr1->to<IR::PathExpression>();
        return pe0->path->name == pe1->path->name &&
               pe0->path->absolute == pe1->path->absolute;
    } else if (auto mem0 = expr0->to<IR::Member>()) {
        auto mem1 = expr1->to<IR::Member>();
        return checkSame(mem0->expr, mem1->expr) && mem0->member == mem1->member;
    }
    BUG("%1%: unexpected expression for meter destination", expr0);
}

/**
 * Set the destination that a meter is attached to??
 */
void DirectMeterMap::setDestination(const IR::IDeclaration* meter,
                                    const IR::Expression* destination) {
    auto info = getInfo(meter);
    if (info == nullptr)
        info = createInfo(meter);
    if (info->destinationField == nullptr) {
        info->destinationField = destination;
    } else {
        bool same = checkSame(destination, info->destinationField);
        if (!same)
            ::error("On this target all meter operations must write to the same destination "
                    "but %1% and %2% are different", destination, info->destinationField);
    }
}

/**
 * Set the size of the table that a meter is attached to.
 */
void DirectMeterMap::setSize(const IR::IDeclaration* meter, unsigned size) {
    auto info = getInfo(meter);
    CHECK_NULL(info);
    info->tableSize = size;
}

}  // namespace AFI

