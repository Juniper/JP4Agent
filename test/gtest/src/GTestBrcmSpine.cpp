//
// GTestBrcmSpine.cpp - GTESTs
//
// GTESTs
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

#include <limits.h>
#include "gtest/gtest.h"
#include "TestPacket.h"
#include "TestUtils.h"
#include "Controller.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <future>
#include <fstream>

using namespace std;

using namespace std::chrono_literals;

//
// Test setup
//
//        +---------------------+
//        |        QFX          |                Controller/GTEST
//        |                     |                    ^
//        |                     |         P4 Runtime |
//        |                     |                    |
//        |                     |                    V
//        |   BCM RPC server <------BCM RPC-----> JP4Agent
//        |                     |
//        |                     |
//        |                     |
//        |                     |
//        |   p1            p2  |
//        +--|-|-----------|-|--+
//           |_|           |_|
//
//

// Test fixture class for common setup.
// We setup the forwarding pipeline config here.
class P4BRCMSPINE : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        std::string pFile("/root/JP4Agent/test/controller/testdata/spine.json");
        std::string rFile("/root/JP4Agent/test/controller/testdata/spine.p4rt");
        ControllerSetP4Input(pFile, rFile);

        ControllerSetConfig();
        sleep_thread_log(5s);
    }
};

// Test1: VRF Classifier Table
TEST_F(P4BRCMSPINE, VrfClassifier)
{
    unsigned char m1[6] = {0x22, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5};
    std::string mac1((char *) &m1[0], 6);
    ControllerAddVrfEntry(0x800, mac1, 0x0b0b0b01, 5);

    unsigned char m2[6] = {0x24, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5};
    std::string mac2((char *) &m2[0], 6);
    ControllerAddVrfEntry(0x800, mac2, 0x0c0c0c01, 5);

    unsigned char m3[6] = {0x26, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5};
    std::string mac3((char *) &m3[0], 6);
    ControllerAddVrfEntry(0x800, mac3, 0x0d0d0d01, 5);
    ControllerAddVrfEntry(0x800, mac3, 0x8b85e902, 5);

    EXPECT_EQ(1, 1);
}

// Test2: Class ID Table
TEST_F(P4BRCMSPINE, ClassId)
{
    ControllerAddClassIdEntry(100, 5, 0x800, 1, 7, 0x37373702, 30, 40, 10);
    ControllerAddClassIdEntry(0, 0, 0x800, 1, 64, 0x0b0b0b01, 3333, 1111, 10);
    ControllerAddClassIdEntry(0, 0, 0x800, 1, 64, 0x0c0c0c01, 4444, 1212, 10);
    ControllerAddClassIdEntry(0, 0, 0x800, 1, 64, 0x0d0d0d01, 5555, 1313, 10);
    ControllerAddClassIdEntry(0, 0, 0x800, 6, 64, 0x8b85e902, 36839, 23, 10);

    EXPECT_EQ(1, 1);
}

// Test3: MyMAC Table
TEST_F(P4BRCMSPINE, MyMac)
{
    unsigned char m[6] = {0x88, 0xa2, 0x5e, 0x91, 0x75, 0xff};
    std::string mac((char *) &m[0], 6);
    ControllerAddMyMacEntry(mac);
    EXPECT_EQ(1, 1);
}

// Test4: Override, VRF, Fallback Route Tables
TEST_F(P4BRCMSPINE, RTT)
{
    // Default route for the default IPv4 table.
    ControllerAddRtEncapEntry(0, 0, 0, 0x44a25e9175ff, 0x22a25e9175ff,
                              13, 10, "vrf");

    // Default route for the VRF 5 table.
    ControllerAddRtEncapEntry(0, 0, 5, 0x66a25e9175ff, 0x33a25e9175ff,
                              13, 20, "vrf");

    // Override route.
    ControllerAddRtEncapEntry(0x0b0b0b01, 24, 0, 0x88a25e91a2a9, 0x55a25e9175ff,
                              13, 10, "override");

    // VRF 5 routes.
    ControllerAddRtEncapEntry(0x0b0b0b01, 24, 5, 0x88a25e91a2a9, 0x77a25e9175ff,
                              13, 10, "vrf");

    ControllerAddRtEncapEntry(0x0c0c0c01, 24, 5, 0x88a25e91a2a9, 0x77a25e9175ff,
                              13, 10, "vrf");

    // Fallback route.
    ControllerAddRtEncapEntry(0x0d0d0d01, 24, 0, 0x88a25e91a2a9, 0x99a25e9175ff,
                              13, 10, "fallback");

    EXPECT_EQ(1, 1);
}

