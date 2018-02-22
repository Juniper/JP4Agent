#!/bin/bash
#
# build/clean all
#
# Created by Sandesh Kumar Sodhi, February 2018
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
#

me=`basename "$0"`
REPO_DIR=..

components=(
    $REPO_DIR/AFI
    $REPO_DIR/src/pi/protos
    $REPO_DIR/src/jp4agent/src
    $REPO_DIR/src/utils/src
    $REPO_DIR/src/pi/src
    $REPO_DIR/src/afi/src
    $REPO_DIR/test/controller/src
    $REPO_DIR/test/gtest/src
    $REPO_DIR/src/targets/aft/aft/src
    $REPO_DIR/src/targets/aft/src
)

if [ "_$1" == "_clean" ]
then
    for component in "${components[@]}"
    do
        echo "===== Running make clean under ${component} ====="
        make clean -C ${component} 
    done
else
    make srcs -C $REPO_DIR/AFI
    for component in "${components[@]}"
    do
        echo "===== Running make under ${component} ====="
        make -C ${component} 
        if [ $? -ne 0 ]
        then
          echo "$me: ERROR!" 
          exit 1
        fi
    done
    echo "$me: SUCCESS!!!"
fi
