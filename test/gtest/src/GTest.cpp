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

#include <limits.h>
#include "gtest/gtest.h"
#include "TestPacket.h"
#include "TestUtils.h"
#include "TapIf.h"
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
//  | |                                         Docker container             | |
//  | |                                                 ##        .          | |
//  | |   +---------------------+                 ## ## ##       ==          | |
//  | |   |        VCP          |              ## ## ## ##      ===          | |
//  | |   |                     |          /""""""""""""""""___/ ===         | |
//  | |   +---------------------+     ~~~ {~~ ~~~~ ~~~ ~~~~ ~~ ~ /  ===- ~~~ | |
//  | |                                    \______ o          __/            | |
//  | |   +---------------------+           \    \        __/                | |
//  | |   |        VFP          |             \____\______/                  | |
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
//  | |       |             `--------o     |                                 | |
//  | |       |                      |_____|                                 | |
//  | |       |                     vmx_link2                                | |
//  | |       |                                                              | |
//  | |       |                                                              | |
//  | |       |                                                              | |
//  | |       |                      ,-----.                                 | |
//  | |       |                      |     |                                 | |
//  | |       `----------------------o     |                                 | |
//  | |                              |_____|                                 | |
//  | |                             vmx_link1                                | |
//  | |                                                                      | |
//  | +----------------------------------------------------------------------+ |
//  |                                                                          |
//  | mx86 server                                                              |
//  +--------------------------------------------------------------------------+
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
//              |
//   ,-----.    |
//   |     |    |
//   |     o----o----------
//   |_____|    |pX / ge-0/0/X
//  vmx_linkX   |
//              |
//

const std::string GE_0_0_0_MAC_STR = "fe:26:0a:2e:aa:f0";
const std::string GE_0_0_1_MAC_STR = "fe:26:0a:2e:aa:f1";
const std::string GE_0_0_2_MAC_STR = "fe:26:0a:2e:aa:f2";
const std::string GE_0_0_3_MAC_STR = "fe:26:0a:2e:aa:f3";
const std::string GE_0_0_4_MAC_STR = "fe:26:0a:2e:aa:f4";
const std::string GE_0_0_5_MAC_STR = "fe:26:0a:2e:aa:f5";
const std::string GE_0_0_6_MAC_STR = "fe:26:0a:2e:aa:f6";
const std::string GE_0_0_7_MAC_STR = "fe:26:0a:2e:aa:f7";

const std::string GE_0_0_0_VMX_IF_NAME = "ge-0.0.0-vmx1";
const std::string GE_0_0_1_VMX_IF_NAME = "ge-0.0.1-vmx1";
const std::string GE_0_0_2_VMX_IF_NAME = "ge-0.0.2-vmx1";
const std::string GE_0_0_3_VMX_IF_NAME = "ge-0.0.3-vmx1";
const std::string GE_0_0_4_VMX_IF_NAME = "ge-0.0.4-vmx1";
const std::string GE_0_0_5_VMX_IF_NAME = "ge-0.0.5-vmx1";
const std::string GE_0_0_6_VMX_IF_NAME = "ge-0.0.6-vmx1";
const std::string GE_0_0_7_VMX_IF_NAME = "ge-0.0.7-vmx1";

const std::string GE_0_0_0_IP_ADDR_STR = "103.30.100.1";
const std::string GE_0_0_1_IP_ADDR_STR = "103.30.110.1";
const std::string GE_0_0_2_IP_ADDR_STR = "103.30.120.1";
const std::string GE_0_0_3_IP_ADDR_STR = "103.30.130.1";
const std::string GE_0_0_4_IP_ADDR_STR = "103.30.140.1";
const std::string GE_0_0_5_IP_ADDR_STR = "103.30.150.1";
const std::string GE_0_0_6_IP_ADDR_STR = "103.30.160.1";
const std::string GE_0_0_7_IP_ADDR_STR = "103.30.170.1";

