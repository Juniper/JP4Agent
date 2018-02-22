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

#include "Afi.h"

namespace AFIHAL {

AfiObjectPtr
AfiDevice::handleDMObject(const AfiJsonResource& res)
{
    Log(DEBUG) << "____ AfiDevice::handleDMObject ____\n";
    AfiObjectPtr afiObj = _objCreator.create(res.type(), res);

    //
    // object creation failed
    //
    if (afiObj == nullptr) {
        Log(ERROR) << "Unable to create afi object ";
        return nullptr;
    }

    insertToObjectMap(afiObj);
   
    //
    // Now bind the afi object.
    //
    if (!(afiObj->bind())) {
        //
        // Unable to bind
        //
        Log(ERROR) << ": Unable to bind afi object ";
        return nullptr;
    }
    return afiObj;
}

#if 0

void
AfiDevice::bindAfiObjects()
{
    Log(DEBUG) << BOOST_CURRENT_FUNCTION << "Binding AFI objects";

    jP4Agent->afiClient().setIngressStart(ingressStartToken());
}
#endif

}  // namespace AFIHAL
