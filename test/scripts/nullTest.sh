#!/bin/bash
#
# Run null gtest with Travis
#
#
# Created by Manmeet Singh, February 2018
# Copyright (c) [2018] Juniper Networks, Inc. All rights reserved.
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

# Start JP4Agent
cd /root/JP4Agent/src/targets/null/bin
./run-jp4agent &
sleep 10

# Execute Null Test
cd /root/JP4Agent/test/gtest/bin
./run-jp4agent-gtest nullTest
sleep 10

# Shut down JP4Agent
. ../../../tools/docker/scripts/shutdown_jp4agent.sh
