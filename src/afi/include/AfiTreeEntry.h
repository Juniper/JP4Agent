//
// Juniper P4 Agent
//
/// @file  AfiTreeEntry.h
/// @brief Afi tree entry
//
// Created by Sandesh Kumar Sodhi, February 2018
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

#ifndef __AFIHAL_AfiTreeEntry__
#define __AFIHAL_AfiTreeEntry__

#include "Afi.h"

namespace AFIHAL {

class AfiTreeEntry;
using AfiTreeEntryPtr = std::shared_ptr<AfiTreeEntry>;
using AfiTreeEntryWeakPtr = std::weak_ptr<AfiTreeEntry>;

class AfiTreeEntry: public AfiObject
{
public:
    AfiTreeEntry (const AfiJsonResource &jsonRes);

    ~AfiTreeEntry () {}

    //  
    // Debug
    //  
    std::ostream &description (std::ostream &os) const;

    friend std::ostream &operator<< (std::ostream &os,
                                     const AfiTreeEntryPtr &afitree) {
        return afitree->description(os);
    }   

protected:
    juniper::afi_tree_entry::AfiTreeEntry _treeEntry;
};

}  // namespace AFIHAL

#endif // __AFIHAL_AfiTreeEntry__
