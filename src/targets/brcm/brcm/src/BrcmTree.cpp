//
// Juniper P4 Agent
//
/// @file  BrcmTree.cpp
/// @brief Brcm Tree
//
// Created by Sudheendra Gopinath, January 2018
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

#include "Brcm.h"
#include "JaegerLog.h"

namespace BRCMHALP {

void BrcmTree::_bind()
{
    std::cout << "BrcmTree: _bind" << std::endl;
    Log(DEBUG)<< "Pushing BrcmTree to ASIC";


    ::ywrapper::StringValue key_field = _tree.key_field();
    Log(DEBUG) << "key_field: " << key_field.value();

    std::stringstream ks;
    ks  << key_field.value();
    JaegerLog::getInstance()->Log("Brcm:BrcmTree:Key Field", ks.str());

    // Write into file for Brcm test
    gtestFile.open("../BrcmTest.txt", std::fstream::app);
    gtestFile << "key_field: " << key_field.value() << "\n";
    gtestFile.close();
}

//  
// Description
//  
std::ostream & BrcmTree::description (std::ostream &os) const
{
    os << "_________ BrcmTree _______"   << std::endl;
    os << "Name                :" << this->name()  << std::endl;
    os << "Id                  :" << this->id()    << std::endl;
    //os << "_defaultTargetToken :" << this->_defaultTargetToken << std::endl;
    //os << "_token              :" << this->_token << std::endl;
    
#if 0
    os << "match_type     :" << _match_type   << std::endl;
    os << "table_type     :" << _table_type   << std::endl;
    os << "_matchKey      :"   << _matchKey   << std::endl;
    os << "Key match kind :" << _keyMatchKind << std::endl;
#endif
    return os;
}

}  // namespace BRCMHALP
