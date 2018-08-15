//
// Juniper P4 Agent
//
/// @file  AfiTreeEncap.cpp
/// @brief Afi Tree and Packet Encapsulation
//
// Created by Sudheendra Gopinath, June 2018
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
#include "AfiTreeEncap.h"
#include "P4Info.h"
#include <cstring>
#include <memory>

#include "Log.h"
#include "Utils.h"
#include "JaegerLog.h"

using namespace juniper::enums;
using namespace juniper::afi_encap_entry;

namespace AFIHAL
{
//
// Description
//
std::ostream &
AfiTreeEncap::description(std::ostream &os) const
{
    os << "_________ AfiTreeEncap _______" << std::endl;
    return os;
}

AfiTreeEncap::AfiTreeEncap(const AfiJsonResource &jsonRes) : AfiObject(jsonRes)
{
    // TBD: FIXME magic number 5000
    char bytes_decoded[5000];
    memset(bytes_decoded, 0, sizeof(bytes_decoded));
    int num_decoded_bytes =
        base64_decode(jsonRes.objStr(), bytes_decoded, 5000);
    _treeEncap.ParseFromArray(bytes_decoded, num_decoded_bytes);

    Log(DEBUG) << "num_decoded_bytes: " << num_decoded_bytes;
    Log(DEBUG) << "treeEncap.ByteSize(): " << _treeEncap.ByteSize();

}

bool
AfiTreeEncap::createChildJsonRes(const uint32_t tId, //P4InfoTablePtr table,
                           const uint32_t aId, //P4InfoActionPtr action,
                           const std::vector<AfiTEntryMatchField> &mfs,
                           const std::vector<AfiAEntry> &aes,
                           Json::Value& result)
{
    Log(DEBUG) << "____ AFI::addTreeEncapEntry ____\n";

    auto table =
        std::dynamic_pointer_cast<P4InfoTable>(P4Info::instance().p4InfoResource(tId));
    if (table == nullptr) {
        Log(ERROR) << "Bad Table ID " << tId;
        return false;
    }

    auto action =
        std::dynamic_pointer_cast<P4InfoAction>(P4Info::instance().p4InfoResource(aId));
    if (action == nullptr) {
        Log(ERROR) << "Bad Action ID " << tId;
        return false;
    }

    int id = 1233457;

    Log(DEBUG) << "____ Encap ____";
    juniper::afi_encap_entry::AfiEncapEntry afiEncapEntryObj;

    for (auto ae : aes) {
        auto id = ae.id();
        std::string name;
        if ((name = action->actionParamName(id)) == "") {
            Log(DEBUG) << "Bad Action Param ID: " << id;
            return false;
        }

        Log(DEBUG) << ae;
        Log(DEBUG) << "Action Param: " << name;

        AfiEncapEntry_AfiKeyKey* keyKey = afiEncapEntryObj.add_afi_key();
        if (name == "port") {
            keyKey->set_field_name(AfiEncapEntryAfiField::AFIENCAPENTRYAFIFIELD_egress_port);
        } else if (name == "smac") {
            keyKey->set_field_name(AfiEncapEntryAfiField::AFIENCAPENTRYAFIFIELD_packet_ether_saddr);
        } else if (name == "dmac") {
            keyKey->set_field_name(AfiEncapEntryAfiField::AFIENCAPENTRYAFIFIELD_packet_ether_daddr);
        } else if (name == "l3_class_id") {
            keyKey->set_field_name(AfiEncapEntryAfiField::AFIENCAPENTRYAFIFIELD_packet_l3_class_id);
        }

        AfiEncapEntry_AfiKey* key = new::AfiEncapEntry_AfiKey();
        ::ywrapper::BytesValue* yv = new ::ywrapper::BytesValue();
        yv->set_value(ae.value());
        key->set_allocated_field_data(yv);
        keyKey->set_allocated_afi_key(key);
    }

    ::ywrapper::StringValue* po = new ::ywrapper::StringValue();
    po->set_value(table->name() + "_encap");
    afiEncapEntryObj.set_allocated_parent_name(po);

    int encapEntrySize = afiEncapEntryObj.ByteSize();
    char *aArray = new char[encapEntrySize];
    afiEncapEntryObj.SerializeToArray(aArray, encapEntrySize);
    std::string eEncoded = base64_encode(aArray, (unsigned int) (encapEntrySize));

    Json::Value eObj;
    std::string eObjName(table->name() +
                         "_entry_encap" +
                         "_" +
                         std::to_string(id+2));
    eObj["afi-object-name"] = eObjName;
    eObj["afi-object-id"] = id + 2;
    eObj["afi-object-type"] = "afi-encap-entry";
    eObj["afi-object"] = eEncoded;
    result.append(eObj);

    Log(DEBUG) << "____ Tree ____";
    juniper::afi_tree_entry::AfiTreeEntry afiTreeEntryObj;

    for (auto mf : mfs) {
        auto id = mf.id();
        std::string name;
        uint32_t bitWidth;

        if (table->matchFieldInfo(id, name, bitWidth) == false) {
            Log(DEBUG) << "Bad Match Field ID: " << id;
            return false;
        }

        Log(DEBUG) << "Match Field : " << name;

        if (mf.type() == AfiTEntryMatchField::MfType::LPM) {
            ::ywrapper::StringValue *pfx = new ::ywrapper::StringValue();
            pfx->set_value(mf.value());
            afiTreeEntryObj.set_allocated_prefix_bytes(pfx);

            ::ywrapper::UintValue *plen = new ::ywrapper::UintValue();
            plen->set_value(mf.len());
            afiTreeEntryObj.set_allocated_prefix_length(plen);
        }

        uint32_t v, m;
        bool ternary = (mf.type() == AfiTEntryMatchField::MfType::TERNARY) ?
                       true : false;

        if (ternary == false) {
            m = 0xffffffff; // TODO: Must be of bitwidth
        }

        if (bitWidth == 0) {
            continue;
        } else if (bitWidth <= 8) {
            v = *((const uint8_t *) &mf.value().c_str()[0]);
            if (ternary == true) {
                m = *((const uint8_t *) &mf.mask().c_str()[0]);
            }
        } else if (bitWidth <= 16) {
            v = *((const uint16_t *) &mf.value().c_str()[0]);
            v = ntohs(v);
            if (ternary == true) {
                m = *((const uint16_t *) &mf.mask().c_str()[0]);
                m = ntohs(m);
            }
        } else if (bitWidth <= 32) {
            v = *((const uint32_t *) &mf.value().c_str()[0]);
            v = ntohl(v);
            if (ternary == true) {
                m = *((const uint32_t *) &mf.mask().c_str()[0]);
                m = ntohl(m);
            }
        } 

        if (bitWidth <= 32) {
            ::ywrapper::UintValue* yv = new ::ywrapper::UintValue();
            yv->set_value(v);

            Log(DEBUG) << "Value: " << v;

            if (name == "local_metadata.vrf_id") {
                afiTreeEntryObj.set_allocated_vrf_id(yv);
            }
        } else if (bitWidth <= 64) {
            unsigned char mc[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
            std::string m(reinterpret_cast<const char *>(mc), 6);

            ::ywrapper::BytesValue* yv = new::ywrapper::BytesValue();
            yv->set_value(mf.value());

            ::ywrapper::BytesValue* ym = new::ywrapper::BytesValue();
            if (ternary == true) {
                m = mf.mask();
            }
            ym->set_value(m);

            if (name == "hdr.ethernet.src_addr") {
            } else if (name == "hdr.ethernet.dst_addr") {
            }
        } else {
            continue;
        }
    }

    ::ywrapper::StringValue* pObj = new ::ywrapper::StringValue();
    pObj->set_value(table->name() + "_tree");
    afiTreeEntryObj.set_allocated_parent_name(pObj);

    ::ywrapper::StringValue* nObj = new ::ywrapper::StringValue();
    nObj->set_value(eObjName);
    afiTreeEntryObj.set_allocated_target_afi_object(nObj);

    int treeEntrySize = afiTreeEntryObj.ByteSize();
    char *mArray = new char[treeEntrySize];
    afiTreeEntryObj.SerializeToArray(mArray, treeEntrySize);
    std::string tEncoded = base64_encode(mArray, (unsigned int) (treeEntrySize));

    Json::Value tObj;
    std::string tObjName(table->name() +
                         "_entry_tree" +
                         "_" +
                         std::to_string(id+1));
    tObj["afi-object-name"] = tObjName;
    tObj["afi-object-id"] = id + 1;
    tObj["afi-object-type"] = "afi-tree-entry";
    tObj["afi-object"] = tEncoded;
    result.append(tObj);

    Log(DEBUG) << "____ TreeEncap ____";
    juniper::afi_tree_encap_entry::AfiTreeEncapEntry afiTreeEncapEntryObj;

    ::ywrapper::StringValue* ptObj = new ::ywrapper::StringValue();
    ptObj->set_value(table->name());
    afiTreeEncapEntryObj.set_allocated_parent_name(ptObj);

    ::ywrapper::StringValue* enObj = new ::ywrapper::StringValue();
    enObj->set_value(eObjName);
    afiTreeEncapEntryObj.set_allocated_encap_entry_object(enObj);

    ::ywrapper::StringValue* trObj = new ::ywrapper::StringValue();
    trObj->set_value(tObjName);
    afiTreeEncapEntryObj.set_allocated_tree_entry_object(trObj);

    int treeEncapEntrySize = afiTreeEncapEntryObj.ByteSize();
    char *teArray = new char[treeEncapEntrySize];
    afiTreeEncapEntryObj.SerializeToArray(teArray, treeEncapEntrySize);
    std::string teEncoded = base64_encode(teArray, (unsigned int) (treeEncapEntrySize));

    Json::Value teObj;
    std::string teObjName(table->name() +
                         "_entry" +
                         "_" +
                         std::to_string(id+1));
    teObj["afi-object-name"] = teObjName;
    teObj["afi-object-id"] = id + 1;
    teObj["afi-object-type"] = "afi-tree-encap-entry";
    teObj["afi-object"] = teEncoded;
    result.append(teObj);

    return true;
}

}  // namespace AFIHAL
