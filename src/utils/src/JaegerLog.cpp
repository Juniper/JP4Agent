//
// Juniper P4 Agent
//
/// @file  JaegerLog.cpp
/// @brief JaegerLog
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

#include "JaegerLog.h"

JaegerLog* JaegerLog::_instance = 0;

JaegerLog::JaegerLog()
{
  _span = NULL;
}

JaegerLog::~JaegerLog()
{
  delete this;
}

JaegerLog* JaegerLog::getInstance()
{
  if (_instance == 0)
  {
    _instance = new JaegerLog();
  }

  return _instance;
}


void JaegerLog::initTracing(std::string configFileName)
{
  try {
    YAML::Node cfgFile = YAML::LoadFile(configFileName);
    const auto cfg = jaegertracing::Config::parse(cfgFile);
    auto tracer = jaegertracing::Tracer::make("JP4Agent", cfg);
    opentracing::Tracer::InitGlobal(tracer);
    } catch (const std::exception& e) {
  }
}

void JaegerLog::teardownTracing()
{
  opentracing::Tracer::Global()->Close();
}

void JaegerLog::startSpan(const std::string spanName)
{
  auto tracer = opentracing::Tracer::Global();
  _span       = tracer->StartSpan(spanName);
  std::this_thread::sleep_for(std::chrono::milliseconds{10});
}

void JaegerLog::finishSpan()
{
  _span->Finish();
}

void JaegerLog::Log(const std::string type, std::string val)
{
  opentracing::string_view name(type);
  opentracing::string_view name_val(val);
  _span->SetBaggageItem(name, name_val);

}
