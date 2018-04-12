//
// Juniper P4 Agent
//
/// @file  BrcmDevice.cpp
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

#include "Brcm.h"
#include <boost/asio.hpp>

#include "BrcmIncludes.h"
#include "BrcmL3Intf.h"

namespace BRCMHALP {

//void
//BrcmDevice::setBrcmHandle() {
//    _brcmPlus = BrcmPlus::create();
//}

void
BrcmDevice::setObjectCreators()
{
    Log(DEBUG) << "___ BrcmDevice::setObjectCreators _______";
    setObjectCreator("afi-tree", &BrcmTree::create);
    setObjectCreator("afi-tree-entry", &BrcmTreeEntry::create);
}

//
// Create a RPC connection with the remote device
//
void
BrcmDevice::brpcStart(std::string dstIp)
{
    Log(DEBUG) << "___ BrcmDevice::brpcStart ___";
    Log(DEBUG) << "Remote IP: " << dstIp;

    _brpc = std::make_shared<BrcmRpc>(dstIp);
    if (_brpc != nullptr) {
        auto ret = _brpc->start();
        if (ret == 0) {
            Log(DEBUG) << "Connected";
        } else {
            Log(DEBUG) << "Failed to connect, " << ret; 
            _brpc = nullptr;
        }
    } else {
        Log(DEBUG) << "Failed to initiate";
    }
}

//
// Teardown RPC connection with the remote device
//
void
BrcmDevice::brpcStop()
{
    Log(DEBUG) << "___ BrcmDevice::brpcStop ___";
    _brpc->stop();
}

//
// Factory creation method, constructors and destructors
//
BrcmDeviceUPtr
BrcmDevice::create (const std::string &name)
{
    Log(DEBUG) << "___ BrcmDevice::create ___";
    auto device = std::make_unique<BrcmDevice>(name);

    // Create the engine mount
    device->setObjectCreators();

    device->brpcStart(name);

    // Create Brcm HALP handle
    BrcmPlus::instance();
    //device->setBrcmHandle();

    // TODO
    // 1. Ports' MAC addresses must come from device config
    // 2. L3 interfaces must come from device config or P4 program
    BrcmL3Intf::brcmAddTestL3Intf(9, 0x2c2c2c01, 0xffffff00, 0x88a25e91a2a8, 0, 1);
    BrcmL3Intf::brcmAddTestL3Intf(13, 0x37373701, 0xffffff00, 0x88a25e91a2a9, 0, 0);

    return device;
}

void
BrcmDevice::destroy()
{
    brpcStop();
}

BrcmDevice::BrcmDevice (const std::string &name): AfiDevice(name)
{
    // create() function does all the work
};

BrcmDevice::~BrcmDevice ()
{
    destroy();
};

}  // namespace BRCMHALP
