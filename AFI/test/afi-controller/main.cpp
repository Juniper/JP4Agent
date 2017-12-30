//
// Juniper AFI Controller
//
/// @file  main.cpp
/// @brief Juniper AFI Controller...
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


#include <iostream>
#include <string>
#include <boost/archive/tmpdir.hpp>
#include <fstream>
#include <jsoncpp/json/json.h>

#include <boost/archive/xml_oarchive.hpp>

#include "afi_tree/afi_tree.pb.h"
#include "base64.h"

int main(int argc, char *argv[])
{   
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout<< "============= AFI Agent ===============\n";

    juniper::afi_tree::AfiTree tree;

    ::ywrapper::StringValue *key_field = new ::ywrapper::StringValue();
    //key_field->set_value("packet.lookupkey");
    key_field->set_value("packet.ip4.daddr");
    tree.set_allocated_key_field(key_field);


    ::ywrapper::StringValue *table_name = new ::ywrapper::StringValue();
    table_name->set_value("ipv4_lpm");
    tree.set_allocated_name(table_name);

    ::ywrapper::UintValue* table_size = new ::ywrapper::UintValue();
    table_size->set_value(10);
    tree.set_allocated_size(table_size);

    //std::ostringstream buf;

    int size = tree.ByteSize();
    char* array = new char[size];
    std::cout << "size: " << size << std::endl;
    tree.SerializeToArray(array, size);

    std::string encoded = base64_encode(array, (unsigned int)size);
    std::cout << "encoded: " << encoded << std::endl << std::endl;

    //Create JSON object
    Json::Value cfg;
    Json::Value afiobj1;

    afiobj1["afi-object"] = encoded;
    afiobj1["afi-object-id"] = 101;
    afiobj1["afi-object-name"] = "ipv4_lpm";
    afiobj1["afi-object-type"] = "afi-tree";

    cfg.append(afiobj1);

#if  0
    Json::Value afiobj2;

    afiobj2["afi-object"] = encoded;
    afiobj2["afi-object-id"] = 102;
    afiobj2["afi-object-name"] = "send_frame";
    afiobj2["afi-object-type"] = "afi-tree";

    cfg.append(afiobj2);
#endif

    std::ofstream file_id;
    file_id.open("./afi_switch.json");

    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(cfg);

    file_id.close();
}
 
