//
// Utils.h
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

#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef UBUNTU
#include <p4/config/p4info.grpc.pb.h>
#else
#include <p4info.grpc.pb.h>
#endif

#include <string>

int get_table_id(const p4::config::P4Info &p4info, const std::string &t_name);

int get_action_id(const p4::config::P4Info &p4info, const std::string &a_name);

int get_mf_id(const p4::config::P4Info &p4info,
              const std::string &t_name, const std::string &mf_name);

int get_param_id(const p4::config::P4Info &p4info,
                 const std::string &a_name, const std::string &param_name);

p4::config::P4Info parse_p4info(const char *path);

#endif  // _UTILS_H_
