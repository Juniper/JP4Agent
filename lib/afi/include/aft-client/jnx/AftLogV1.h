//
/// @file   AftLog.h
/// @brief
//
//  Copyright (c) Juniper Networks, Inc., [2017].
//
//  All rights reserved.
//
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftLog__
#define __AftLog__

#include "AftTypes.h"

class AftLog {
public:
    static bool tracing();
    static void setTracing(bool enabled);
    static void trace(const std::string &name, const std::string &traceString);
    static void trace(const std::string &name, const std::string &traceString, std::stringstream &ss);
    static void error(const std::string &name, const std::string &errorString);
    static void error(const std::string &name, const std::string &errorString, std::stringstream &ss);
};


#endif
