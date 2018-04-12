//
// GTestBrcm.cpp - GTESTs
//
// GTESTs
//
// Created by Sudheendra Gopinath, March 2018
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
class P4BRCM : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        ControllerSetConfig();
        sleep_thread_log(15s);
    }
};

// Test1: Inject and Punt Test.
TEST_F(P4BRCM, BrcmInjectPuntL2Pkt)
{
    uint16_t port = 13;

    TestPacket *testPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_INJECT);
    char pktbuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t pktlen = testPkt->getEtherPacket(pktbuf, ETHER_PAYLOAD_BUF_SIZE);
    std::string inject_l2_pkt{pktbuf, pktlen};

    // Print injected pkt
    std::cout << "Injected pkt:" << std::hex << std::uppercase;
    for (size_t i = 0; i < pktlen; i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)inject_l2_pkt[i];
    }
    std::cout << std::endl << std::dec << std::nouppercase;

    std::string recvd_pkt;
    ControllerInjectPuntL2Pkt(inject_l2_pkt, recvd_pkt, port, port, 15s);
    ASSERT_FALSE(recvd_pkt.empty()) << "Failed to read packet";

    // Verify packets
    EXPECT_EQ(0, memcmp(pktbuf, recvd_pkt.data(), pktlen))
        << "Sent and received pkts differ";
}

// Test2: Transit Packet Test.
TEST_F(P4BRCM, BrcmIpv4Router)
{
    uint16_t iport = 9;
    uint16_t eport = 13;

    // Launch controller to add route entry.
    ControllerAddRouteEntry(0x37373702, 24, 0x0a000001, 0x88a25e9175ff, 13);
    sleep_thread_log(5s);

    TestPacket *injectPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_TRANSIT_INJECT);
    char pktbuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t pktlen = injectPkt->getEtherPacket(pktbuf, ETHER_PAYLOAD_BUF_SIZE);
    std::string inject_l2_pkt{pktbuf, pktlen};

    // Print injected pkt
    std::cout << "Injected pkt:" << std::hex << std::uppercase;
    for (size_t i = 0; i < pktlen; i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)inject_l2_pkt[i];
    }
    std::cout << std::endl << std::dec << std::nouppercase;

    std::string recvd_pkt;
    ControllerInjectPuntL2Pkt(inject_l2_pkt, recvd_pkt, eport, iport, 30s);
    ASSERT_FALSE(recvd_pkt.empty()) << "Failed to read packet";

    TestPacket *puntPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_BRCM_TEST_TRANSIT_PUNT);
    pktlen = puntPkt->getEtherPacket(pktbuf, ETHER_PAYLOAD_BUF_SIZE);

    // Verify packets. Ignore checksum difference.
    EXPECT_EQ(0, (memcmp(pktbuf, recvd_pkt.data(), 22) &&
                  memcmp(pktbuf + 25, recvd_pkt.data() + 25, pktlen - 26)))
        << "Sent and received pkts differ";
}
