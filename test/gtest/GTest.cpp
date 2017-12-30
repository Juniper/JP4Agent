//
// GTest.cpp - GTESTs
//
// GTESTs
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

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <limits.h>
#include "gtest/gtest.h"
#include "TestPacket.h"
#include "TestUtils.h"
#include "TapIf.h"
#include "Controller.h"
#include <iostream>
#include <iomanip>
#include <ctime>

//
// Test setup
//
// Junos RE CLI Sandbox configuration
// ==================================
//
// root# show forwarding-options 
// forwarding-sandbox jp4agent {
//     port p1 {
//         interface xe-0/0/0:1;
//     }
//     port p2 {
//         interface xe-0/0/0:2;
//     }
// }
//
//
//  +--------------------------------------------------------------------------+
//  | +----------------------------------------------------------------------+ |
//  | |                                          Docker container            | |
//  | |                                                  ##        .         | |
//  | |   +---------------------+                  ## ## ##       ==         | |
//  | |   |        VCP          |               ## ## ## ##      ===         | |
//  | |   |                     |           /""""""""""""""""___/ ===        | |
//  | |   +---------------------+      ~~~ {~~ ~~~~ ~~~ ~~~~ ~~ ~ /  ===- ~~~| |
//  | |                                     \______ o          __/           | |
//  | |   +---------------------+            \    \        __/               | |
//  | |   |        VFP          |              \____\______/                 | |
//  | |   |                     |                                            | |
//  | |   |                     |                                            | |
//  | |   |                     |                                            | |
//  | |   |                     |                Controller/GTEST            | |
//  | |   |                     |                    ^                       | |
//  | |   |                     |         P4 Runtime |                       | |
//  | |   |                     |                    |                       | |
//  | |   |                     |                    V                       | |
//  | |   |     afi-server <-------grpc/tcp-----> JP4Agent                   | |
//  | |   |                     |                                            | |
//  | |   |                     |                                            | |
//  | |   |                     |                                            | |
//  | |   |       riot          |                                            | |
//  | |   |                     |                                            | |
//  | |   |   p1            p2  |                                            | |
//  | |   |ge-0/0/2     ge-0/0/3|                                            | |
//  | |   +---------------------+                                            | |
//  | |       |             |                                                | |
//  | |       |             |        ,-----.                                 | |
//  | |       |             |        |     |                                 | |
//  | |       |             `--------o     o----                             | |
//  | |       |                      |_____|                                 | |
//  | |       |                     vmx_link2                                | |
//  | |       |                                                              | |
//  | |       |                                                              | |
//  | |       |                                                              | |
//  | |       |                      ,-----.                                 | |
//  | |       |                      |     |                                 | |
//  | |       `----------------------o     o----                             | |
//  | |                              |_____|                                 | |
//  | |                             vmx_link1                                | |
//  | |                                                                      | |
//  | +----------------------------------------------------------------------+ |
//  |                                                                          |
//  | mx86 server                                                              |
//  +--------------------------------------------------------------------------+
//
// root@de5cf35cd169:~# ifconfig | grep tap
// tap0      Link encap:Ethernet  HWaddr 32:26:0a:2e:cc:f0
// tap1      Link encap:Ethernet  HWaddr 32:26:0a:2e:cc:f1
// tap2      Link encap:Ethernet  HWaddr 32:26:0a:2e:cc:f2
// tap3      Link encap:Ethernet  HWaddr 32:26:0a:2e:cc:f3
// tap4      Link encap:Ethernet  HWaddr 32:26:0a:2e:cc:f4
// tap5      Link encap:Ethernet  HWaddr 32:26:0a:2e:cc:f5
// tap6      Link encap:Ethernet  HWaddr 32:26:0a:2e:cc:f6
// tap7      Link encap:Ethernet  HWaddr 32:26:0a:2e:cc:f7
// root@de5cf35cd169:~#
//
// root@de5cf35cd169:~# ifconfig | grep vmx_link
// vmx_link0 Link encap:Ethernet  HWaddr 32:26:0a:2e:bb:f0
// vmx_link1 Link encap:Ethernet  HWaddr 32:26:0a:2e:bb:f1
// vmx_link2 Link encap:Ethernet  HWaddr 32:26:0a:2e:bb:f2
// vmx_link3 Link encap:Ethernet  HWaddr 32:26:0a:2e:bb:f3
// vmx_link4 Link encap:Ethernet  HWaddr 32:26:0a:2e:bb:f4
// vmx_link5 Link encap:Ethernet  HWaddr 32:26:0a:2e:bb:f5
// vmx_link6 Link encap:Ethernet  HWaddr 32:26:0a:2e:bb:f6
// vmx_link7 Link encap:Ethernet  HWaddr 32:26:0a:2e:bb:f7
// root@de5cf35cd169:~#
//

