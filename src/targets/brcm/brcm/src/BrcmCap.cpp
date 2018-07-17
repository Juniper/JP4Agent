//
// Juniper P4 Agent
//
/// @file  BrcmCap.cpp
/// @brief Brcm CAP
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
#include "BrcmFp.h"

namespace BRCMHALP {

void BrcmCap::_bind()
{
    std::vector<Fp::MatchKey> key;

    std::cout << "BrcmCap: _bind" << std::endl;
    Log(DEBUG)<< "Pushing BrcmCap to ASIC";

    ::ywrapper::UintValue gid = _cap.group_id();
    Log(DEBUG) << "group_id: " << gid.value();

    ::ywrapper::UintValue gp = _cap.group_priority();
    Log(DEBUG) << "group_priority: " << gp.value();

    ::ywrapper::StringValue mo = _cap.match_object();
    BrcmCapMatchPtr cmo = std::dynamic_pointer_cast<BrcmCapMatch>(
        AFIHAL::Afi::instance().getAfiObject(mo.value()));

    if (cmo == nullptr) {
        Log(ERROR) << ": Unable to find afi-cap-match object " << mo.value();
        return;
    }

    ::ywrapper::BoolValue et = cmo->capMatch.ethertype();
    Log(DEBUG) << "ethertype: " << et.value();
    if (et.value())
        key.push_back(Fp::MatchKey::etherType);

    ::ywrapper::BoolValue smac = cmo->capMatch.source_mac_address();
    Log(DEBUG) << "source_mac_address: " << smac.value();
    if (smac.value())
        key.push_back(Fp::MatchKey::srcMac);

    ::ywrapper::BoolValue daddr = cmo->capMatch.destination_ipv4_address();
    Log(DEBUG) << "destination_ipv4_address: " << daddr.value();
    if (daddr.value())
        key.push_back(Fp::MatchKey::inetDstAddr);

    ::ywrapper::StringValue ao = _cap.action_object();
    BrcmCapActionPtr cao = std::dynamic_pointer_cast<BrcmCapAction>(
        AFIHAL::Afi::instance().getAfiObject(ao.value()));
    if (cao == nullptr) {
        Log(ERROR) << ": Unable to find afi-cap-action object " << ao.value();
        return;
    }

    ::ywrapper::BoolValue vrf = cao->capAction.vrf();
    Log(DEBUG) << "vrf: " << vrf.value();

    int ret = Fp::addGroup(Fp::FpType::VFP, gid.value(), gp.value(), key);
    Log(DEBUG) << "Fp::addGroup: " << ret;

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
std::ostream & BrcmCap::description (std::ostream &os) const
{
    os << "_________ BrcmCap _______"   << std::endl;
    os << "Name                :" << this->name()  << std::endl;
    os << "Id                  :" << this->id()    << std::endl;
    
    return os;
}

}  // namespace BRCMHALP
