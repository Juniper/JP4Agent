//
// Juniper P4 Agent
//
/// @file  P4Info.cpp
/// @brief P4 Info
//
// Created by Sandesh Kumar Sodhi, November 2017
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

#include "pvtPI.h"

//
// Description
//
std::ostream &
P4InfoTable::description(std::ostream &os) const
{
    const auto &pre = _table.preamble();
    os << "_________ P4InfoTable _______" << std::endl;
    os << "pre.id():" << pre.id() << std::endl;
    os << "pre.name().c_str():" << pre.name().c_str() << std::endl;
    os << "table.match_fields().size():" << _table.match_fields().size()
       << std::endl;
    os << "table.action_refs().size():" << _table.action_refs().size()
       << std::endl;
    os << "table.size():" << _table.size() << std::endl;
    return os;
}

//
// Description
//
std::ostream &
P4InfoAction::description(std::ostream &os) const
{
    const auto &pre = _action.preamble();
    os << "_________ Action _______" << std::endl;
    os << "pre.id():" << pre.id() << std::endl;
    os << "pre.name().c_str():" << pre.name().c_str() << std::endl;
    os << "action.params().size():" << _action.params().size() << std::endl;
    for (const auto &param : _action.params()) {
        os << "param.id():" << param.id() << std::endl;
        os << "param.name().c_str():" << param.name().c_str() << std::endl;
        os << "param.bitwidth():" << param.bitwidth() << std::endl;
    }

    return os;
}
