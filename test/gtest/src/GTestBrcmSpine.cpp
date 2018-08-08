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

