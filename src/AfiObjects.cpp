//
// Juniper P4 Agent
//
/// @file  AfiObjects.cpp
/// @brief Afi Objects
//
// Created by Sandesh Kumar Sodhi, December 2017
// Copyright (c) [2017] Juniper Networks, Inc. All rights reserved.
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

#include "AfiObjects.h"
#include "JP4Agent.h"

AftNodeToken AfiTree::bind(void)
{
    Log(DEBUG)<< "Pushing AfiTree to ASIC";

    ::ywrapper::StringValue key_field = _tree.key_field();
    Log(DEBUG) << "key_field: " << key_field.value();

    AftNodeToken puntToken  = jP4Agent->afiClient().puntPortToken();
    setDefaultTragetToken(puntToken);

    _token = jP4Agent->afiClient().addTable(_name, key_field.value(), _defaultTragetToken);

    return _token;
}

//  
// Description
//  
std::ostream & AfiTree::description (std::ostream &os) const
{
    os << "_________ AfiTree _______"   << std::endl;
    os << "Name                :" << this->name()  << std::endl;
    os << "Id                  :" << this->id()    << std::endl;
    os << "_defaultTragetToken :" << this->_defaultTragetToken << std::endl;
    os << "_token              :" << this->_token << std::endl;
    
#if 0
    os << "match_type     :" << _match_type   << std::endl;
    os << "table_type     :" << _table_type   << std::endl;
    os << "_matchKey      :"   << _matchKey   << std::endl;
    os << "Key match kind :" << _keyMatchKind << std::endl;
#endif
    return os;
}

void 
AfiDeviceCfg::populateAfiObjects(const Json::Value &cfg_root)
{

    for (Json::Value::ArrayIndex i = 0; i != cfg_root.size(); i++) {
        const Json::Value &cfg_obj = cfg_root[i];
        std::string type = cfg_obj["afi-object-type"].asString();
        AfiObjectId id   = cfg_obj["afi-object-id"].asUInt64();
        std::string name = cfg_obj["afi-object-name"].asString();
        std::string obj  = cfg_obj["afi-object"].asString();

        if (_debugmode.find("debug-afi-objects") != std::string::npos) {
            Log(DEBUG) << "object-id   : " << id;
            Log(DEBUG) << "object-name : " << name;
            Log(DEBUG) << "object-type : " << type;
            Log(DEBUG) << "object      : " << obj;
        }
        
        if (type == "afi-tree") {

            // FIXME magic number 5000
            char bytes_decoded[5000];
            memset(bytes_decoded, 0, sizeof(bytes_decoded));
            int num_decoded_bytes = base64_decode(obj, bytes_decoded, 5000);

            //std::cout << "bytes_decoded: " << bytes_decoded << std::endl;
            //std::cout << "num_decoded_bytes: " << num_decoded_bytes << std::endl;

            juniper::afi_tree::AfiTree tree;
            tree.ParseFromArray(bytes_decoded, num_decoded_bytes);

            //std::cout << "tree.ByteSize(): " << tree.ByteSize() << std::endl;
            ::ywrapper::StringValue key_field = tree.key_field();
            Log(DEBUG) << "key_field: " << key_field.value();

            AfiObjectPtr objPtr(new AfiTree(id, name, tree));

            insert2TreeMap(objPtr);
            
            AftNodeToken objToken = objPtr->bind();
            Log(DEBUG) << "objToken: " << objToken;

            setIngressStartToken(objToken);
            Log(DEBUG) << "_ingressStartToken: " << _ingressStartToken;

        } else {
            Log(ERROR) << "obj-type: " << type << "Not supported yet";
        }
    }
}


void
AfiDeviceCfg::bindAfiObjects()
{
    Log(DEBUG) << BOOST_CURRENT_FUNCTION << "Binding AFI objects";

    jP4Agent->afiClient().setIngressStart(ingressStartToken());
}

