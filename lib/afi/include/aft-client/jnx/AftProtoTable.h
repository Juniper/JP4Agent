//
/// @file   AftProtoTable.h
/// @brief  AftProtoTable and AftNameEntry class definitions
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

#ifndef __AftProtoTable__
#define __AftProtoTable__

#include "jnx/AftProto.h"
#include "jnx/AftStringTable.h"

///
/// @defgroup  AftProto
/// @brief Classes used as the core of protocol lookups for Aft
/// @{
///


///
/// @class   AftProtoTable
/// @brief   Master class for managing protocol mappings
///
class AftProtoTable: public AftStringTable<AftProtoTable, AftProtoIndex> {
public:
    using AftStringTable::AftStringTable;

};

///
/// @}
///

#endif
