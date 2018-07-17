//
// Juniper P4 Agent
//
/// @file  BrcmCapEntry.cpp
/// @brief Brcm CAP Entry
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

#include "BrcmIncludes.h"

namespace BRCMHALP {

void BrcmCapEntry::_bind()
{
    std::cout << "BrcmCapEntry: _bind" << std::endl;
    Log(DEBUG)<< "Pushing BrcmCapEntry to ASIC";

    ::ywrapper::StringValue po = _capEntry.parent_name();
    BrcmCapPtr co = std::dynamic_pointer_cast<BrcmCap>(
        AFIHAL::Afi::instance().getAfiObject(po.value()));
    if (co == nullptr) {
        Log(ERROR) << ": Unable to find afi-cap object " << po.value();
        return;
    }

    ::ywrapper::StringValue mo = _capEntry.match_object();
    BrcmCapEntryMatchPtr cemo = std::dynamic_pointer_cast<BrcmCapEntryMatch>(
        AFIHAL::Afi::instance().getAfiObject(mo.value()));
    if (cemo == nullptr) {
        Log(ERROR) << ": Unable to find afi-cap-entry-match object "
                   << mo.value();
        return;
    }

    ::ywrapper::StringValue ao = _capEntry.action_object();
    BrcmCapEntryActionPtr ceao = std::dynamic_pointer_cast<BrcmCapEntryAction>(
        AFIHAL::Afi::instance().getAfiObject(ao.value()));
    if (ceao == nullptr) {
        Log(ERROR) << ": Unable to find afi-cap-entry-action object "
                   << ao.value();
        return;
    }

    ::ywrapper::UintValue gid = co->gid();
    Log(DEBUG) << "group_id: " << gid.value();
    _fpe = Fp::createRule(gid.value());

    ::ywrapper::UintValue gp = co->gp();
    Log(DEBUG) << "group_priority: " << gp.value();

    ::ywrapper::UintValue et = cemo->capEntryMatch.ethertype();
    Log(DEBUG) << "ethertype: " << et.value();
    _fpe->addMatchField(Fp::MatchKey::etherType, et.value(), 0xffff);

    ::ywrapper::BytesValue sma = cemo->capEntryMatch.source_mac_address();
    Log(DEBUG) << "source mac address: " << sma.value();
    uint8_t m[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    _fpe->addMatchFieldStr(Fp::MatchKey::srcMac, sma.value().c_str(), m);

    const ::ywrapper::UintValue& da = cemo->capEntryMatch.destination_ipv4_address();
    Log(DEBUG) << "destination address: " << da.value();
    _fpe->addMatchField(Fp::MatchKey::inetDstAddr, da.value(), 0xffffffff);

    ::ywrapper::IntValue vrf = ceao->capEntryAction.vrf();
    Log(DEBUG) << "vrf: " << vrf.value();
    _fpe->addAction(Fp::ActionKey::vrfId, vrf.value(), 0xffff);

    ::ywrapper::IntValue cid = ceao->capEntryAction.destination_class_id();
    Log(DEBUG) << "class id: " << cid.value();

    _fpe->setPriority(2);
    _fpe->install();
#ifdef SUD_T
    std::vector<Fp::MatchKey> key;

    ::ywrapper::UintValue et = _capEntryMatch.ethertype();
    Log(DEBUG) << "ethertype: " << et.value();
    if (et.value())
        key.push_back(Fp::MatchKey::etherType);

    ::ywrapper::BoolValue smac = _capEntryMatch.source_mac_address();
    Log(DEBUG) << "source_mac_address: " << smac.value();
    if (smac.value())
        key.push_back(Fp::MatchKey::srcMac);

    ::ywrapper::BoolValue daddr = _capEntryMatch.destination_ipv4_address();
    Log(DEBUG) << "destination_ipv4_address: " << daddr.value();
    if (daddr.value())
        key.push_back(Fp::MatchKey::inetDstAddr);

    ::ywrapper::IntValue vrf = _capEntryAction.vrf();
    Log(DEBUG) << "vrf: " << vrf.value();
#endif // SUD_T

    //int ret = Fp::addGroup(Fp::FpType::VFP, gid.value(), gp.value(), key);
    //Log(DEBUG) << "Fp::addGroup: " << ret;

#ifdef SUD
    ::ywrapper::StringValue key_field = _filter.key_field();
    Log(DEBUG) << "key_field: " << key_field.value();

    std::stringstream ks;
    ks  << key_field.value();
    JaegerLog::getInstance()->Log("Brcm:BrcmFilter:Key Field", ks.str());

    // Write into file for Brcm test
    gtestFile.open("../BrcmTest.txt", std::fstream::app);
    gtestFile << "key_field: " << key_field.value() << "\n";
    gtestFile.close();
#endif // SUD
}

//  
// Description
//  
std::ostream & BrcmCapEntry::description (std::ostream &os) const
{
    os << "_________ BrcmCapEntry _______"   << std::endl;
    os << "Name                :" << this->name()  << std::endl;
    os << "Id                  :" << this->id()    << std::endl;
    
    return os;
}

}  // namespace BRCMHALP
