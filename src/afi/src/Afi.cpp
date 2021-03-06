//
// Juniper P4 Agent
//
/// @file  Afi.cpp
/// @brief Afi
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

#include "Afi.h"
#include <string>

#include "AfiDM.h"
#include "Utils.h"

namespace AFIHAL
{
bool
Afi::handleAfiJsonObject(const Json::Value &cfg_obj, const bool &pipeline_stage)
{
    Log(DEBUG) << "___ AFI::handleAfiJsonObject ___\n";

    // TBD: Revisit: on stack
    AfiJsonResource res(cfg_obj["afi-object-type"].asString(),
                        cfg_obj["afi-object-id"].asUInt64(),
                        cfg_obj["afi-object-name"].asString(),
                        cfg_obj["afi-object"].asString());

    //
    // Create Afi Object
    //
    AfiObjectPtr afiObj = _afiDevice->handleDMObject(res, pipeline_stage);

    //
    // If we didn't create a object, we fail the operation
    //
    if (afiObj == nullptr) {
        Log(ERROR) << "Error creating afi object";
        return false;
    }

    return true;
}

bool
Afi::handlePipelineConfig(const Json::Value &cfg_root)
{
    Log(DEBUG) << "____ AFI:: handlePipelineConfig ____\n";
    for (Json::Value::ArrayIndex i = 0; i != cfg_root.size(); i++) {
        const Json::Value &cfg_obj = cfg_root[i];
        auto               status  = handleAfiJsonObject(cfg_obj, true);
        if (true != status) {
            Log(ERROR) << "Error handling afi json object";
            return status;
        }
    }

    _afiDevice->bindAfiObjects();
    return true;
}

bool
Afi::addAfiTree(const std::string &aftTreeName, const std::string &keyField,
                const int protocol, const std::string &defaultNextObject,
                const unsigned int treeSize)
{
    Log(DEBUG) << "____ AFI::addAfiTree ____\n";
    Log(DEBUG) << "aftTreeName: " << aftTreeName;
    Log(DEBUG) << "keyField   : " << keyField;

    Json::Value afiTreeJsonObject;

    afiTreeJsonObject["afi-object-type"] = "afi-tree";

    afiTreeJsonObject["afi-object-name"] = aftTreeName;
    afiTreeJsonObject["afi-object-id"]   = 1000001;

    juniper::afi_tree::AfiTree afiTree;

    ::ywrapper::StringValue *tree_name = new ::ywrapper::StringValue();
    tree_name->set_value(aftTreeName);
    afiTree.set_allocated_name(tree_name);

    ::ywrapper::StringValue *key_field = new ::ywrapper::StringValue();
    key_field->set_value(keyField);
    afiTree.set_allocated_key_field(key_field);

    ::ywrapper::UintValue *proto = new ::ywrapper::UintValue();
    proto->set_value(protocol);
    afiTree.set_allocated_proto(proto);

    ::ywrapper::StringValue *default_next_node = new ::ywrapper::StringValue();
    default_next_node->set_value(defaultNextObject);
    afiTree.set_allocated_default_next_node(default_next_node);

    ::ywrapper::UintValue *size = new ::ywrapper::UintValue();
    size->set_value(treeSize);
    afiTree.set_allocated_size(size);

    int   tree_size = afiTree.ByteSize();
    char *array     = new char[tree_size];
    std::cout << "tree_size: " << tree_size << std::endl;
    afiTree.SerializeToArray(array, tree_size);

    std::string encoded = base64_encode(array, (unsigned int)tree_size);
    std::cout << "encoded: " << encoded << std::endl << std::endl;

    afiTreeJsonObject["afi-object"] = encoded;

    auto status = handleAfiJsonObject(afiTreeJsonObject, false);
    if (true != status) {
        Log(ERROR) << "Error handling afi tree entry json object";
        return status;
    }

    return true;
}

bool
Afi::addEntry(const std::string &keystr, int pLen)
{
    Log(DEBUG) << "____ AFI::addEntry ____\n";
    Log(DEBUG) << "keystr : " << keystr;
    Log(DEBUG) << "pLen   : " << pLen;

    Json::Value afiTreeEntryJsonObject;

    afiTreeEntryJsonObject["afi-object-type"] = "afi-tree-entry";

    std::string afi_object_name               = "entry1";
    afiTreeEntryJsonObject["afi-object-name"] = afi_object_name;
    afiTreeEntryJsonObject["afi-object-id"]   = 1233456;

    juniper::afi_tree_entry::AfiTreeEntry afiTreeEntry;

    ::ywrapper::StringValue *entry_name = new ::ywrapper::StringValue();
    entry_name->set_value("entry1");
    afiTreeEntry.set_allocated_name(entry_name);

    ::ywrapper::StringValue *parent_name = new ::ywrapper::StringValue();
    parent_name->set_value("ipv4_lpm");
    afiTreeEntry.set_allocated_parent_name(parent_name);

    ::ywrapper::StringValue *target_afi_object = new ::ywrapper::StringValue();
    target_afi_object->set_value("etherencap1");
    afiTreeEntry.set_allocated_target_afi_object(target_afi_object);

    ::ywrapper::StringValue *prefix_bytes = new ::ywrapper::StringValue();
    // prefix_bytes->set_value("10.10.10.10/16");
    prefix_bytes->set_value(keystr);
    afiTreeEntry.set_allocated_prefix_bytes(prefix_bytes);

    ::ywrapper::UintValue *prefix_length = new ::ywrapper::UintValue();
    prefix_length->set_value(pLen);
    afiTreeEntry.set_allocated_prefix_length(prefix_length);

    int   entry_size = afiTreeEntry.ByteSize();
    char *array      = new char[entry_size];
    std::cout << "entry_size: " << entry_size << std::endl;
    afiTreeEntry.SerializeToArray(array, entry_size);

    std::string encoded = base64_encode(array, (unsigned int)entry_size);
    std::cout << "encoded: " << encoded << std::endl << std::endl;

    afiTreeEntryJsonObject["afi-object"] = encoded;

    auto status = handleAfiJsonObject(afiTreeEntryJsonObject, false);
    if (true != status) {
        Log(ERROR) << "Error handling afi tree entry json object";
        return status;
    }

    return true;
}

bool
Afi::afiAddObjEntry (const uint32_t tId,
                     const uint32_t aId,
                     const std::vector<AfiTEntryMatchField> &mfs,
                     const std::vector<AfiAEntry> &aes)
{
    Log(DEBUG) << "____ AFI::addObjEntry ____\n";
    Log(DEBUG) << "Table ID  : " << tId;
    Log(DEBUG) << "Action ID : " << aId;

    auto table =
        std::dynamic_pointer_cast<P4InfoTable>(P4Info::instance().p4InfoResource(tId));
    if (table == nullptr) {
        Log(ERROR) << "Bad Table ID " << tId;
        return false;
    }

    for (auto mf : mfs) {
         auto id = mf.id();
         std::string name;
         uint32_t bitWidth;
         table->matchFieldInfo(id, name, bitWidth);

         Log(DEBUG) << "Match Field Name: "     << name;
         Log(DEBUG) << "Match Field BitWidth: " << bitWidth;
         Log(DEBUG) << mf;
    }

    auto action =
        std::dynamic_pointer_cast<P4InfoAction>(P4Info::instance().p4InfoResource(aId));
    if (action == nullptr) {
        Log(ERROR) << "Bad Action ID " << tId;
        return false;
    }

    auto tName = table->name();
    auto aName = action->name();
    Log(DEBUG) << "Table Name: "  << tName;
    Log(DEBUG) << "Action Name: " << aName;

    // Get parent AFI object.
    AfiObjectPtr afiPObj = AFIHAL::Afi::instance().getAfiObject(tName);
    if (afiPObj == nullptr) {
        Log(ERROR) << "No AFI object for table: " << tName;
        return false;
    }

    Log(DEBUG) << "Table Object Type: " << afiPObj->type();

    // Prepare AFI object.
    Json::Value eObjs;

    if (afiPObj->createChildJsonRes(tId, aId, mfs, aes, eObjs) == false) {
        return false;
    }

    // Add all objects in array.
    for (Json::Value::ArrayIndex i = 0; i != eObjs.size(); i++) {
        const Json::Value &eObj = eObjs[i];
        auto status = handleAfiJsonObject(eObj, false);
        if (true != status) {
            Log(ERROR) << "Error handling afi tree entry json object";
            return status;
        }
    }

    return true;
}

}  // namespace AFIHAL
