//
// Juniper P4 Agent
//
/// @file  BrcmDevice.h
/// @brief Brcm Device
//
// Created by Sudheendra Gopinath, March 2018
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

#ifndef __BrcmDevice__
#define __BrcmDevice__

#include "Brcm.h"
#include "BrcmInit.h"
#include "BrcmRpc.h"

namespace BRCMHALP {

//
// BrcmDevice
//

class BrcmDevice;
using BrcmDeviceUPtr = std::unique_ptr<BrcmDevice>;
using BrcmDevicePtr = std::shared_ptr<BrcmDevice>;
using BrcmDeviceWeakPtr = std::weak_ptr<BrcmDevice>;

class BrcmDevice final : public AFIHAL::AfiDevice
{
public:
    BrcmDevice(const std::string &name);
    ~BrcmDevice();
    
    static BrcmDeviceUPtr create(const std::string &newName);
    void destroy();

private:
    BrcmRpcPtr      _brpc;
    //BrcmPlusPtr     _brcmPlus;

    void setObjectCreators();

    void brpcStart(std::string dstIp);

    void brpcStop();

    //void setBrcmHandle();
};

}  // namespace BRCMHALP

#endif // __BrcmDevice__