const std::string VMX_LINK0_NAME_STR = "vmx_link10";
const std::string VMX_LINK1_NAME_STR = "vmx_link11";
const std::string VMX_LINK2_NAME_STR = "vmx_link12";
const std::string VMX_LINK3_NAME_STR = "vmx_link13";
const std::string VMX_LINK4_NAME_STR = "vmx_link14";
const std::string VMX_LINK5_NAME_STR = "vmx_link15";
const std::string VMX_LINK6_NAME_STR = "vmx_link16";
const std::string VMX_LINK7_NAME_STR = "vmx_link17";

const std::string VMX_LINK0_MAC_STR = "32:26:0a:2e:bb:f0";
const std::string VMX_LINK1_MAC_STR = "32:26:0a:2e:bb:f1";
const std::string VMX_LINK2_MAC_STR = "32:26:0a:2e:bb:f2";
const std::string VMX_LINK3_MAC_STR = "32:26:0a:2e:bb:f3";
const std::string VMX_LINK4_MAC_STR = "32:26:0a:2e:bb:f4";
const std::string VMX_LINK5_MAC_STR = "32:26:0a:2e:bb:f5";
const std::string VMX_LINK6_MAC_STR = "32:26:0a:2e:bb:f6";
const std::string VMX_LINK7_MAC_STR = "32:26:0a:2e:bb:f7";

const std::string VMX_LINK0_IP_ADDR_STR = "103.30.100.2";
const std::string VMX_LINK1_IP_ADDR_STR = "103.30.110.2";
const std::string VMX_LINK2_IP_ADDR_STR = "103.30.120.2";
const std::string VMX_LINK3_IP_ADDR_STR = "103.30.130.2";
const std::string VMX_LINK4_IP_ADDR_STR = "103.30.140.2";
const std::string VMX_LINK5_IP_ADDR_STR = "103.30.150.2";
const std::string VMX_LINK6_IP_ADDR_STR = "103.30.160.2";
const std::string VMX_LINK7_IP_ADDR_STR = "103.30.170.2";

/*
 * IPv4 Router
 *                                                         Expected Packet
 *                                                               /\
 *               ,----------------------------------------.      ||
 *               |                                        |      ||
 *               |              .                         |      ||
 *    ,-----.    |             / \                        |    ,-----.
 *    |     |    |            /   \                       |    |     |
 *    |     o----o---------->/  R1-\--->[Eth Encap]------>o----o     |
 *    |_____|    |p2        /_______\                   p3|    |_____|
 *   vmx_link2   |ge-0/0/2                        ge-0/0/3|    vmx_link3
 *      /\       |                                        |
 *      ||       |                                        |
 *      ||       |            Sandbox: jp4agent           |
 *      ||       `----------------------------------------'
 * Input Packet
 */

// Test fixture class for common setup. We setup the forwarding pipeline config
// here.
class P4 : public ::testing::Test
{
protected:
    static void SetUpTestCase()
    {
        ControllerSetConfig();
        sleep_thread_log(15s);
    }
};

// Test1: Inject L2 pkt with port ID of vmx_link2 at controller and verify
// receipt at vmx_link2.
TEST_F(P4, injectL2Pkt)
{
    constexpr uint16_t egress_port = 0;
    constexpr int num_pkts = 1;
    constexpr int pcap_timeout_sec = 10;

    // Start listening for pkts.
    const std::vector<std::string> capture_ifs{GE_0_0_2_VMX_IF_NAME};
    std::vector<pid_t> pcap_pids;
    ASSERT_NO_FATAL_FAILURE(
        start_pktcap(capture_ifs, num_pkts, pcap_timeout_sec, pcap_pids));
    sleep_thread_log(3s);

    std::cout << "Injecting L2 packet on egress port " << egress_port << "...";
    TestPacket *testPkt = testPacketLibrary.getTestPacket(
        TestPacketLibrary::TEST_PKT_ID_IPV4_ECHO_REQ_TO_TAP2);
    char pktbuf[ETHER_PAYLOAD_BUF_SIZE];
    size_t pktlen = testPkt->getEtherPacket(pktbuf, ETHER_PAYLOAD_BUF_SIZE);
    std::string l2_pkt{pktbuf, pktlen};

    // Call controller to inject the L2 pkt on the JP4Agent connection.
    EXPECT_TRUE(ControllerInjectL2Pkt(l2_pkt, egress_port))
        << "Failed to inject L2 pkt.";
    std::cout << "Done." << std::endl;

    // Print injected pkt
    std::cout << "Injected pkt:" << std::hex << std::uppercase;
    for (size_t i = 0; i < pktlen; i++) {
        if (i % 16 == 0) std::cout << "\n";
        std::cout << " " << std::setfill('0') << std::setw(2)
                  << +(uint8_t)l2_pkt[i];
    }
    std::cout << std::endl << std::dec << std::nouppercase;

    // Wait for pcap child processes
    ASSERT_TRUE(stop_pktcap(capture_ifs, pcap_pids))
        << "Packet capture failed or timed out";

    // Verify packets
    tVerifyPackets(capture_ifs);
}

