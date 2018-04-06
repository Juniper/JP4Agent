//
// Juniper P4 Agent
//
/// @file  JaegerLog.h
/// @brief Jaeger Logging interface
//
// Created by Manmeet Singh, March 2018
// Copyright (c) [2018] Juniper Networks, Inc. All rights reserved.
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


#ifndef JaegerLog_h
#define JaegerLog_h

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <jaegertracing/Tracer.h>


class JaegerLog
{
  private:
    static JaegerLog* _instance;
    std::unique_ptr<opentracing::v1::Span> _span;
    JaegerLog();
    ~JaegerLog();
  public:
    /* Static access method. */
    static JaegerLog* getInstance();
    void initTracing(std::string configFileName);
    void teardownTracing();
    void startSpan(const std::string spanName);
    void finishSpan();
    void Log(const std::string type, std::string val);
};


#endif /* JaegerLog_h */
