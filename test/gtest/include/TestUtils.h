//
// TestUtils.h
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
#ifndef TEST_GTEST_INCLUDE_TESTUTILS_H_
#define TEST_GTEST_INCLUDE_TESTUTILS_H_

#include <linux/if_tun.h>
#include <net/if.h>

#include <chrono>
#include <string>
#include <vector>

#include "TestPacket.h"

extern std::string gTestTimeStr;
extern std::string gtestOutputDirName;
extern std::string gtestExpectedDirName;
extern std::string tsharkBinary;

std::string getTimeStr();
void        sleep_thread_log(std::chrono::milliseconds nms);
void        tVerifyPackets(const std::vector<std::string> &interfaces);
void start_pktcap(const std::vector<std::string> &intfs, unsigned int num_pkts,
                  unsigned int timeout_sec, std::vector<pid_t> &pids);
bool stop_pktcap(const std::vector<std::string> &intfs,
                 const std::vector<pid_t> &      pids);

int  SendRawEth(const char *ifName, TestPacketLibrary::TestPacketId tcPktNum);
void send_arp_req(const char *ifname, const char *target_ip_addr);

#endif  // TEST_GTEST_INCLUDE_TESTUTILS_H_
