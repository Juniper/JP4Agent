//
// TestUtils.cpp
//
// Test Utils
//
// Created by Sandesh Kumar Sodhi, January 2017
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

#include "TestUtils.h"

#include <arpa/inet.h>
#include <libnet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <pcap.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "P4InfoUtils.h"
#include "TapIf.h"
#include "gtest/gtest.h"

#define SEND_BUF_SIZE 1500
#define ETHER_PAYLOAD_BUF_SIZE 5000
#define SEND_BUF_STR_SIZE 5000

static pcap_t *pcap_hdl;  // Packet capture handle.

void
sleep_thread_log(std::chrono::milliseconds nms)
{
    std::cout << "Sleeping for " << nms.count() << "ms..." << std::flush;
    std::this_thread::sleep_for(nms);
    std::cout << "Done." << std::endl;
}

static std::string
getShellCmdOutput(const std::string &cmd)
{
    std::array<char, 128> buff;
    std::string           output;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buff.data(), 128, pipe.get()) != NULL) {
            output += buff.data();
        }
    }
    return output;
}

// SIGALRM handler to timeout the pkt listen loop.
static void
alarm_handler(int sig)
{
    pcap_breakloop(pcap_hdl);
}

/*
 * Listen to and capture packets on the specified interface and save them in a
 * specified output file. This function should be invoked as a forked child
 * process since it uses a signal handler for SIGALRM to break the packet
 * capture loop.
 */
void
listen_pkt_intf(const char *intf, const char *dump_file, unsigned int num_pkts,
                unsigned int to_sec)
{
    int              ret         = 0;
    int              exit_status = EXIT_FAILURE;
    char             errbuf[PCAP_ERRBUF_SIZE];
    pcap_dumper_t *  pd = nullptr;
    struct sigaction psa;

    std::cout << "\nListening for packets on interface " << intf << std::endl;
    pcap_hdl = pcap_open_live(intf, BUFSIZ, 0, -1, errbuf);
    if (pcap_hdl == nullptr) {
        std::cout << "pcap_open_live() failed due to " << errbuf << std::endl;
        goto quit;
    }

    pd = pcap_dump_open(pcap_hdl, dump_file);
    if (pd == nullptr) {
        std::cout << "Failed to open pcap file for writing." << std::endl;
        goto quit;
    }

    psa.sa_handler = alarm_handler;
    sigaction(SIGALRM, &psa, NULL);
    alarm(to_sec);

    ret = pcap_loop(pcap_hdl, num_pkts, pcap_dump,
                    reinterpret_cast<u_char *>(pd));
    if (ret == -2) {
        std::cout << "Timed out listening for packets on intf " << intf
                  << std::endl;
        goto quit;
    }

    exit_status = EXIT_SUCCESS;
quit:
    if (pd) {
        pcap_dump_close(pd);
    }
    if (pcap_hdl) {
        pcap_close(pcap_hdl);
    }
    exit(exit_status);
}

/*
 * Capture packets on the specified interfaces by forking off a process per
 * interface.
 */
std::vector<pid_t>
start_pktcap(const std::string &             tOutputDir,
             const std::vector<std::string> &intfs, unsigned int num_pkts,
             unsigned int timeout_sec)
{
    std::string        ifPcapFile;
    std::vector<pid_t> pids;

    for (const auto &intf : intfs) {
        ifPcapFile = tOutputDir + "/" + intf + ".pcap";
        pid_t cid  = fork();
        if (cid == -1) break;
        if (cid > 0) {
            pids.push_back(cid);
        } else {
            listen_pkt_intf(intf.c_str(), ifPcapFile.c_str(), num_pkts,
                            timeout_sec);
        }
    }

    if (pids.size() < intfs.size()) {
        stop_pktcap(intfs, pids);
        pids.clear();
    }

    return pids;
}

// Wait for pcap child processes and verify exit status.
bool
stop_pktcap(const std::vector<std::string> &intfs,
            const std::vector<pid_t> &      pids)
{
    bool pcap_status = true;

    for (size_t i = 0; i < pids.size(); i++) {
        int status = 0;
        waitpid(pids[i], &status, 0);
        if (!WIFEXITED(status) || (EXIT_SUCCESS != WEXITSTATUS(status))) {
            ADD_FAILURE() << "Packet capture failed or timed out for intf "
                          << intfs[i];
            pcap_status = false;
        }
    }

    return pcap_status;
}

void
tVerifyPackets(const std::string &tOutputDir, const std::string &tExpectedDir,
               const std::vector<std::string> &interfaces)
{
    const std::string tsharkBinary =
        "/usr/bin/tshark -o ip.check_checksum:TRUE";

    std::string cmd, ifPcapFile, ifPktsFile, ifExpectedPcapFile,
        ifExpectedPktsFile;

    for (const auto &interface : interfaces) {
        ifPcapFile         = tOutputDir + "/" + interface + ".pcap";
        ifPktsFile         = tOutputDir + "/" + interface + ".pkts";
        ifExpectedPcapFile = tExpectedDir + "/" + interface + ".pcap";
        ifExpectedPktsFile = tOutputDir + "/" + interface + ".pkts.expected";

        cmd = tsharkBinary + " -nx " + " -r " + ifExpectedPcapFile + " > " +
              ifExpectedPktsFile + " 2> /dev/null";
        int ret = system(cmd.c_str());
        EXPECT_EQ(0, ret);
        cmd = tsharkBinary + " -nx " + " -r " + ifPcapFile + " > " +
              ifPktsFile + " 2> /dev/null";
        ret = system(cmd.c_str());
        EXPECT_EQ(0, ret);
        cmd = "diff " + ifExpectedPktsFile + " " + ifPktsFile;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        std::cout << cmd << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        ret = system(cmd.c_str());
        EXPECT_EQ(0, ret);

        if (ret != 0) {
            // diff present
            std::cout << "Expected packets and actual packets differ"
                      << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "Expected: " << ifExpectedPcapFile << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            cmd = tsharkBinary + " -nVx " + " -r " + ifExpectedPcapFile +
                  " 2> /dev/null";
            std::cout << getShellCmdOutput(cmd);
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "Actual: " << ifPcapFile << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            cmd =
                tsharkBinary + " -nVx " + " -r " + ifPcapFile + " 2> /dev/null";
            std::cout << getShellCmdOutput(cmd);
        } else {
            // No diff
            std::cout << "SUCCESS: Actual packets matches with expected packets"
                      << std::endl;
        }
    }
}

