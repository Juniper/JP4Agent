//
// Juniper P4 Agent
//
/// @file  NullTreeEntry.h
/// @brief Null tree entry
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

#include "Null.h"

namespace NULLHALP {

//NullNodeToken NullTreeEntry::bind(void)
void NullTreeEntry::_bind()
{
    std::cout << "NullTreeEntry: _bind" << std::endl;
    Log(DEBUG)<< "Pushing NullTreeEntry to ASIC";

    Log(DEBUG) << "tree.ByteSize(): " << _treeEntry.ByteSize();

    ::ywrapper::StringValue entry_name = _treeEntry.name();
    Log(DEBUG) << "entry_name: " << entry_name.value();

    ::ywrapper::StringValue parent_name = _treeEntry.parent_name();
    Log(DEBUG) << "parent_name: " << parent_name.value();

    ::ywrapper::StringValue target_afi_object = _treeEntry.target_afi_object();
    Log(DEBUG) << "target_afi_object: " << target_afi_object.value();

    ::ywrapper::StringValue prefix_bytes = _treeEntry.prefix_bytes();
    //Log(DEBUG) << "prefix: " << prefix.value();
    std::string prefix_bytes_str = prefix_bytes.value();

    ::ywrapper::UintValue prefix_length = _treeEntry.prefix_length();

    //AfiObjectPtr afiObjPtr = AFIHAL::Afi::instance().getAfiObject(entry_name.value());
    
    //AfiTreePtr afiTreePtr = std::dynamic_pointer_cast<AfiTree>(afiObjPtr);
    NullTreePtr nullTreePtr = std::dynamic_pointer_cast<NullTree>(AFIHAL::Afi::instance().getAfiObject(parent_name.value()));

    if (nullTreePtr == nullptr) {
        Log(ERROR) << "Could not find parent AfiTree";
        return;
    }

    std::cout<<"nullTree :" << nullTreePtr << "\n";

    // Write into file
    gtestFile.open("../NullTest.txt", std::fstream::app);
    gtestFile << "tree.ByteSize(): " << _treeEntry.ByteSize() << "\n";
    gtestFile << "entry_name: " << entry_name.value() << "\n";
    gtestFile << "parent_name: " << parent_name.value() << "\n";
    gtestFile << "target_afi_object: " << target_afi_object.value() << "\n";
    gtestFile.close();

}

//  
// Description
//  
std::ostream & NullTreeEntry::description (std::ostream &os) const
{
    os << "_________ NullTreeEntry _______"   << std::endl;
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
