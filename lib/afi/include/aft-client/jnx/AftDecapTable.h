//
/// @file   AftDecapTable.h
/// @brief  AftDecapTable and AftDecapEntry class definitions
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

#ifndef __AftDecapTable__
#define __AftDecapTable__

#include "jnx/AftStringTable.h"

///
/// @defgroup  AftDecap De-encapsulation
/// @brief Classes used to manage packet de-encapsulation
/// @{
///

///
/// @class   AftDecapEntry
/// @brief   Individual entry in the de-encapsulation table
///
class AftDecapTable: public AftStringBitable<AftDecapTable, AftDecapIndex> {
protected:
    AftDecapIndex maxIndex;
    
public:
    using AftStringBitable::AftStringBitable;
    
    bool insert (const std::string &newEntryName, AftDecapIndex newIndex) {
        if (maxIndex < newIndex) maxIndex = newIndex + 1;
        return _insert(newEntryName, newIndex);
    };
    
    void insert (const std::string &newName) {
        _insert(newName, maxIndex++);
    }
};

///
/// @}
///

#endif 
