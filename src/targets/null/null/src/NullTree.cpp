//
// Juniper P4 Agent
//
/// @file  NullTree.h
/// @brief Null Tree
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

#include "Null.h"

namespace NULLHALP {

void NullTree::_bind()
{
    std::cout << "NullTree: _bind" << std::endl;
    Log(DEBUG)<< "Pushing NullTree to ASIC";


    ::ywrapper::StringValue key_field = _tree.key_field();
    Log(DEBUG) << "key_field: " << key_field.value();

    //TBD: FIXME: Write into file
}

//  
// Description
//  
std::ostream & NullTree::description (std::ostream &os) const
{
    os << "_________ NullTree _______"   << std::endl;
    os << "Name                :" << this->name()  << std::endl;
    os << "Id                  :" << this->id()    << std::endl;
    //os << "_defaultTragetToken :" << this->_defaultTragetToken << std::endl;
    //os << "_token              :" << this->_token << std::endl;
    
#if 0
    os << "match_type     :" << _match_type   << std::endl;
    os << "table_type     :" << _table_type   << std::endl;
    os << "_matchKey      :"   << _matchKey   << std::endl;
    os << "Key match kind :" << _keyMatchKind << std::endl;
#endif
    return os;
}

}  // namespace NULLHALP
