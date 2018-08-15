//
// Juniper P4 Agent
//
/// @file  AfiEncap.cpp
/// @brief Afi Packet Encapsulation
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
#include "AfiEncap.h"
//#include "AfiEncapEntry.h"
#include "P4Info.h"
#include <cstring>
#include <memory>

#include "Log.h"
#include "Utils.h"
#include "JaegerLog.h"

namespace AFIHAL
{
//
// Description
//
std::ostream &
AfiEncap::description(std::ostream &os) const
{
    os << "_________ AfiEncap _______" << std::endl;
    return os;
}

AfiEncap::AfiEncap(const AfiJsonResource &jsonRes) : AfiObject(jsonRes)
{
    // TBD: FIXME magic number 5000
    char bytes_decoded[5000];
    memset(bytes_decoded, 0, sizeof(bytes_decoded));
    int num_decoded_bytes =
        base64_decode(jsonRes.objStr(), bytes_decoded, 5000);
    _encap.ParseFromArray(bytes_decoded, num_decoded_bytes);

    Log(DEBUG) << "num_decoded_bytes: " << num_decoded_bytes;
    Log(DEBUG) << "encap.ByteSize(): " << _encap.ByteSize();

}

#ifdef SUD_T
bool
AfiCap::createChildJsonRes(const uint32_t tId, //P4InfoTablePtr table,
                           const uint32_t aId, //P4InfoActionPtr action,
                           const std::vector<AfiTEntryMatchField> &mfs,
                           const std::vector<AfiAEntry> &aes,
                           Json::Value& result)
{
    Log(DEBUG) << "____ AFI::addCapEntry ____\n";

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
    Log(DEBUG) << "____ Match Keys ____";

    juniper::afi_cap_entry_match::AfiCapEntryMatch afiMatchObj;
    for (auto mf : mfs) {
        auto id = mf.id();
        std::string name;
        uint32_t bitWidth;
        if (table->matchFieldInfo(id, name, bitWidth) == false) {
            Log(DEBUG) << "Bad Match Field ID: " << id;
            return false;
        }

        Log(DEBUG) << "Match Field : " << name;

        uint32_t v, m;
        bool ternary = (mf.type() == AfiTEntryMatchField::MfType::TERNARY) ?
                       true : false;

        if (ternary == false) {
            m = 0xffffffff;
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

            ::ywrapper::UintValue* ym = new ::ywrapper::UintValue();
            ym->set_value(m);

            Log(DEBUG) << "Value, mask : " << v << " " << m;

            if (name == "hdr.ethernet.ether_type") {
                afiMatchObj.set_allocated_ethertype(yv);
                afiMatchObj.set_allocated_ethertype_mask(ym);
            } else if (name == "hdr.vlan_tag[0].vid") {
                afiMatchObj.set_allocated_outer_vlan_id(yv);
                afiMatchObj.set_allocated_outer_vlan_id_mask(ym);
            } else if (name == "hdr.vlan_tag[0].pcp") {
                afiMatchObj.set_allocated_outer_vlan_dot1p(yv);
                afiMatchObj.set_allocated_outer_vlan_dot1p_mask(ym);
            } else if (name == "hdr.ipv4_base.ttl") {
                afiMatchObj.set_allocated_ipv4_ttl(yv);
                afiMatchObj.set_allocated_ipv4_ttl_mask(ym);
            } else if (name == "hdr.ipv4_base.protocol") {
                afiMatchObj.set_allocated_ip_protocol(yv);
                afiMatchObj.set_allocated_ip_protocol_mask(ym);
            } else if (name == "hdr.ipv4_base.diffserv") {
            } else if (name == "hdr.ipv4_base.src_addr") {
                afiMatchObj.set_allocated_source_ipv4_address(yv);
                afiMatchObj.set_allocated_source_ipv4_address_mask(ym);
            } else if (name == "hdr.ipv4_base.dst_addr") {
                afiMatchObj.set_allocated_destination_ipv4_address(yv);
                afiMatchObj.set_allocated_destination_ipv4_address_mask(ym);
            } else if (name == "hdr.standard_metadata.ingress_port") {
            } else if (name == "hdr.ipv6_base.traffic_class") {
            } else if (name == "hdr.ipv6_base.dst_addr") {
            } else if (name == "local_metadata.l4_src_port") {
                afiMatchObj.set_allocated_l4_source_port_mask(yv);
                afiMatchObj.set_allocated_l4_source_port_mask(ym);
            } else if (name == "local_metadata.l4_dst_port") {
                afiMatchObj.set_allocated_l4_destination_port(yv);
                afiMatchObj.set_allocated_l4_destination_port_mask(ym);
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
                afiMatchObj.set_allocated_source_mac_address(yv);
                afiMatchObj.set_allocated_source_mac_address_mask(ym);
            } else if (name == "hdr.ethernet.dst_addr") {
                afiMatchObj.set_allocated_destination_mac_address(yv);
                afiMatchObj.set_allocated_destination_mac_address_mask(ym);
            }
        } else {
            continue;
        }
    }

    int matchSize = afiMatchObj.ByteSize();
    char *mArray = new char[matchSize];
    afiMatchObj.SerializeToArray(mArray, matchSize);
    std::string mEncoded = base64_encode(mArray, (unsigned int) (matchSize));

    Json::Value mObj;
    std::string mObjName(table->name() +
                         "_entry_match" +
                         "_" +
                         std::to_string(id+1));
    mObj["afi-object-name"] = mObjName;
    mObj["afi-object-id"] = id + 1;
    mObj["afi-object-type"] = "afi-cap-entry-match";
    mObj["afi-object"] = mEncoded;
    result.append(mObj);

    Log(DEBUG) << "____ Action Keys ____";

    juniper::afi_cap_entry_action::AfiCapEntryAction afiActionObj;
    for (auto ae : aes) {
        auto id = ae.id();
        std::string name;
        if ((name = action->actionParamName(id)) == "") {
            Log(DEBUG) << "Bad Action Param ID: " << id;
            return false;
        }

        std::cout << ae;

        Log(DEBUG) << "Action Param: " << name;

        if (name == "vrf_id") {
            ::ywrapper::UintValue* yv = new ::ywrapper::UintValue();
            uint32_t v;
            str2Uint(ae.value(), v);
            yv->set_value(v);
            afiActionObj.set_allocated_vrf(yv);
        } else if (name == "class_id_value") {
            ::ywrapper::UintValue* yv = new ::ywrapper::UintValue();
            uint8_t v;
            str2Uint(ae.value(), v);
            yv->set_value(v);
            afiActionObj.set_allocated_destination_class_id(yv);
        } else if (name == "hdr.ethernet.src_addr") {
        } else if (name == "hdr.ipv4_base.diffserv") {
        } else if (name == "hdr.ipv4_base.dst_addr") {
        } else if (name == "hdr.standard_metadata.ingress_port") {
        } else if (name == "hdr.ipv6_base.traffic_class") {
        } else if (name == "hdr.ipv6_base.dst_addr") {
        } else if (name == "hdr.ipv4_base.src_addr") {
        }
    }

    int actionSize = afiActionObj.ByteSize();
    char *aArray = new char[actionSize];
    afiActionObj.SerializeToArray(aArray, actionSize);
    std::string aEncoded = base64_encode(aArray, (unsigned int) (actionSize));

    Json::Value aObj;
    std::string aObjName(table->name() +
                         "_entry_action" +
                         "_" +
                         std::to_string(id+2));
    aObj["afi-object-name"] = aObjName;
    aObj["afi-object-id"] = id + 2;
    aObj["afi-object-type"] = "afi-cap-entry-action";
    aObj["afi-object"] = aEncoded;
    result.append(aObj);

    juniper::afi_cap_entry::AfiCapEntry afiCapEntryObj;

    ::ywrapper::StringValue* po = new ::ywrapper::StringValue();
    po->set_value(table->name());
    afiCapEntryObj.set_allocated_parent_name(po);

    ::ywrapper::StringValue* mo = new ::ywrapper::StringValue();
    mo->set_value(mObjName);
    afiCapEntryObj.set_allocated_match_object(mo);

    ::ywrapper::StringValue* ao = new ::ywrapper::StringValue();
    ao->set_value(aObjName);
    afiCapEntryObj.set_allocated_action_object(ao);

    int eSize = afiCapEntryObj.ByteSize();
    char *eArray = new char[eSize];
    afiCapEntryObj.SerializeToArray(eArray, eSize);
    std::string encoded = base64_encode(eArray, (unsigned int) (eSize));

    Json::Value eObj;
    eObj["afi-object-type"] = "afi-cap-entry";
    eObj["afi-object-name"] = table->name() +
                              "_entry" +
                              "_" +
                              std::to_string(id);
    eObj["afi-object-id"] = id;
    eObj["afi-object"] = encoded;

    result.append(eObj);

    return true;
}
#endif // SUD_T

}  // namespace AFIHAL
