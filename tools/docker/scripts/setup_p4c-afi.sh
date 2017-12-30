#!/bin/bash
#
# Advanced Forwarding Interface : AFI client examples
#
# Created by Sandesh Kumar Sodhi, January 2017
# Copyright (c) [2017] Juniper Networks, Inc. All rights reserved.
#
# All rights reserved.
#
# Notice and Disclaimer: This code is licensed to you under the Apache
# License 2.0 (the "License"). You may not use this code except in compliance
# with the License. This code is not an official Juniper product. You can
# obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# Third-Party Code: This code may depend on other components under separate
# copyright notice and license terms. Your use of the source code for those
# components is subject to the terms and conditions of the respective license
# as noted in the Third-Party source code file.
#

#
# Setup p4c-afi
# Sandesh Kumar Sodhi
#


cd /root ; git clone --recursive https://github.com/p4lang/p4c.git
mkdir -p /root/p4c/extensions
ln -s /root/JP4Agent/p4c-backend/afi /root/p4c/extensions/afi
ln -s /root/JP4Agent/p4c-backend/p4c.afi.cfg /root/p4c/tools/driver/p4c_src/p4c.afi.cfg

sed -i '/p4c_src\/p4c.bmv2.cfg/a \ \ p4c_src/p4c.afi.cfg' /root/p4c/tools/driver/CMakeLists.txt

mkdir -p /root/p4c/build
cd /root/p4c/build ; cmake .. -DCMAKE_BUILD_TYPE=DEBUG ; make -j4