int
SendRawEth(const char *ifName, TestPacketLibrary::TestPacketId tcPktNum)
{
    int   i;
    int   ether_type;
    char *ether_payload = NULL;
    int * dst_mac;

    int                  sockfd;
    struct ifreq         if_idx;
    struct ifreq         if_mac;
    int                  tx_len = 0;
    char                 sendbuf[SEND_BUF_SIZE];
    struct ether_header *eh = (struct ether_header *)sendbuf;
    struct sockaddr_ll   socket_address;
    int                  byte_count;

    TestPacket *testPkt = testPacketLibrary.getTestPacket(tcPktNum);

    ether_type    = testPkt->getEtherType();
    dst_mac       = testPkt->getDstMac();
    ether_payload = testPkt->getEtherPayloadStr();

    // RAW socket
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        perror("socket");
    }

    // Get interface index */
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifName, IFNAMSIZ);

    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
        perror("SIOCGIFINDEX");
    }

    // Get the MAC address of the interface to send on
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifName, IFNAMSIZ);
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0) {
        perror("SIOCGIFHWADDR");
    }

    //
    // Build Ethernet header
    //
    memset(sendbuf, 0, SEND_BUF_SIZE);

    // Destination MAC
    for (i = 0; i < MAC_ADDR_LEN; i++) {
        eh->ether_dhost[i] = dst_mac[i];
    }

    // Source MAC
    for (i = 0; i < MAC_ADDR_LEN; i++) {
        eh->ether_shost[i] =
            (reinterpret_cast<uint8_t *>(&if_mac.ifr_hwaddr.sa_data))[i];
    }

    // Ethertype field
    eh->ether_type = htons(ether_type);
    tx_len += sizeof(struct ether_header);

    getRidOfSpacesFromString(ether_payload);

    byte_count = convertHexStringToBinary(ether_payload, &sendbuf[tx_len],
                                          (SEND_BUF_SIZE - tx_len));

    tx_len += byte_count;

    // Index of the network device
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    // Address length
    socket_address.sll_halen = ETH_ALEN;

    // Destination MAC
    for (i = 0; i < MAC_ADDR_LEN; i++) {
        socket_address.sll_addr[i] = dst_mac[i];
    }

    std::cout << "Sending packet on interface " << ifName << "..." << std::endl;
    pktTrace("Raw socket send", sendbuf, tx_len);

    // Send packet
    if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr *)&socket_address,
               sizeof(struct sockaddr_ll)) < 0) {
        std::cout << "Error: Send failed!" << std::endl;
        return -1;
    } else {
        std::cout << "Send success!!!" << std::endl;
        return 0;
    }
}

void
send_arp_req(const char *ifname, const char *target_ip_addr)
{
    char      errbuf[LIBNET_ERRBUF_SIZE]{};
    libnet_t *l = libnet_init(LIBNET_LINK, ifname, errbuf);
    ASSERT_NE(nullptr, l) << "libnet_init() failed: " << errbuf;

    std::unique_ptr<libnet_t, decltype(&libnet_destroy)> lup{l, libnet_destroy};

    uint32_t sip = libnet_get_ipaddr4(l);
    ASSERT_NE((uint32_t)-1, sip)
        << "Couldn't get own IP addr: " << libnet_geterror(l);

    struct libnet_ether_addr *src_mac_addr = libnet_get_hwaddr(l);
    ASSERT_NE(nullptr, src_mac_addr)
        << "Couldn't get own MAC addr: " << libnet_geterror(l);

    std::string tip_str(target_ip_addr);
    uint32_t    tip = libnet_name2addr4(l, &tip_str[0], LIBNET_DONT_RESOLVE);
    ASSERT_NE((uint32_t)-1, tip)
        << "Error converting IP addr: " << libnet_geterror(l);

    uint8_t       mac_zero_addr[6]{};
    auto *        sip_bytes = reinterpret_cast<uint8_t *>(&sip);
    auto *        tip_bytes = reinterpret_cast<uint8_t *>(&tip);
    libnet_ptag_t pt =
        libnet_autobuild_arp(ARPOP_REQUEST, src_mac_addr->ether_addr_octet,
                             sip_bytes, mac_zero_addr, tip_bytes, l);
    ASSERT_NE(-1, pt) << "Error building ARP header: " << libnet_geterror(l);

    uint8_t mac_bcast_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    pt = libnet_autobuild_ethernet(mac_bcast_addr, ETHERTYPE_ARP, l);
    ASSERT_NE(-1, pt) << "Error building ethernet hdr" << libnet_geterror(l);

    const int bytes_written = libnet_write(l);
    ASSERT_NE(-1, bytes_written)
        << "Error writing packet: " << libnet_geterror(l);

    std::cout << __func__
              << "(): Arp request packet sent (size: " << bytes_written
              << ")\n";
}