// Test2: Punt path: Inject L2 pkt in vmx_link2 and verify receipt at controller.
TEST_F(P4, puntL2Pkt)
{
    constexpr uint16_t expected_ingress_port = 0;

    // Start Controller and listen.
    uint16_t ingress_port = 0;
    std::string recvd_pkt;
    auto fut = std::async(std::launch::async, ControllerPuntPkt,
                          std::ref(recvd_pkt), std::ref(ingress_port), 15s);

    constexpr int num_pkts = 1;
    constexpr int pcap_timeout_sec = 10;
    const std::vector<std::string> capture_ifs{GE_0_0_2_VMX_IF_NAME};
    std::vector<pid_t> pcap_pids;
    ASSERT_NO_FATAL_FAILURE(
        start_pktcap(capture_ifs, num_pkts, pcap_timeout_sec, pcap_pids));
    sleep_thread_log(3s);

    // Send RawEth pkt on vmx_link2
    int ret = SendRawEth(
        VMX_LINK2_NAME_STR,
        TestPacketLibrary::VMXZT_TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3);
    EXPECT_EQ(0, ret) << "Failed to send pkt to VMX_LINK2";

    // Wait for pcap child processes
    ASSERT_TRUE(stop_pktcap(capture_ifs, pcap_pids))
        << "Packet capture failed or timed out";

    // Verify packets
    tVerifyPackets(capture_ifs);

    // Wait for controller thread
    ASSERT_TRUE(fut.get()) << "Timed out waiting for packet";
    ASSERT_FALSE(recvd_pkt.empty()) << "Failed to read packet";
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

// Test3: ICMP echo request/reply
TEST_F(P4, hostPing)
{
    // Start Controller to handle ping.
    auto fut = std::async(std::launch::async, ControllerICMPEcho, 15s);

    constexpr int num_pkts = 2;
    constexpr int pcap_timeout_sec = 10;
    const std::vector<std::string> capture_ifs{GE_0_0_2_VMX_IF_NAME};
    std::vector<pid_t> pcap_pids;
    ASSERT_NO_FATAL_FAILURE(
        start_pktcap(capture_ifs, num_pkts, pcap_timeout_sec, pcap_pids));
    sleep_thread_log(3s);

    // Send ICMP Echo request.
    int ret = SendRawEth(
        VMX_LINK2_NAME_STR,
        TestPacketLibrary::VMXZT_TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3);
    EXPECT_EQ(0, ret) << "Failed to send pkt to VMX_LINK2";

    // Wait for pcap child processes
    ASSERT_TRUE(stop_pktcap(capture_ifs, pcap_pids))
        << "Packet capture failed or timed out";

    // Verify packets
    tVerifyPackets(capture_ifs);

    // Wait for controller thread
    ASSERT_TRUE(fut.get()) << "Timed out waiting for pkt";
}

// Test4: Send ARP request to Controller and verify ARP reply.
TEST_F(P4, sendArpReq)
{
    // Start Controller to handle arp req.
    std::string recvd_pkt;
    auto fut = std::async(std::launch::async, ControllerHandleArpReq, 15s);

    constexpr int num_pkts = 2; // ARP request and reply.
    constexpr int pcap_timeout_sec = 10;
    const std::vector<std::string> capture_ifs{GE_0_0_2_VMX_IF_NAME};
    std::vector<pid_t> pcap_pids;
    ASSERT_NO_FATAL_FAILURE(
        start_pktcap(capture_ifs, num_pkts, pcap_timeout_sec, pcap_pids));
    sleep_thread_log(3s);

    // Construct and send ARP request
    EXPECT_NO_FATAL_FAILURE(
        send_arp_req(VMX_LINK2_NAME_STR, GE_0_0_2_IP_ADDR_STR.c_str()));

    // Wait for pcap child processes
    ASSERT_TRUE(stop_pktcap(capture_ifs, pcap_pids))
        << "Packet capture failed or timed out";

    // Verify packets
    tVerifyPackets(capture_ifs);

    // Wait for controller thread
    ASSERT_TRUE(fut.get()) << "Timed out waiting for ARP request packet.";
}

// Test5: Transit packet.
TEST_F(P4, ipv4Router)
{
    // Launch controller to add route entry.
    ControllerAddRouteEntry(0x0a000001, 16, 0x0a000001, 0x88a25e9175ff, 1);
    sleep_thread_log(5s);

    const int num_pkts_to_send = 1;
    const int pcap_timeout_sec = 10;

    // Start packet capture on ingress and egress interfaces.
    const std::vector<std::string> capture_ifs{GE_0_0_2_VMX_IF_NAME,
                                               GE_0_0_3_VMX_IF_NAME};
    std::vector<pid_t> pcap_pids;
    ASSERT_NO_FATAL_FAILURE(start_pktcap(capture_ifs, num_pkts_to_send,
                                         pcap_timeout_sec, pcap_pids));
    sleep_thread_log(3s);

    // Send L2 packet.
    for (int i = 0; i < num_pkts_to_send; i++) {
        int ret = SendRawEth(
            VMX_LINK2_NAME_STR,
            TestPacketLibrary::VMXZT_TEST_PKT_ID_IPV4_ROUTER_ICMP_ECHO_TO_TAP3);
        EXPECT_EQ(0, ret);
        std::this_thread::sleep_for(1s);
    }

    // Wait for pcap child processes
    ASSERT_TRUE(stop_pktcap(capture_ifs, pcap_pids))
        << "Packet capture failed or timed out";

    // Verify packets
    tVerifyPackets(capture_ifs);
}

// Test6: Null Target Test.
TEST_F(P4, nullTest)
{
    // Launch controller to add route entry.
    ControllerAddRouteEntry(0x0a000001, 16, 0x0a000001, 0x88a25e9175ff, 1);
    sleep_thread_log(15s);
    ifstream gtestFile;
    std::string line, log;
    const std::string expectedString = "key_field: packet.ip4.daddr\ntree.ByteSize(): 69\nentry_name: entry1\nparent_name: ipv4_lpm\ntarget_afi_object: etherencap1\n";
    gtestFile.open("/root/JP4Agent/src/targets/null/NullTest.txt");
    while ( getline (gtestFile,line) )
        {
            log += line + "\n";
	}
    gtestFile.close();
    EXPECT_TRUE(log.compare(expectedString) == 0);
}

//
// gtest main
//
int main(int argc, char **argv)
{
    // Create result dir.
    gtestOutputDirName = "GTEST_RESULT_" + getTimeStr();
    boost::filesystem::create_directories(gtestOutputDirName);

    ::testing::InitGoogleTest(&argc, argv);
    if (argc == 1) {
        ::testing::GTEST_FLAG(filter) = "P4.*-P4.nullTest";
    }
    else {
        if (strcmp(argv[1], "brcm")  == 0) {
            ::testing::GTEST_FLAG(filter) = "P4BRCM.*";
        } else {
            ::testing::GTEST_FLAG(filter) = "*nullTest*";
        }
    }
   //::testing::GTEST_FLAG(filter) = "*ipv4Router*";
   //::testing::GTEST_FLAG(filter) = "*injectL2Pkt*:*puntL2Pkt*:*hostPing*";

    return RUN_ALL_TESTS();
}