//const std::string afiServerAddr   = "128.0.0.16:50051"; // grpc/tcp
const std::string afiServerAddr   = "172.18.0.1:65051"; // grpc/tcp
const std::string afiHospathAddr  = "128.0.0.16:9002";  // udp


//
//                  |
//       ,-----.    |
//   tapX|     |    |
//   ----o     o----o----------
//       |_____|    |pX / ge-0/0/X
//      vmx_linkX   |
//                  |
//

const std::string GE_0_0_0_MAC_STR = "32:26:0a:2e:aa:f0";
const std::string GE_0_0_1_MAC_STR = "32:26:0a:2e:aa:f1";
const std::string GE_0_0_2_MAC_STR = "32:26:0a:2e:aa:f2";
const std::string GE_0_0_3_MAC_STR = "32:26:0a:2e:aa:f3";
const std::string GE_0_0_4_MAC_STR = "32:26:0a:2e:aa:f4";
const std::string GE_0_0_5_MAC_STR = "32:26:0a:2e:aa:f5";
const std::string GE_0_0_6_MAC_STR = "32:26:0a:2e:aa:f6";
const std::string GE_0_0_7_MAC_STR = "32:26:0a:2e:aa:f7";

const std::string GE_0_0_0_VMX_IF_NAME = "ge-0.0.0-vmx1";
const std::string GE_0_0_1_VMX_IF_NAME = "ge-0.0.1-vmx1";
const std::string GE_0_0_2_VMX_IF_NAME = "ge-0.0.2-vmx1";
const std::string GE_0_0_3_VMX_IF_NAME = "ge-0.0.3-vmx1";
const std::string GE_0_0_4_VMX_IF_NAME = "ge-0.0.4-vmx1";
const std::string GE_0_0_5_VMX_IF_NAME = "ge-0.0.5-vmx1";
const std::string GE_0_0_6_VMX_IF_NAME = "ge-0.0.6-vmx1";
const std::string GE_0_0_7_VMX_IF_NAME = "ge-0.0.7-vmx1";

const std::string GE_0_0_0_IP_ADDR_STR = "103.30.00.1";
const std::string GE_0_0_1_IP_ADDR_STR = "103.30.10.1";
const std::string GE_0_0_2_IP_ADDR_STR = "103.30.20.1";
const std::string GE_0_0_3_IP_ADDR_STR = "103.30.30.1";
const std::string GE_0_0_4_IP_ADDR_STR = "103.30.40.1";
const std::string GE_0_0_5_IP_ADDR_STR = "103.30.50.1";
const std::string GE_0_0_6_IP_ADDR_STR = "103.30.60.1";
const std::string GE_0_0_7_IP_ADDR_STR = "103.30.70.1";

const std::string VMX_LINK0_NAME_STR = "vmx_link0";
const std::string VMX_LINK1_NAME_STR = "vmx_link1";
const std::string VMX_LINK2_NAME_STR = "vmx_link2";
const std::string VMX_LINK3_NAME_STR = "vmx_link3";
const std::string VMX_LINK4_NAME_STR = "vmx_link4";
const std::string VMX_LINK5_NAME_STR = "vmx_link5";
const std::string VMX_LINK6_NAME_STR = "vmx_link6";
const std::string VMX_LINK7_NAME_STR = "vmx_link7";

const std::string VMX_LINK0_MAC_STR = "32:26:0a:2e:bb:f0";
const std::string VMX_LINK1_MAC_STR = "32:26:0a:2e:bb:f1";
const std::string VMX_LINK2_MAC_STR = "32:26:0a:2e:bb:f2";
const std::string VMX_LINK3_MAC_STR = "32:26:0a:2e:bb:f3";
const std::string VMX_LINK4_MAC_STR = "32:26:0a:2e:bb:f4";
const std::string VMX_LINK5_MAC_STR = "32:26:0a:2e:bb:f5";
const std::string VMX_LINK6_MAC_STR = "32:26:0a:2e:bb:f6";
const std::string VMX_LINK7_MAC_STR = "32:26:0a:2e:bb:f7";

