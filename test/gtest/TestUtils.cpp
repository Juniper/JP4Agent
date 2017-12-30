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

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include "gtest/gtest.h"
#include "TestPacket.h"
#include "Utils.h"
#include "TestUtils.h"
#include "TapIf.h"

#define SEND_BUF_SIZE		    1500
#define ETHER_PAYLOAD_BUF_SIZE	5000

#define SEND_BUF_STR_SIZE	    5000


std::string gTestTimeStr;
std::string gtestOutputDirName;
std::string gtestExpectedDirName = "GTEST_EXPECTED";
std::string tsharkBinary = "/usr/bin/tshark -o ip.check_checksum:TRUE";
std::atomic<bool> test_complete(false);


void
getTimeStr(std::string &timeStr)
{

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,80,"%d%m%Y_%I%M%S",timeinfo);
  timeStr = buffer;
}

void
tapIfReadPkts(std::string &tapName)
{
    TapIf tapIf(tapName.c_str());
    int tapFd = tapIf.init();
    EXPECT_GT(tapFd, 0);

    while (!test_complete.load()) {
        tapIf.ifRead();
    }
}

std::string
getShellCmdOutput(std::string &cmd)
{
    std::array<char, 128> buff;

    std::string output;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buff.data(), 128, pipe.get()) != NULL)
            output += buff.data();
    }
    return output;
}

void
tStartTsharkCapture (std::string &tcName,
                      std::string &tName,
                      std::vector<std::string> &interfaces)
{
    int ret;
    std::string cmd, cmd_output;
    std::string tDir = tcName + "/" + tName;
    std::string tOutputDir = gtestOutputDirName + "/" + tDir;

    cmd = "mkdir -p " + tOutputDir;
    ret = system(cmd.c_str());
    EXPECT_EQ(0, ret);

    for(auto interface : interfaces) {

        std::string ifPcapFile = tOutputDir + "/" + interface  + ".pcap";
        std::string ifstderrFile = tOutputDir + "/" + interface  + ".stderr";
        std::string tsharkCmd = tsharkBinary + " -i " + interface +
                                " -w " + ifPcapFile + " 2> " + ifstderrFile + 
                                " < /dev/null &";

        ret = system(tsharkCmd.c_str());
        EXPECT_EQ(0, ret);

        cmd = "cat " + ifstderrFile;
        cmd_output = getShellCmdOutput(cmd);
        while (cmd_output.find("Capturing on") != std::string::npos) {
            sleep (1);
            cmd_output = getShellCmdOutput(cmd);
        }
        sleep (1);

        //cmd = "rm -f " + ifstderrFile;
        //cmd_output = getShellCmdOutput(cmd);
    }

    //
    // tshark takes somet to run
    //
    sleep(15);
}

void
tVerifyPackets (std::string &tcName,
                std::string &tName,
                std::vector<std::string> &interfaces)
{
    int ret = 0;
    std::string cmd;
    std::string tDir = tcName + "/" + tName;
    std::string tOutputDir = gtestOutputDirName + "/" + tDir;
    std::string tExpectedDir = gtestExpectedDirName + "/" + tDir;

    for(auto interface : interfaces) {
        std::string ifPcapFile = tOutputDir + "/" + interface  + ".pcap";
        std::string ifPktsFile = tOutputDir + "/" + interface  + ".pkts";
        std::string ifExpectedPcapFile = tExpectedDir + "/" + interface  +
                                         ".pcap";
        std::string ifExpectedPktsFile = tOutputDir + "/" + interface  +
                                         ".pkts.expected";

        cmd = tsharkBinary + " -nx " + " -r " + ifExpectedPcapFile + " > " + 
              ifExpectedPktsFile + " 2> /dev/null";
        ret = system(cmd.c_str());
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
            std::cout << "Actual: "<< ifPcapFile << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            cmd = tsharkBinary + " -nVx " + " -r " + ifPcapFile +
                  " 2> /dev/null";
            std::cout << getShellCmdOutput(cmd);
        } else {
            // No diff
            std::cout << "SUCCESS: Actual packets matches with expected packets"
                      << std::endl;
        }
    }
}

void
stopTsharkCapture (void)
{
    std::string cmd = "ps -auwx | grep tshark | grep -v grep";
    std::string cmd_output = getShellCmdOutput(cmd);
    while (cmd_output.find("tshark") != std::string::npos) {
        sleep (1);
        system("/usr/bin/pkill tshark");
        cmd_output = getShellCmdOutput(cmd);
    }
}


int
SendRawEth (const std::string &ifNameStr,
            TestPacketLibrary::TestPacketId tcPktNum)
{
    int         i;
	int         ether_type;
	char       *ether_payload = NULL;
	int        *dst_mac;

	int         sockfd;
	struct      ifreq if_idx;
	struct      ifreq if_mac;
	int         tx_len = 0;
	char        sendbuf[SEND_BUF_SIZE];
	struct      ether_header *eh = (struct ether_header *) sendbuf;
	struct      sockaddr_ll socket_address;
	char        ifName[IFNAMSIZ];
	int         byte_count;

    TestPacket* testPkt = testPacketLibrary.getTestPacket(tcPktNum);

	ether_type    = testPkt->getEtherType();
	dst_mac       = testPkt->getDstMac();
	ether_payload = testPkt->getEtherPayloadStr();

    strncpy(ifName, ifNameStr.c_str(), IFNAMSIZ);
	ifName[IFNAMSIZ - 1] = '\0';

	// RAW socket
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    perror("socket");
	}

	// Get interface index */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);

	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
	    perror("SIOCGIFINDEX");
	}

	// Get the MAC address of the interface to send on 
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
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
		eh->ether_shost[i] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[i];
	}

	// Ethertype field
	eh->ether_type = htons(ether_type);
	tx_len        += sizeof(struct ether_header);

	getRidOfSpacesFromString(ether_payload);

	byte_count = convertHexStringToBinary(ether_payload, 
                                          &sendbuf[tx_len], 
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
	if (sendto(sockfd, sendbuf, tx_len, 0, 
	    (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
	    std::cout << "Error: Send failed!" << std::endl;
		return -1;
	} else {
	    std::cout << "Send success!!!" << std::endl;
		return 0;
	}
}

