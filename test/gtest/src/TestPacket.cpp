//
// TestPacket.cpp
//
// JP4Agent GTESTs
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

#include "TestPacket.h"

//
// Test Packet Library
//
TestPacketLibrary testPacketLibrary;

void
TestPacketLibrary::buildTestPacketLibrary(void)
{
    TestPacket *testPkt;

    //
    // TEST_PKT_ID_IPV4_ECHO_REQ_TO_TAP1
    //
    // Mac tap1 32:26:0a:2e:cc:f1
    // Src IP : 103.30.10.1
    // Dst IP : 103.30.10.3
    //
    testPkt = &_testPacketLibrary[TEST_PKT_ID_IPV4_ECHO_REQ_TO_TAP1];

    // testPkt->setDstMac("32:26:0a:2e:cc:f1");
    testPkt->setDstMac("33:22:0a:2e:ff:f1");
    testPkt->setSrcMac("32:26:0A:2E:aa:F1");
    testPkt->setEtherType(ETH_P_IP);  // 0x0800
    testPkt->setEtherPayloadStr(
        "4500 0054 dacc 4000 4001 7d9c"
        "671e 0a01 671e 0a03"
        "0800 5cba 492b 3942 ee7c 5658 0000"
        "0000 0a30 0b00 0000 0000 1011 1213 1415"
        "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
        "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
        "3637");

    //
    // TEST_PKT_ID_IPV4_ECHO_REQ_TO_TAP2
    //
    // Mac tap2 7a:44:b9:85:3e:10
    // Src IP : 103.30.80.1
    // Dst IP : 103.30.80.3
    testPkt = &_testPacketLibrary[TEST_PKT_ID_IPV4_ECHO_REQ_TO_TAP2];

    testPkt->setDstMac("7A:44:B9:85:3E:10");
    testPkt->setSrcMac("32:26:0A:2E:FF:F3");
    testPkt->setEtherType(ETH_P_IP);  // 0x0800
    testPkt->setEtherPayloadStr(
        "4500 0054 dacc 4000 4001 f19b"
        "671e 5001 671e 5003 "
        "0800 5cba 492b 3942 ee7c 5658 0000"
        "0000 0a30 0b00 0000 0000 1011 1213 1415"
        "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
        "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
        "3637");

    // tap2 mac     : 32:26:0a:2e:cc:f2
    // ge-0/0/2 mac : 32:26:0a:2e:aa:f2
    //
    // TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3
    //
    // tap3 mac : 7a:44:b9:85:3e:10
    //
    // Src IP : 103.30.20.2
    // Dst IP : 103.30.30.3
    testPkt = &_testPacketLibrary[TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3];

    testPkt->setDstMac("32:26:0a:2e:aa:f2");
    testPkt->setSrcMac("32:26:0A:2E:BB:F2");
    testPkt->setEtherType(ETH_P_IP);  // 0x0800
    testPkt->setEtherPayloadStr(
        "4500 0054 dacc 4000 4001 5f9c"
        "671e 1401 671e 1E03"
        "0800 5cba 492b 3942 ee7c 5658 0000"
        "0000 0a30 0b00 0000 0000 1011 1213 1415"
        "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
        "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
        "3637");

    // tap2 mac     : 32:26:0a:2e:cc:f2
    // xe-0/0/0:1   : 00:05:86:75:5c:01
    //
    // VMXZT_TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3
    //
    // tap3 mac : 7a:44:b9:85:3e:10
    //
    // Src IP : 103.30.20.2
    // Dst IP : 103.30.30.3
    testPkt =
        &_testPacketLibrary[VMXZT_TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3];

    testPkt->setDstMac("2C:6B:F5:75:5c:01");
    testPkt->setSrcMac("32:26:0A:2E:BB:F2");
    testPkt->setEtherType(ETH_P_IP);  // 0x0800
    testPkt->setEtherPayloadStr(
        "4500 0054 dacc 4000 4001 5f9c"
        "671e 1401 671e 1E03"
        "0800 5cba 492b 3942 ee7c 5658 0000"
        "0000 0a30 0b00 0000 0000 1011 1213 1415"
        "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
        "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
        "3637");

    //
    // TEST_PKT_ID_PUNT_ICMP_ECHO
    //
    // Src IP : 103.30.00.2
    // Dst IP : 103.30.00.1
    testPkt = &_testPacketLibrary[TEST_PKT_ID_PUNT_ICMP_ECHO];

    testPkt->setEtherType(ETH_P_IP);  // 0x0800
    testPkt->setDstMac("33:22:0A:2E:AA:F1");
    testPkt->setEtherPayloadStr(
        "4500 0054 a038 4000 4001 cc31"
        "671e 0002 671e 0001 "
        "0800 bf46 3271 0001 06f8 8558"
        "0000 0000 bb23 0000 0000 0000 1011 1213"
        "1415 1617 1819 1a1b 1c1d 1e1f 2021 2223"
        "2425 2627 2829 2a2b 2c2d 2e2f 3031 3233"
        "3435 3637");

    //
    // TEST_PKT_ID_IPV4_VLAN
    //
    testPkt = &_testPacketLibrary[TEST_PKT_ID_IPV4_VLAN];

    testPkt->setEtherType(ETH_P_8021Q);  // 0x8100
    testPkt->setDstMac("33:22:0a:2e:ff:f1");
    testPkt->setEtherPayloadStr(
        "000b 0800 4500 0054 40b3 4000"
        "4001 b3b6 671e 3c02 671e 3c01 0800 fbe6"
        "3edd 8281 0b21 4158 0000 0000 2d6e 0200"
        "0000 0000 1011 1213 1415 1617 1819 1a1b"
        "1c1d 1e1f 2021 2223 2425 2627 2829 2a2b"
        "2c2d 2e2f 3031 3233 3435 3637");
#if 0

    testPkt->setEtherPayloadStr(
        "000c 8847 000c 81ff 3322 0a2e fff1 5ed8 f932"
        "bd85 8100 000b 0800 4500 0054 40b3 4000"
        "4001 b3b6 671e 3c02 671e 3c01 0800 fbe6"
        "3edd 8281 0b21 4158 0000 0000 2d6e 0200"
        "0000 0000 1011 1213 1415 1617 1819 1a1b"
        "1c1d 1e1f 2021 2223 2425 2627 2829 2a2b"
        "2c2d 2e2f 3031 3233 3435 3637");
#endif

    //
    // TEST_PKT_ID_MPLS_L2VLAN
    //
    testPkt = &_testPacketLibrary[TEST_PKT_ID_MPLS_L2VLAN];

    testPkt->setEtherType(ETH_P_MPLS_UC);  // 0x8847
    testPkt->setDstMac("33:22:0a:2e:ff:f1");
    testPkt->setEtherPayloadStr(
        "000c 8847 000c 81ff 3322 0a2e fff1 5ed8 f932"
        "bd85 8100 000b 0800 4500 0054 40b3 4000"
        "4001 b3b6 671e 3c02 671e 3c01 0800 fbe6"
        "3edd 8281 0b21 4158 0000 0000 2d6e 0200"
        "0000 0000 1011 1213 1415 1617 1819 1a1b"
        "1c1d 1e1f 2021 2223 2425 2627 2829 2a2b"
        "2c2d 2e2f 3031 3233 3435 3637");

    //
    // TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP7
    //
    // Src IP : 103.30.60.2
    // Dst IP : 103.30.70.3
    testPkt = &_testPacketLibrary[TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP7];

    testPkt->setDstMac("32:26:0a:2e:aa:f6");  // ge-0/0/6 mac
    testPkt->setSrcMac("32:26:0A:2e:bb:f6");  // vmx_link6 MAC
    testPkt->setEtherType(ETH_P_IP);          // 0x0800
    testPkt->setEtherPayloadStr(
        "4500 0054 dacc 4000 4001 0f9c"
        "671e 3c01 671e 4603"  // 103.30.60.2 -> 103.30.70.3
        "0800 5cba 492b 3942 ee7c 5658 0000"
        "0000 0a30 0b00 0000 0000 1011 1213 1415"
        "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
        "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
        "3637");
  
    //
    // TEST_PKT_ID_BRCM_TEST_INJECT
    //
        // Mac    : 88:a2:5e:91:a2:a9
        // Src IP : 55.55.55.2
        // Dst IP : 55.55.55.1
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_INJECT];

    testPkt->setDstMac("88:a2:5e:91:a2:a9");
    testPkt->setSrcMac("32:26:0A:2E:FF:F3");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 1ae7 0000 4001 8351"
                "3737 3702 3737 3701 "
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_TRANSIT_INJECT
    //
        // Mac    : 88:a2:5e:91:a2:a8
        // Src IP : 44.44.44.2
        // Dst IP : 55.55.55.2
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_TRANSIT_INJECT];

    testPkt->setDstMac("88:a2:5e:91:a2:a8");
    testPkt->setSrcMac("32:26:0A:2E:FF:F3");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 5e3c 0000 4001 5606"
                "2c2c 2c02 3737 3702"
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_TRANSIT_PUNT
    //
        // Mac    : 88:a2:5e:91:a2:a9
        // Src IP : 44.44.44.2
        // Dst IP : 55.55.55.2
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_TRANSIT_PUNT];

    testPkt->setDstMac("88:a2:5e:91:75:ff");
    testPkt->setSrcMac("88:a2:5e:91:a2:a9");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 5e3c 0000 4001 5606"
                "2c2c 2c02 3737 3702"
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_OVERRIDE_INJECT
    //
        // Dst Mac : 88:a2:5e:91:a2:a8
        // Src Mac : 22:a1:a2:a3:a4:a5
        // Src IP  : 3.3.3.3
        // Dst IP  : 11.11.11.1
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_OVERRIDE_INJECT];

    testPkt->setDstMac("88:a2:5e:91:a2:a8");
    testPkt->setSrcMac("22:a1:a2:a3:a4:a5");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 5e3c 0000 4001 005c"
                "0303 0303 0b0b 0b01"
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_OVERRIDE_PUNT
    //
        // Dst Mac : 55:a2:5e:91:75:ff
        // Src Mac : 88:a2:5e:91:a2:a9
        // Src IP  : 3.3.3.3
        // Dst IP  : 11.11.11.1
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_OVERRIDE_PUNT];

    testPkt->setDstMac("55:a2:5e:91:75:ff");
    testPkt->setSrcMac("88:a2:5e:91:a2:a9");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 5e3c 0000 4001 005c"
                "0303 0303 0b0b 0b01"
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_VRF_INJECT
    //
        // Dst Mac : 88:a2:5e:91:a2:a8
        // Src Mac : 24:a1:a2:a3:a4:a5
        // Src IP  : 4.4.4.4
        // Dst IP  : 12.12.12.1
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_VRF_INJECT];

    testPkt->setDstMac("88:a2:5e:91:a2:a8");
    testPkt->setSrcMac("24:a1:a2:a3:a4:a5");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 5e3c 0000 4001 fc58"
                "0404 0404 0c0c 0c01"
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_VRF_PUNT
    //
        // Dst Mac : 77:a2:5e:91:75:ff
        // Src Mac : 88:a2:5e:91:a2:a9
        // Src IP  : 4.4.4.4
        // Dst IP  : 12.12.12.1
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_VRF_PUNT];

    testPkt->setDstMac("77:a2:5e:91:75:ff");
    testPkt->setSrcMac("88:a2:5e:91:a2:a9");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 5e3c 0000 4001 fc58"
                "0404 0404 0c0c 0c01"
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_FALLBACK
    //
        // Dst Mac : 88:a2:5e:91:a2:a8
        // Src Mac : 26:a1:a2:a3:a4:a5
        // Src IP  : 5.5.5.5
        // Dst IP  : 13.13.13.1
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_FALLBACK_INJECT];

    testPkt->setDstMac("88:a2:5e:91:a2:a8");
    testPkt->setSrcMac("26:a1:a2:a3:a4:a5");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 5e3c 0000 4001 f855"
                "0505 0505 0d0d 0d01"
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_FALLBACK_PUNT
    //
        // Dst Mac : 99:a2:5e:91:75:ff
        // Src Mac : 88:a2:5e:91:a2:a9
        // Src IP  : 5.5.5.5
        // Dst IP  : 13.13.13.1
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_FALLBACK_PUNT];

    testPkt->setDstMac("99:a2:5e:91:75:ff");
    testPkt->setSrcMac("88:a2:5e:91:a2:a9");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0054 5e3c 0000 4001 f855"
                "0505 0505 0d0d 0d01"
                "0800 5cba 492b 3942 ee7c 5658 0000"
                "0000 0a30 0b00 0000 0000 1011 1213 1415"
                "1617 1819 1a1b 1c1d 1e1f 2021 2223 2425"
                "2627 2829 2a2b 2c2d 2e2f 3031 3233 3435"
                "3637");

    //
    // TEST_PKT_ID_BRCM_TEST_PUNT_TBL_INJECT
    //
        // Dst Mac : 88:a2:5e:91:a2:a8
        // Src Mac : 26:a1:a2:a3:a4:a5
        // Src IP  : 139.133.217.110
        // Dst IP  : 139.133.233.2
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_PUNT_TBL_INJECT];

    testPkt->setDstMac("88:a2:5e:91:a2:a8");
    testPkt->setSrcMac("26:a1:a2:a3:a4:a5");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0028"
                "08bb 4000"
                "ff06 9998"
                "8b85 d96e"
                "8b85 e902"
                "9005 0017 7214 f130 9431 1028 5011 2238"
                "1c64 0000");

    //
    // TEST_PKT_ID_BRCM_TEST_PUNT_TBL_PUNT1
    //
        // Dst Mac : 88:a2:5e:91:a2:a8
        // Src Mac : 26:a1:a2:a3:a4:a5
        // Src IP  : 139.133.217.110
        // Dst IP  : 139.133.233.2
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_PUNT_TBL_PUNT1];

    testPkt->setDstMac("88:a2:5e:91:a2:a8");
    testPkt->setSrcMac("26:a1:a2:a3:a4:a5");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0028"
                "08bb 4000"
                "ff06 9998"
                "8b85 d96e"
                "8b85 e902"
                "9005 0017 7214 f130 9431 1028 5011 2238"
                "1c64 0000 0000 0000 0000");

    //
    // TEST_PKT_ID_BRCM_TEST_PUNT_TBL_PUNT2
    //
        // Dst Mac : 33:a2:5e:91:75:ff
        // Src Mac : 88:a2:5e:91:a2:a9
        // Src IP  : 139.133.217.110
        // Dst IP  : 139.133.233.2
    testPkt = &_testPacketLibrary[TEST_PKT_ID_BRCM_TEST_PUNT_TBL_PUNT2];

    testPkt->setDstMac("33:a2:5e:91:75:ff");
    testPkt->setSrcMac("88:a2:5e:91:a2:a9");
    testPkt->setEtherType(ETH_P_IP); // 0x0800
    testPkt->setEtherPayloadStr(
                "4500 0028"
                "08bb 4000"
                "ff06 9998"
                "8b85 d96e"
                "8b85 e902"
                "9005 0017 7214 f130 9431 1028 5011 2238"
                "1c64 0000 0000 0000 0000");
}
