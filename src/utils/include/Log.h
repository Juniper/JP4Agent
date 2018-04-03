//
// Juniper P4 Agent
//
/// @file  Log.h
/// @brief Log/debug infra
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


#ifndef Log_h
#define Log_h


#include <iostream>
#include <unistd.h>

//
// Log
//

enum LogLevel{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR
};

class Log {
public:
    Log(LogLevel level) : _logLevel(level) { }
    ~Log() { std::cout << std::endl; }
    template<class T>
    Log &operator<<(const T &msg) {
        if (_hdrDisplayed == false) {
	  std::cout << currentDateTime();
            //std::cout << " {" << ::getpid() << "}";
	  std::cout << " [" << logLevelStr() << "]";
            _hdrDisplayed = true;
        }
	std::cout << msg;
        return *this;
    }


private:
    LogLevel   _logLevel = DEBUG;
    bool       _hdrDisplayed = false;

    const std::string currentDateTime() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
        return buf;
    }

    inline std::string logLevelStr() {
        std::string str;
        switch(_logLevel) {
            case DEBUG:    str = "DEBUG"  ; break;
            case INFO:     str = "INFO"   ; break;
            case WARNING:  str = "WARNING"; break;
            case ERROR:    str = "ERROR"  ; break;
        }
        return str;
    }
};

#endif /* Log_h */
