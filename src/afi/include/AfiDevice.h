//
// Juniper P4 Agent
//
/// @file  AfiDevice.cpp
/// @brief Afi Device
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

#ifndef SRC_AFI_INCLUDE_AFIDEVICE_H_
#define SRC_AFI_INCLUDE_AFIDEVICE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "AfiCreator.h"
#include "AfiJsonResource.h"
#include "AfiObject.h"

namespace AFIHAL
{
class AfiDevice;
using AfiDeviceUPtr = std::unique_ptr<AfiDevice>;
// using AfiDevicePtr = std::shared_ptr<AfiDevice>;
// using AfiDeviceWeakPtr = std::weak_ptr<AfiDevice>;

using AfiObjectNameMap = std::map<std::string, AfiObjectPtr>;

class AfiDevice
{
 protected:
    //
    // Device mount
    //
    AfiCreator<AfiJsonResource, AfiObjectPtr> _objCreator;

 public:
    //
    // Constructor and destructor
    //
    explicit AfiDevice(const std::string &name) : _name(name) {}
    ~AfiDevice() {}
    // virtual ~AfiDevice() {};

    //
    // Mount interface
    //
    void setObjectCreator(
        const std::string objType,  //<<<<<<< TBD: string ref
        AfiCreator<AfiJsonResource, AfiObjectPtr>::AfiObjectCreator objCreator)
    {
        _objCreator.setCreator(objType, objCreator);
    }

    virtual void setObjectCreators() = 0;

    AfiObjectPtr handleDMObject(const AfiJsonResource &res);

    void insertToObjectMap(const AfiObjectPtr &obj)
    {
        Log(DEBUG) << "insertToObjectMap... obj->name():" << obj->name();
        _objectsMap[obj->name()] = obj;
    }

    const AfiObjectPtr getAfiObject(const std::string &name)
    {
        Log(DEBUG) << "getAfiObject name:" << name;
        return _objectsMap[name];
    }

    const std::vector<AfiObjectPtr> getAfiObjects() const
    {
        std::vector<AfiObjectPtr> objs;
        for (const auto &objpair : _objectsMap) {
            objs.push_back(objpair.second);
        }
        return objs;
    }

 private:
    AfiObjectNameMap _objectsMap;
    std::string      _name;
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFIDEVICE_H_