const std::string VMX_LINK0_IP_ADDR_STR = "103.30.00.2";
const std::string VMX_LINK1_IP_ADDR_STR = "103.30.10.2";
const std::string VMX_LINK2_IP_ADDR_STR = "103.30.20.2";
const std::string VMX_LINK3_IP_ADDR_STR = "103.30.30.2";
const std::string VMX_LINK4_IP_ADDR_STR = "103.30.40.2";
const std::string VMX_LINK5_IP_ADDR_STR = "103.30.50.2";
const std::string VMX_LINK6_IP_ADDR_STR = "103.30.60.2";
const std::string VMX_LINK7_IP_ADDR_STR = "103.30.70.2";

const std::string TAP0_NAME_STR = "tap0";
const std::string TAP1_NAME_STR = "tap1";
const std::string TAP2_NAME_STR = "tap2";
const std::string TAP3_NAME_STR = "tap3";
const std::string TAP4_NAME_STR = "tap4";
const std::string TAP5_NAME_STR = "tap5";
const std::string TAP6_NAME_STR = "tap6";
const std::string TAP7_NAME_STR = "tap7";

const std::string TAP0_MAC_STR = "32:26:0a:2e:cc:f0";
const std::string TAP1_MAC_STR = "32:26:0a:2e:cc:f1";
const std::string TAP2_MAC_STR = "32:26:0a:2e:cc:f2";
const std::string TAP3_MAC_STR = "32:26:0a:2e:cc:f3";
const std::string TAP4_MAC_STR = "32:26:0a:2e:cc:f4";
const std::string TAP5_MAC_STR = "32:26:0a:2e:cc:f5";
const std::string TAP6_MAC_STR = "32:26:0a:2e:cc:f6";
const std::string TAP7_MAC_STR = "32:26:0a:2e:cc:f7";

const std::string TAP0_IP_ADDR_STR = "103.30.00.3";
const std::string TAP1_IP_ADDR_STR = "103.30.10.3";
const std::string TAP2_IP_ADDR_STR = "103.30.20.3";
const std::string TAP3_IP_ADDR_STR = "103.30.30.3";
const std::string TAP4_IP_ADDR_STR = "103.30.40.3";
const std::string TAP5_IP_ADDR_STR = "103.30.50.3";
const std::string TAP6_IP_ADDR_STR = "103.30.60.3";
const std::string TAP7_IP_ADDR_STR = "103.30.70.3";

//
// IPv4 Router
//                                                         Expected Packet
//                                                               /\
//               ,----------------------------------------.      ||
//               |                                        |      ||
//               |              .                         |      ||
//    ,-----.    |             / \                        |    ,-----.
//    |     |    |            /   \                       |    |     |      
//    |     o----o---------->/  R1-\--->[Eth Encap]------>o----o     |
//    |_____|    |p2        /_______\                   p3|    |_____|
//   vmx_link2   |ge-0/0/2                        ge-0/0/3|    vmx_link3
//      /\       |                                        |
//      ||       |                                        |
//      ||       |            Sandbox: jp4agent           |
//      ||       `----------------------------------------'
// Input Packet
//

TEST(AFI, ipv4Router)
{
    int ret = 0;
    std::string tcName = "AFI";
    std::string tName  = "ipv4Router";

    // Controller
    //TestCaseSimpleRouter();
    //sleep(60);

    std::vector<std::string> capture_ifs;
    capture_ifs.push_back(GE_0_0_2_VMX_IF_NAME);
    capture_ifs.push_back(GE_0_0_3_VMX_IF_NAME);
    tStartTsharkCapture(tcName, tName, capture_ifs);

    std::string tapName = TAP3_NAME_STR;

    test_complete.store(false);
    boost::thread tapThread(boost::bind(&tapIfReadPkts, boost::ref(tapName)));

    std::cout << "Sleeping for 2 seconds ... \n";
    sleep(2);

    const int num_pkts_to_send = 1;
    for (int i = 0; i < num_pkts_to_send; i++) {
        ret = SendRawEth(VMX_LINK2_NAME_STR,
                  TestPacketLibrary::VMXZT_TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3);
        EXPECT_EQ(0, ret);
        sleep(2);
    }

    test_complete.store(true);

    if (tapThread.timed_join( boost::posix_time::seconds(5))) {
        std::cout<<"\nDone!\n";
    } else {
        std::cerr<<"\nTimed out!\n";
    }

    EXPECT_EQ(0, ret);
    stopTsharkCapture();
    tVerifyPackets(tcName, tName, capture_ifs);
}

