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

#ifndef _BACKENDS_AFI_PORTABLESWITCH_H_
#define _BACKENDS_AFI_PORTABLESWITCH_H_

#include "ir/ir.h"
#include "lower.h"
#include "lib/gmputil.h"
#include "lib/json.h"
#include "frontends/common/resolveReferences/referenceMap.h"
#include "frontends/p4/coreLibrary.h"
#include "frontends/p4/enumInstance.h"
#include "frontends/p4/methodInstance.h"
#include "frontends/p4/typeMap.h"
#include "helpers.h"

namespace P4 {

using ::Model::Elem;
using ::Model::Type_Model;
using ::Model::Param_Model;

// Block has a name and a collection of elements
template<typename T>
struct Block_Model : public Type_Model {
    std::vector<T> elems;
    explicit Block_Model(cstring name) :
        ::Model::Type_Model(name) {}
};

/// Enum_Model : Block_Model<Elem> : Type_Model
struct Enum_Model : public Block_Model<Elem> {
    ::Model::Type_Model type;
    explicit Enum_Model(cstring name) :
        Block_Model(name), type("Enum") {}
};

/// Parser_Model : Block_Model<Param_Model> : Type_Model
struct Parser_Model : public Block_Model<Param_Model> {
    ::Model::Type_Model type;
    explicit Parser_Model(cstring name) :
        Block_Model<Param_Model>(name), type("Parser") {}
};

/// Control_Model : Block_Model<Param_Model> : Type_Model
struct Control_Model : public Block_Model<Param_Model> {
    ::Model::Type_Model type;
    explicit Control_Model(cstring name) :
        Block_Model<Param_Model>(name), type("Control") {}
};

/// Method_Model : Block_Model<Param_Model> : Type_Model
struct Method_Model : public Block_Model<Param_Model> {
    ::Model::Type_Model type;
    explicit Method_Model(cstring name) :
        Block_Model<Param_Model>(name), type("Method") {}
};

/// Extern_Model : Block_Model<Method_Model> : Type_Model
struct Extern_Model : public Block_Model<Method_Model> {
    ::Model::Type_Model type;
    explicit Extern_Model(cstring name) :
        Block_Model<Method_Model>(name), type("Extern") {}
};

/// PortableModel : Model::Model
class PortableModel : public ::Model::Model {
 public:
    std::vector<Parser_Model*>  parsers;
    std::vector<Control_Model*> controls;
    std::vector<Extern_Model*>  externs;
    std::vector<Type_Model*>    match_kinds;
    bool find_match_kind(cstring kind_name);
    bool find_extern(cstring extern_name);
    static PortableModel instance;
    PortableModel() : ::Model::Model("0.2") {}
};

}  // namespace P4

std::ostream& operator<<(std::ostream &out, Model::Type_Model& m);
std::ostream& operator<<(std::ostream &out, Model::Param_Model& p);
std::ostream& operator<<(std::ostream &out, P4::PortableModel& e);
std::ostream& operator<<(std::ostream &out, P4::Method_Model& p);
std::ostream& operator<<(std::ostream &out, P4::Parser_Model* p);
std::ostream& operator<<(std::ostream &out, P4::Control_Model* p);
std::ostream& operator<<(std::ostream &out, P4::Extern_Model* p);

// portableSwitch

#endif  /* _BACKENDS_AFI_PORTABLESWITCH_H_ */
