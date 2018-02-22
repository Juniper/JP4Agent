//
// Juniper P4 Agent
//
/// @file  AfiTree.h
/// @brief Afi Tree
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

#ifndef __AfiTree__
#define __AfiTree__

#include "Afi.h"

namespace AFIHAL {

class AfiTree;
using AfiTreePtr = std::shared_ptr<AfiTree>;
using AfiTreeWeakPtr = std::weak_ptr<AfiTree>;

class AfiTree: public AfiObject
{
public:
    AfiTree (const AfiJsonResource &jsonRes);

    ~AfiTree () {}

    //  
    // Debug
    //  
    std::ostream &description (std::ostream &os) const;

    friend std::ostream &operator<< (std::ostream &os,
                                     const AfiTreePtr &afitree) {
        return afitree->description(os);
    }   

protected:
    juniper::afi_tree::AfiTree _tree;
};

}  // namespace AFIHAL

#endif // __AfiTree__