// Test2: Inject L2 pkt with port ID of vmx_link2 at controller and verify
// receipt at vmx_link2.
TEST(AFI, injectL2Pkt)
{
    int ret = 0;
    std::string tcName = "AFI";
    std::string tName = "injectL2Pkt";

    std::vector<std::string> capture_ifs{GE_0_0_2_VMX_IF_NAME};
    std::string tapName = TAP2_NAME_STR;
    constexpr uint16_t egress_port = 0;

    tStartTsharkCapture(tcName, tName, capture_ifs);

    test_complete.store(false);
    boost::thread tap_thrd{[&tapName]() { tapIfReadPkts(tapName); }};

    std::cout << "Sleeping for 2 seconds ... \n";
    sleep(2);

    TestPacket *testPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_IPV4_ECHO_REQ_TO_TAP2);
    char pktbuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t pktlen = testPkt->getEtherPacket(pktbuf, ETHER_PAYLOAD_BUF_SIZE);

    std::string l2_pkt{pktbuf, pktlen};

    // Print injected pkt
    std::cout << "Injected pkt:" << std::hex << std::uppercase;
    for (size_t i = 0; i < pktlen; i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)l2_pkt[i];
    }
    std::cout << "\n" << std::dec << std::nouppercase;

    // Call controller to inject the L2 pkt on the JP4Agent connection.
    ret = ControllerInjectL2Pkt(l2_pkt, egress_port);
    EXPECT_EQ(0, ret);
    sleep(2);

    test_complete.store(true);

    bool complete = tap_thrd.try_join_for(boost::chrono::seconds(5));
    EXPECT_EQ(true, complete) << "Timed out waiting for pkt";

    stopTsharkCapture();
    tVerifyPackets(tcName, tName, capture_ifs);
}

// Test3: Punt path: Inject L2 pkt in vmx_link2 and verify receipt at controller.
TEST(AFI, puntL2Pkt)
{
    int ret = 0;
    uint16_t ingress_port = 0;
    constexpr uint16_t expected_ingress_port = 0;
    std::string recvd_pkt;

    // Start Controller and listen.
    boost::thread cont_thrd{[&recvd_pkt, &ingress_port]() {
        ControllerPuntPkt(recvd_pkt, ingress_port);
    }};

    // Send RawEth pkt on vmx_link2
    ret = SendRawEth(
        VMX_LINK2_NAME_STR,
        TestPacketLibrary::VMXZT_TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3);
    EXPECT_EQ(0, ret) << "Failed to send pkt to VMX_LINK2";

    // Wait for 5 seconds
    bool complete = cont_thrd.try_join_for(boost::chrono::seconds(5));
    ASSERT_EQ(true, complete) << "Timed out waiting for pkt";
    EXPECT_EQ(false, recvd_pkt.empty()) << "Failed to read packet";
    EXPECT_EQ(expected_ingress_port, ingress_port)
        << "Packet didn't arrive on the expected ingress port";

    // Compare and verify
    TestPacket *testPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::VMXZT_TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3);
    char pktbuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t pktlen = testPkt->getEtherPacket(pktbuf, ETHER_PAYLOAD_BUF_SIZE);

    EXPECT_EQ(0, memcmp(pktbuf, recvd_pkt.data(), pktlen))
        << "Sent and received pkts differ";
}

//
// gtest main
//
int main(int argc, char **argv)
{

    getTimeStr(gTestTimeStr);
    std::cout<<gTestTimeStr << std::endl;

    stopTsharkCapture();

    gtestOutputDirName = "GTEST_" + gTestTimeStr;
    std::string mk_gtestOutputDirName_cmd = "mkdir -p " + gtestOutputDirName;

    system(mk_gtestOutputDirName_cmd.c_str());

    ::testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "*";
    //::testing::GTEST_FLAG(filter) = "*SandboxBlueOpen*:*sb2IPv4Routing*";
    //::testing::GTEST_FLAG(filter) = "*ipv4Router*";
    ::testing::GTEST_FLAG(filter) = "*injectL2Pkt*:*puntL2Pkt*";
    return RUN_ALL_TESTS();
}
