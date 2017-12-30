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

#ifndef __TestUtils__
#define __TestUtils__

#include <net/if.h>
#include <linux/if_tun.h>
#include <atomic>

extern std::string gTestTimeStr;
extern std::string gtestOutputDirName;
extern std::string gtestExpectedDirName;
extern std::string tsharkBinary;
extern std::atomic<bool> test_complete;

void getTimeStr(std::string &timeStr);
void tapIfReadPkts(std::string &tapName);
void tStartTsharkCapture(std::string &tcName,
                         std::string &tName,
                         std::vector<std::string> &interfaces);
void tVerifyPackets(std::string &tcName,
                         std::string &tName,
                         std::vector<std::string> &interfaces);
void stopTsharkCapture(void);


extern int SendRawEth (const std::string &ifNameStr, TestPacketLibrary::TestPacketId tcPktNum);


#endif // __TestUtils__
