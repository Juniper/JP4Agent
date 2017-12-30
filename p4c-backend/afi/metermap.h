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


#ifndef _BACKENDS_AFI_METERMAP_H_
#define _BACKENDS_AFI_METERMAP_H_

#include "ir/ir.h"

namespace AFI {

class DirectMeterMap final {
 public:
    struct DirectMeterInfo {
        const IR::Expression* destinationField;
        const IR::P4Table* table;
        unsigned tableSize;

        DirectMeterInfo() : destinationField(nullptr), table(nullptr), tableSize(0) {}
    };

 private:
    // key is declaration of direct meter
    std::map<const IR::IDeclaration*, DirectMeterInfo*> directMeter;
    DirectMeterInfo* createInfo(const IR::IDeclaration* meter);
 public:
    DirectMeterInfo* getInfo(const IR::IDeclaration* meter);
    void setDestination(const IR::IDeclaration* meter, const IR::Expression* destination);
    void setTable(const IR::IDeclaration* meter, const IR::P4Table* table);
    void setSize(const IR::IDeclaration* meter, unsigned size);
};

}  // namespace AFI

#endif  /* _BACKENDS_AFI_METERMAP_H_ */
