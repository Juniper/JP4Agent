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

#ifndef _BACKENDS_AFI_JSONOBJECTS_H_
#define _BACKENDS_AFI_JSONOBJECTS_H_

#include <map>
#include "lib/json.h"
#include "lib/ordered_map.h"

namespace AFI {

class JsonObjects {
 protected:
    Util::JsonObject* find_object_by_name(Util::JsonArray* array,
                                          const cstring& name);

 public:
    void add_program_info(const cstring& name);
    void add_meta_info();
    unsigned add_header_type(const cstring& name, Util::JsonArray*& fields, unsigned max_length);
    unsigned add_union_type(const cstring& name, Util::JsonArray*& fields);
    unsigned add_union(const cstring& type, Util::JsonArray*& fields, const cstring& name);
    unsigned add_header_type(const cstring& name);
    void add_header_field(const cstring& name, Util::JsonArray*& field);
    unsigned add_header(const cstring& type, const cstring& name);
    unsigned add_metadata(const cstring& type, const cstring& name);
    void add_header_stack(const cstring& type, const cstring& name,
                          const unsigned size, std::vector<unsigned>& header_ids);
    void add_error(const cstring& name, const unsigned type);
    void add_enum(const cstring& enum_name, const cstring& entry_name,
                  const unsigned entry_value);
    unsigned add_parser(const cstring& name);
    unsigned add_parser_state(const unsigned id, const cstring& state_name);
    void add_parser_transition(const unsigned id, Util::IJson* transition);
    void add_parser_op(const unsigned id, Util::IJson* op);
    void add_parser_transition_key(const unsigned id, Util::IJson* key);
    unsigned add_action(const cstring& name, Util::JsonArray*& params, Util::JsonArray*& body);
    void add_pipeline();
    void add_extern_attribute(const cstring& name, const cstring& type,
                              const cstring& value, Util::JsonArray* attributes);
    void add_extern(const cstring& name, const cstring& type, Util::JsonArray*& attributes);
    JsonObjects();
    Util::JsonArray* insert_array_field(Util::JsonObject* parent, cstring name);
    Util::JsonArray* append_array(Util::JsonArray* parent);
    Util::JsonArray* create_parameters(Util::JsonObject* object);
    Util::JsonObject* create_primitive(Util::JsonArray* parent, cstring name);

    std::map<unsigned, Util::JsonObject*> map_parser;
    std::map<unsigned, Util::JsonObject*> map_parser_state;

 public:
    Util::JsonObject* toplevel;
    Util::JsonObject* meta;
    Util::JsonArray* actions;
    Util::JsonArray* calculations;
    Util::JsonArray* checksums;
    Util::JsonArray* counters;
    Util::JsonArray* deparsers;
    Util::JsonArray* enums;
    Util::JsonArray* errors;
    Util::JsonArray* externs;
    Util::JsonArray* field_lists;
    Util::JsonArray* headers;
    Util::JsonArray* header_stacks;
    Util::JsonArray* header_types;
    Util::JsonArray* header_union_types;
    Util::JsonArray* header_unions;
    Util::JsonArray* header_union_stacks;
    ordered_map<std::string, unsigned> header_type_id;
    ordered_map<std::string, unsigned> union_type_id;
    Util::JsonArray* learn_lists;
    Util::JsonArray* meter_arrays;
    Util::JsonArray* parsers;
    Util::JsonArray* pipelines;
    Util::JsonArray* register_arrays;
    Util::JsonArray* force_arith;
    Util::JsonArray* field_aliases;
};

}  // namespace AFI

#endif /* _BACKENDS_AFI_JSONOBJECTS_H_ */
