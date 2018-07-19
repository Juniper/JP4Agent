//
// Utils.cpp
//
// Test controller
//
// Created by Sandesh Kumar Sodhi, December 2017
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

#include "P4InfoUtils.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include <fstream>
#include <streambuf>
#include <string>

int get_table_id(const p4::config::P4Info &p4info,
                 const std::string &t_name) {
  for (const auto &table : p4info.tables()) {
    const auto &pre = table.preamble();
    if (pre.name() == t_name) return pre.id();
  }
  return 0;
}

int get_action_id(const p4::config::P4Info &p4info,
                  const std::string &a_name) {
  for (const auto &action : p4info.actions()) {
    const auto &pre = action.preamble();
    if (pre.name() == a_name) return pre.id();
  }
  return 0;
}

int get_mf_id(const p4::config::P4Info &p4info,
              const std::string &t_name, const std::string &mf_name) {
  for (const auto &table : p4info.tables()) {
    const auto &pre = table.preamble();
    if (pre.name() != t_name) continue;
    for (const auto &mf : table.match_fields()) {
      if (mf.name() == mf_name) return mf.id();
    }
  }
  return -1;
}

int get_param_id(const p4::config::P4Info &p4info,
                 const std::string &a_name, const std::string &param_name) {
  for (const auto &action : p4info.actions()) {
    const auto &pre = action.preamble();
    if (pre.name() != a_name) continue;
    for (const auto &param : action.params())
      if (param.name() == param_name) return param.id();
  }
  return -1;
}

p4::config::P4Info parse_p4info(const char *path) {
  p4::config::P4Info p4info;
  std::ifstream istream(path);
  // p4info.ParseFromIstream(&istream);
  google::protobuf::io::IstreamInputStream istream_(&istream);
  google::protobuf::TextFormat::Parse(&istream_, &p4info);
  return p4info;
}