// Test5: Punt Table
TEST_F(P4BRCMSPINE, Punt)
{
    ControllerAddPuntEntry(9, 13, 0x800, 0x88a25e91a2a8,
                           0, 255, 0x8b85d96e, 0x8b85e902,
                           6, 10, 5, 29);

    EXPECT_EQ(1, 1);
}

// Traffic Tests
// Two test L3 interfaces in the system:
//   Ingress: Port: 9,  MAC: 0x88a25e91a2a8, IP: 44.44.44.1/24
//   Egress:  Port: 13, MAC: 0x88a25e91a2a9, IP: 55.55.55.1/24

// Three NHIDs:
//   (1) Egress Port: 13, NH MAC: 0x55a25e9175ff
//   (2) Egress Port: 13, NH MAC: 0x77a25e9175ff
//   (3) Egress Port: 13, NH MAC: 0x99a25e9175ff

// Four routes:
//   11.11.11.0/24, NH(1), VRF 0, Class ID 10, Override Table
//   12.12.12.0/24, NH(1), VRF 5, Class ID 10, VRF Table
//   13.13.13.0/24, NH(1), VRF 0, Class ID 10, Fallback Table

// Four test IPv4 packets:
//   smac: 0x22a1a2a3a4a5, dmac: 0x88a25e91a2a8, sip: 3.3.3.3, dip: 11.11.11.1
//   smac: 0x24a1a2a3a4a5, dmac: 0x88a25e91a2a8, sip: 4.4.4.4, dip: 12.12.12.1
//   smac: 0x26a1a2a3a4a5, dmac: 0x88a25e91a2a8, sip: 5.5.5.5, dip: 13.13.13.1
//   smac: 0x26a1a2a3a4a5, dmac: 0x88a25e91a2a8, sip: 139.133.217.110, dip: 139.133.233.2

// Test6: VRF table traffic Test
TEST_F(P4BRCMSPINE, VrfTraffic)
{
    uint16_t iport = 9;
    uint16_t eport = 13;

    sleep_thread_log(2s);

    TestPacket *vrfPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_VRF_INJECT);
    char vrfBuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t vrfLen = vrfPkt->getEtherPacket(vrfBuf, ETHER_PAYLOAD_BUF_SIZE);
    std::string vrfL2Pkt{vrfBuf, vrfLen};

    // Print injected pkt
    std::cout << "Injected pkt on port: " << iport << std::hex << std::uppercase;
    for (size_t i = 0; i < vrfLen; i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)vrfL2Pkt[i];
    }
    std::cout << std::endl << std::dec << std::nouppercase;

    std::string retPkt;
    ControllerInjectPuntL2Pkt(vrfL2Pkt, retPkt, eport, iport, 30s);
    ASSERT_FALSE(retPkt.empty()) << "Failed to read packet";

    TestPacket *puntPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_VRF_PUNT);
    vrfLen = puntPkt->getEtherPacket(vrfBuf, ETHER_PAYLOAD_BUF_SIZE);

    // Verify packets. Ignore checksum difference.
    EXPECT_EQ(1, ((memcmp(vrfBuf, retPkt.data(), 22) == 0) &&
                  (memcmp(vrfBuf + 25, retPkt.data() + 25, vrfLen - 26) == 0)))
        << "Sent and received pkts differ";
}

// Test7: Override table traffic Test
TEST_F(P4BRCMSPINE, OverrideTraffic)
{
    uint16_t iport = 9;
    uint16_t eport = 13;

    // Override packet
    sleep_thread_log(2s);

    TestPacket *oridePkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_OVERRIDE_INJECT);
    char orideBuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t orideLen = oridePkt->getEtherPacket(orideBuf, ETHER_PAYLOAD_BUF_SIZE);
    std::string orideL2Pkt{orideBuf, orideLen};

    // Print injected pkt
    std::cout << "Injected pkt on port: " << iport << std::hex << std::uppercase;
    for (size_t i = 0; i < orideLen; i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)orideL2Pkt[i];
    }
    std::cout << std::endl << std::dec << std::nouppercase;

    std::string retPkt;
    ControllerInjectPuntL2Pkt(orideL2Pkt, retPkt, eport, iport, 30s);
    ASSERT_FALSE(retPkt.empty()) << "Failed to read packet";

    TestPacket *puntPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_OVERRIDE_PUNT);
    orideLen = puntPkt->getEtherPacket(orideBuf, ETHER_PAYLOAD_BUF_SIZE);

    // Verify packets. Ignore checksum difference.
    EXPECT_EQ(1, ((memcmp(orideBuf, retPkt.data(), 22) == 0) &&
                  (memcmp(orideBuf + 25, retPkt.data() + 25, orideLen - 26) == 0)))
        << "Sent and received pkts differ";
}

