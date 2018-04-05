//
// Juniper P4 Agent
//
/// @file  AftTreeEntry.h
/// @brief Aft tree entry
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

#include <memory>
#include <string>
#include "Aft.h"
#include "JaegerLog.h"

namespace AFTHALP
{
// AftNodeToken AftTreeEntry::bind(void)
void
AftTreeEntry::_bind()
{
    std::cout << "AftTreeEntry: _bind" << std::endl;
    Log(DEBUG) << "Pushing AftTreeEntry to ASIC";

    Log(DEBUG) << "tree.ByteSize(): " << _treeEntry.ByteSize();

    ::ywrapper::StringValue entry_name = _treeEntry.name();
    Log(DEBUG) << "entry_name: " << entry_name.value();

    ::ywrapper::StringValue parent_name = _treeEntry.parent_name();
    Log(DEBUG) << "parent_name: " << parent_name.value();

    ::ywrapper::StringValue target_afi_object = _treeEntry.target_afi_object();
    Log(DEBUG) << "target_afi_object: " << target_afi_object.value();

    ::ywrapper::StringValue prefix_bytes = _treeEntry.prefix_bytes();
    // Log(DEBUG) << "prefix: " << prefix.value();
    std::string prefix_bytes_str = prefix_bytes.value();

    ::ywrapper::UintValue prefix_length = _treeEntry.prefix_length();

    // AfiObjectPtr afiObjPtr =
    // AFIHAL::Afi::instance().getAfiObject(entry_name.value());

    // AfiTreePtr afiTreePtr = std::dynamic_pointer_cast<AfiTree>(afiObjPtr);
    AftTreePtr aftTreePtr = std::dynamic_pointer_cast<AftTree>(
        AFIHAL::Afi::instance().getAfiObject(parent_name.value()));

    std::stringstream es;
    es << entry_name.value();
    JaegerLog::getInstance()->Log("AFT:AFTTreeEntry:Name", es.str());

    std::stringstream ps;
    ps << parent_name.value();
    JaegerLog::getInstance()->Log("AFT:AFTTreeEntry:Parent Name", ps.str());

    std::stringstream as;
    as << target_afi_object.value();
    JaegerLog::getInstance()->Log("AFT:AFTTreeEntry:Target AFI Object", as.str());
    JaegerLog::getInstance()->finishSpan();

    if (aftTreePtr == nullptr) {
        Log(ERROR) << "Could not find parent AfiTree";
        return;
    }

    std::cout << "aftTree :" << aftTreePtr << "\n";
    AftNodeToken aftTreeToken = aftTreePtr->token();

    Log(DEBUG) << "aftTreePtr->token() :" << aftTreeToken;

    uint16_t     portId = 1;  // TBD: FIXME
    AftNodeToken outputPortToken =
        AftClient::instance().outputPortToken(portId);
    Log(DEBUG) << "outputPortToken:" << outputPortToken;

    AftNodeToken etherEncapToken = AftClient::instance().addEtherEncapNode(
        "32:26:0a:2e:ff:f1", "5e:d8:f9:32:bd:85", outputPortToken);

    // jP4Agent->afiClient().addRoute(aftTreeToken, "1.1.1.1/10",
    // etherEncapToken);
    Log(DEBUG) << "Adding route...";
    for (unsigned int i = 0; i < prefix_length.value(); i++) {
        std::cout << int(prefix_bytes_str[i]) << ".";
    }
    std::cout << "\n";
    Log(DEBUG) << "prefix_bytes_str.c_str():" << prefix_bytes_str.c_str();
    Log(DEBUG) << "prefix_bytes_str.size() :" << prefix_bytes_str.size();
    Log(DEBUG) << "prefix_length.value()   :" << prefix_length.value();
    Log(DEBUG) << "etherEncapToken         :" << etherEncapToken;

    AftClient::instance().addRoute(aftTreeToken, prefix_bytes_str.c_str(),
                                   prefix_bytes_str.size(), 32,
                                   // prefix_length.value(),
                                   etherEncapToken);
}

//
// Description
//
std::ostream &
AftTreeEntry::description(std::ostream &os) const
{
    os << "_________ AftTreeEntry _______" << std::endl;
    os << "Name                :" << this->name() << std::endl;
    os << "Id                  :" << this->id() << std::endl;
    // os << "_defaultTargetToken :" << this->_defaultTargetToken << std::endl;
    // os << "_token              :" << this->_token << std::endl;

#if 0
    os << "match_type     :" << _match_type   << std::endl;
    os << "table_type     :" << _table_type   << std::endl;
    os << "_matchKey      :"   << _matchKey   << std::endl;
    os << "Key match kind :" << _keyMatchKind << std::endl;
#endif
    return os;
}

}  // namespace AFTHALP
