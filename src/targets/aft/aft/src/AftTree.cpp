//
// Juniper P4 Agent
//
/// @file  AftTree.h
/// @brief Aft Tree
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

#include <memory>
#include "Aft.h"

extern std::unique_ptr<opentracing::v1::Span> span;

namespace AFTHALP
{
void
AftTree::_bind()
{
    std::cout << "AftTree: _bind" << std::endl;
    Log(DEBUG) << "Pushing AftTree to ASIC";

    ::ywrapper::StringValue key_field = _tree.key_field();
    Log(DEBUG) << "key_field: " << key_field.value();

    std::stringstream ks;
    ks << key_field.value();
    opentracing::string_view key("Aft:AftTree:Key Field");
    opentracing::string_view key_val(ks.str());
    span->SetBaggageItem(key, key_val);

    AftNodeToken puntToken = AftClient::instance().puntPortToken();
    setDefaultTargetToken(puntToken);

    Log(DEBUG) << "_defaultTargetToken: " << _defaultTargetToken;
    // TBD: rtt name from _tree
    _token = AftClient::instance().addTable("rtt0", key_field.value(),
                                            _defaultTargetToken);

    Log(DEBUG) << "RTT _token: " << _token;

    AftClient::instance().setIngressStart(_token);
    // return _token;
}

//
// Description
//
std::ostream &
AftTree::description(std::ostream &os) const
{
    os << "_________ AftTree _______" << std::endl;
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