// Test8: Fallback table traffic Test
TEST_F(P4BRCMSPINE, FallbackTraffic)
{
    uint16_t iport = 9;
    uint16_t eport = 13;

    // Fallback packet
    sleep_thread_log(2s);

    TestPacket *fbackPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_FALLBACK_INJECT);
    char fbackBuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t fbackLen = fbackPkt->getEtherPacket(fbackBuf, ETHER_PAYLOAD_BUF_SIZE);
    std::string fbackL2Pkt{fbackBuf, fbackLen};

    // Print injected pkt
    std::cout << "Injected pkt on port: " << iport << std::hex << std::uppercase;
    for (size_t i = 0; i < fbackLen; i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)fbackL2Pkt[i];
    }
    std::cout << std::endl << std::dec << std::nouppercase;

    std::string retPkt;
    ControllerInjectPuntL2Pkt(fbackL2Pkt, retPkt, eport, iport, 30s);
    ASSERT_FALSE(retPkt.empty()) << "Failed to read packet";

    TestPacket *puntPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_FALLBACK_PUNT);
    fbackLen = puntPkt->getEtherPacket(fbackBuf, ETHER_PAYLOAD_BUF_SIZE);

    // Verify packets. Ignore checksum difference.
    EXPECT_EQ(1, ((memcmp(fbackBuf, retPkt.data(), 22) == 0) &&
                  (memcmp(fbackBuf + 25, retPkt.data() + 25, fbackLen - 26) == 0)))
        << "Sent and received pkts differ";
}

// Test9: Punt table traffic Test
TEST_F(P4BRCMSPINE, PuntTableTraffic)
{
    uint16_t iport = 9;
    uint16_t eport = 13;

    // Punt table packet
    sleep_thread_log(2s);

    TestPacket *ptblPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_PUNT_TBL_INJECT);
    char ptblBuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t ptblLen = ptblPkt->getEtherPacket(ptblBuf, ETHER_PAYLOAD_BUF_SIZE);
    std::string ptblL2Pkt{ptblBuf, ptblLen};

    // Print injected pkt
    std::cout << "Injected pkt on port: " << iport << std::hex << std::uppercase;
    for (size_t i = 0; i < ptblLen; i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)ptblL2Pkt[i];
    }
    std::cout << std::endl << std::dec << std::nouppercase;

    std::string retPkt1, retPkt2;
    ControllerInjectPuntL2Pkts(ptblL2Pkt, retPkt1, retPkt2, eport, iport, 10s);
    ASSERT_FALSE(retPkt1.empty()) << "Failed to read packet 1";
    ASSERT_FALSE(retPkt2.empty()) << "Failed to read packet 2";

    TestPacket *puntPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_PUNT_TBL_PUNT1);
    ptblLen = puntPkt->getEtherPacket(ptblBuf, ETHER_PAYLOAD_BUF_SIZE);

    // Verify packets. Ignore checksum difference.
    ASSERT_TRUE((memcmp(ptblBuf, retPkt1.data(), 22) == 0) &&
                (memcmp(ptblBuf + 25, retPkt1.data() + 25, ptblLen - 26) == 0))
                << "Packet 1 differs";

    puntPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_PUNT_TBL_PUNT2);
    ptblLen = puntPkt->getEtherPacket(ptblBuf, ETHER_PAYLOAD_BUF_SIZE);

    EXPECT_EQ(1, ((memcmp(ptblBuf, retPkt2.data(), 22) == 0) &&
                  (memcmp(ptblBuf + 25, retPkt2.data() + 25, ptblLen - 26) == 0)))
        << "Packet 2 differs";
}
