//
/// @file   AftGroupTable.h
/// @brief  AftGroupTable and AftNameEntry class definitions
//
//  Copyright (c) Juniper Networks, Inc., [2017].
//
//  All rights reserved.
//
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftGroupTable__
#define __AftGroupTable__

#include "jnx/AftGroup.h"
#include "jnx/AftStringTable.h"

///
/// @defgroup  AftGroup
/// @brief Classes used as the core of group lookups for Aft
/// @{
///


///
/// @class   AftGroupTable
/// @brief   Master class for managing group mappings
///
class AftGroupTable: public AftStringBitable<AftGroupTable, AftGroupIndex> {
protected:
    AftGroupIndex maxIndex = 0;
    
public:
    AftGroupTable(): AftStringBitable<AftGroupTable, AftGroupIndex>() {
        _insert("none", maxIndex++);
    }
    
    bool insert (const std::string &newEntryName, AftTypeIndex newIndex) {
        if (maxIndex < newIndex) maxIndex = newIndex + 1;
        return _insert(newEntryName, newIndex);
    };
    
    AftGroup &&insert (const std::string &newName) {
        AftGroupIndex newIndex = maxIndex++;
        _insert(newName, newIndex);
        return std::move(AftGroup(newIndex));
    }
};

///
/// @}
///

#endif
