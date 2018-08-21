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
    unsigned char m[6] = {0x88, 0xa2, 0x5e, 0x91, 0x75, 0xff};
    std::string mac((char *) &m[0], 6);
    ControllerAddVrfEntry(0x800, mac, 0x37373702, 5);
    EXPECT_EQ(1, 1);
}

// Test2: Class ID Table
TEST_F(P4BRCMSPINE, ClassId)
{
    ControllerAddClassIdEntry(100, 5, 0x800, 6, 7, 0x37373702, 30, 40, 10);
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
    ControllerAddRtEncapEntry(0, 0, 0, 0x44a25e9175ff, 0x55a25e9175ff,
                              13, 10, "vrf");

    // Default route for the VRF 5 table.
    ControllerAddRtEncapEntry(0, 0, 5, 0x66a25e9175ff, 0x77a25e9175ff,
                              13, 20, "vrf");

    // Override route.
    ControllerAddRtEncapEntry(0x37373702, 24, 0, 0x44a25e9175ff, 0x55a25e9175ff,
                              13, 10, "override");

    // VRF 5 route.
    ControllerAddRtEncapEntry(0x47474702, 24, 5, 0x66a25e9175ff, 0x77a25e9175ff,
                              13, 20, "vrf");

    // Fallback route.
    ControllerAddRtEncapEntry(0x57575702, 24, 0, 0x88a25e9175ff, 0x99a25e9175ff,
                              13, 30, "fallback");

    EXPECT_EQ(1, 1);
}

// Test5: Punt Table
TEST_F(P4BRCMSPINE, Punt)
{
    ControllerAddPuntEntry(9, 13, 0x800, 0x88a25e9175ff,
                           6, 5, 0x10000001, 0x20000001,
                           6, 0, 0, 12,
                           4, 10, 5, 3);

    EXPECT_EQ(1, 1);
}
