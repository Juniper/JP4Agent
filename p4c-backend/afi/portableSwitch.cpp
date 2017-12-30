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


#include "frontends/common/model.h"
#include "portableSwitch.h"

namespace P4 {

PortableModel PortableModel::instance;

}

bool P4::PortableModel::find_match_kind(cstring kind_name) {
    bool found = false;
    for (auto m : instance.match_kinds) {
        if (m->toString() == kind_name) {
            found = true;
            break;
        }
    }
    return found;
}

bool P4::PortableModel::find_extern(cstring extern_name) {
    bool found = false;
    for (auto m : instance.externs) {
        if (m->type.toString() == extern_name) {
            found = true;
            break;
        }
    }
    return found;
}

std::ostream& operator<<(std::ostream &out, Model::Type_Model& m) {
    out << "Type_Model(" << m.toString() << ")";
    return out;
}

std::ostream& operator<<(std::ostream &out, Model::Param_Model& p) {
    out << "Param_Model(" << p.toString() << ") " << p.type;
    return out;
}

std::ostream& operator<<(std::ostream &out, P4::PortableModel& e) {
    out << "PortableModel " << e.version << std::endl;
    for (auto v : e.parsers)  out << v;
    for (auto v : e.controls) out << v;
    for (auto v : e.externs)  out << v;
    return out;
}

std::ostream& operator<<(std::ostream &out, P4::Method_Model& p) {
    out << "Method_Model(" << p.toString() << ") " << p.type << std::endl;
    for (auto e : p.elems) {
        out << "    " << e << std::endl;
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, P4::Parser_Model* p) {
    out << "Parser_Model(" << p->toString() << ") " << p->type << std::endl;
    for (auto e : p->elems) {
        out << "  " << e << std::endl;
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, P4::Control_Model* p) {
    out << "Control_Model(" << p->toString() << ") " << p->type << std::endl;
    for (auto e : p->elems) {
        out << "  " << e << std::endl;
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, P4::Extern_Model* p) {
    out << "Extern_Model(" << p->toString() << ") " << p->type << std::endl;
    for (auto e : p->elems) {
        out << "  " << e << std::endl;
    }
    return out;
}
