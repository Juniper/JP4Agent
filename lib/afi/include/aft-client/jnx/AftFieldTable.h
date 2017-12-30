//
/// @file   AftFieldTable.h
/// @brief  AftFieldTable and AftFieldEntry class definitions
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

#ifndef __AftFieldTable__
#define __AftFieldTable__

#include "jnx/AftStringTable.h"


///
/// @addtogroup AftFieldKey
/// @{
///

///
/// @class   AftFieldTable
/// @brief   Master class for managing field mappings
///
class AftFieldTable: public AftStringTable<AftFieldTable, AftFieldIndex> {
public:
    using AftStringTable::AftStringTable;
    
};

///
/// @}
///

#endif 
