//
// Juniper P4 Agent
//
/// @file  Afi.h
/// @brief Afi
//
// Created by Sandesh Kumar Sodhi, January 2018
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

#ifndef __Afi__
#define __Afi__

#include <memory>
#include <map>
#include <jsoncpp/json/json.h>

#include "Log.h"
#include "Utils.h"
#include "AfiTypes.h"
#include "AfiJsonResource.h"
#include "AfiDM.h"
#include "AfiNext.h"
#include "AfiObject.h"
#include "AfiCreator.h"
#include "AfiDevice.h"
#include "AfiTree.h"
#include "AfiTreeEntry.h"

//
// This function to be impleted by each target
//
AFIHAL::AfiDeviceUPtr createDevice(const std::string &name);

namespace AFIHAL {

class Afi;
using AfiUPtr = std::unique_ptr<Afi>;
using AfiObjectNameMap = std::map<std::string, AfiObjectPtr>;
//using AfiObjectIdMap = std::map<uint32_t, AfiObjectPtr>;

class Afi {

public:
    static Afi& instance() {
        static Afi afi;
        return afi;
    }
  
    Afi(Afi const&) = delete;
    Afi(Afi&&) = delete;
    Afi& operator=(Afi const&) = delete;
    Afi& operator=(Afi &&) = delete;

    //AfiDevice& getDevice() {
    //    return _afiDevice;
    //}

    void init() {
        //AfiDeviceUPtr createDevice(const std::string &name);

        Log(DEBUG) << "___ Afi::init : creating device ___";
        _afiDevice = createDevice("_device_");
    }

    bool handleAfiJsonObject(const Json::Value &cfg_obj);
    bool handlePipelineConfig(const Json::Value &cfg_root);
    bool addEntry(const std::string &keystr, int pLen);

    const AfiObjectPtr getAfiObject(const std::string &name)
    {
        Log(DEBUG) << "getAfiObject name:"<< name;
        return _afiDevice->getAfiObject(name); 
    }

protected:
    Afi() {}
    ~Afi() {}
 
private:
    AfiDeviceUPtr    _afiDevice;
};

}  // namespace AFIHAL


#endif // __Afi__
