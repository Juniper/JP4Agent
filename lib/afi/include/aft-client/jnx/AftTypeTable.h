//
/// @file   AftTypeTable.h
/// @brief  AftTypeTable and AftTypeEntry class definitions
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

#ifndef __AftTypeTable__
#define __AftTypeTable__

#include "jnx/AftStringTable.h"

///
/// @addtogroup AftTypes
/// @{
///

class AftTypeTable: public AftStringBitable<AftTypeTable, AftTypeIndex> {
protected:
    AftTypeIndex maxIndex = 0;
    
public:
    AftTypeTable(): AftStringBitable<AftTypeTable, AftTypeIndex>() {
        _insert("AftNodeTypeNone", maxIndex++);
    }
    
    bool insert (const std::string &newEntryName, AftTypeIndex newIndex) {
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
