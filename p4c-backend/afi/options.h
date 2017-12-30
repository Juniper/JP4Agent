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


#ifndef _BACKENDS_AFI_OPTIONS_H_
#define _BACKENDS_AFI_OPTIONS_H_

#include <getopt.h>
#include "frontends/common/options.h"

namespace AFI {

enum class Target { UNKNOWN, PORTABLE, SIMPLE };

class AFIOptions : public CompilerOptions {
 public:
    AFI::Target arch = AFI::Target::UNKNOWN;
    AFIOptions() {
        registerOption("--arch", "arch",
                       [this](const char* arg) {
                        if (!strcmp(arg, "psa")) {
                            arch = AFI::Target::PORTABLE;
                        } else if (!strcmp(arg, "ss")) {
                            arch = AFI::Target::SIMPLE;
                        } else {
                            ::error("Unknown architecture %1%", arg);
                        }
                        return true; },
                       "Compile for the specified architecture (psa or ss), default is ss.");
     }
};

}  // namespace AFI

#endif  /* _BACKENDS_AFI_OPTIONS_H_ */
