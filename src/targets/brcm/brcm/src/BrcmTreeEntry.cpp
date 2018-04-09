//
// Juniper P4 Agent
//
/// @file  BrcmTreeEntry.cpp
/// @brief Brcm tree entry
//
// Created by Sudheendra Gopinath, March 2018
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

#include <netinet/ether.h>
#include <vector>

#include "Brcm.h"
#include "JaegerLog.h"

#include "BrcmL3Intf.h"
#include "BrcmNh.h"
#include "BrcmRt.h"

namespace BRCMHALP {

//BrcmNodeToken BrcmTreeEntry::bind(void)
void BrcmTreeEntry::_bind()
{
    std::cout << "BrcmTreeEntry: _bind" << std::endl;
    Log(DEBUG)<< "Pushing BrcmTreeEntry to ASIC";

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

    // AfiObjectPtr afiObjPtr =
    // AFIHAL::Afi::instance().getAfiObject(entry_name.value());
    
    //AfiTreePtr afiTreePtr = std::dynamic_pointer_cast<AfiTree>(afiObjPtr);
    BrcmTreePtr BrcmTreePtr = std::dynamic_pointer_cast<BrcmTree>(
         AFIHAL::Afi::instance().getAfiObject(parent_name.value()));

    if (BrcmTreePtr == nullptr) {
        Log(ERROR) << "Could not find parent AfiTree";
        return;
    }

    std::cout<<"BrcmTree :" << BrcmTreePtr << "\n";

    std::stringstream es;
    es << entry_name.value();
    JaegerLog::getInstance()->Log("Brcm:BrcmTreeEntry:Name", es.str());

    std::stringstream ps;
    ps << parent_name.value();
    JaegerLog::getInstance()->Log("Brcm:BrcmTreeEntry:Parent Name", ps.str());

    std::stringstream as;
    as << target_afi_object.value();
    JaegerLog::getInstance()->Log("Brcm:BrcmTreeEntry:Target AFI Object", as.str());

    JaegerLog::getInstance()->finishSpan();

    uint32_t              dstAddr;
    uint16_t              port;
    bcm_if_t              bcmNhid = 0;

    memcpy(&dstAddr, prefix_bytes_str.c_str(), prefix_bytes_str.size());

    // Temporary
    char m[24];
    if (dstAddr == 0x022c2c2c) {
        strcpy(m, "88:a2:5e:91:c0:a9");
        port = 9;
    } else if (dstAddr == 0x02373737) {
        strcpy(m, "88:a2:5e:91:75:ff");
        port = 13;
    } else if (dstAddr == 0x012c2c2c) {
        strcpy(m, "88:a2:5e:91:a2:a8");
        port = 0;
    } else if (dstAddr == 0x01373737) {
        strcpy(m, "88:a2:5e:91:a2:a9");
        port = 0;
    }

    std::cout << "\n";
    Log(DEBUG) << "prefix_bytes_str.c_str():" << prefix_bytes_str.c_str();
    Log(DEBUG) << "prefix_bytes_str.size() :" << prefix_bytes_str.size();
    Log(DEBUG) << "prefix_length.value()   :" << prefix_length.value();
    //Log(DEBUG) << "dest_ip_addr            :" << std::hex(dstAddr);
    Log(DEBUG) << "dst_mac_addr            :" << m;
    Log(DEBUG) << "dst_port                :" << port;

    if (port == 0) {
        // cpu port
        bcmNhid = 100002;
    } else {
        // not the CPU port, so L3 interface must exist
        std::shared_ptr<BrcmL3Intf> brcmL3Intf = BrcmL3Intf::get(port);
        if (brcmL3Intf == nullptr) {
            std::cout << "L3 interface does not exist for port "
                      << port
                      << std::endl;
            return;
        }

        BrcmNhParamsUcast nhParams((uint8_t *) ether_aton(m),
                                   brcmL3Intf->getVlanToken(),
                                   port);
        BrcmNhUcast::add(nhParams, &bcmNhid);
    }

    std::cout << "bcmNhid = " << bcmNhid << std::endl;

    BrcmRtParamsV4 rtParams(0, bcmNhid, dstAddr, prefix_length.value());
    BrcmRtV4::add(rtParams);
}

//  
// Description
//  
std::ostream & BrcmTreeEntry::description (std::ostream &os) const
{
    os << "_________ BrcmTreeEntry _______"   << std::endl;
    os << "Name                :" << this->name()  << std::endl;
    os << "Id                  :" << this->id()    << std::endl;
    //os << "_defaultTargetToken :" << this->_defaultTargetToken << std::endl;
    //os << "_token              :" << this->_token << std::endl;
    
#if 0
    os << "match_type     :" << _match_type   << std::endl;
    os << "table_type     :" << _table_type   << std::endl;
    os << "_matchKey      :"   << _matchKey   << std::endl;
    os << "Key match kind :" << _keyMatchKind << std::endl;
#endif
    return os;
}

}  // namespace BRCMHALP
