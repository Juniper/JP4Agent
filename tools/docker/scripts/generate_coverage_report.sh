#!/bin/bash
#
# Regression scripts
#
# Created by Manmeet Singh, January 2018
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


MY_DIR="$(dirname "$0")"
source "$MY_DIR/cmn.sh"

me=`basename "$0"`
if [ "$#" -eq 0 ]; then
   echo "Usage: $me <report dir>"
   exit 1
fi

REPORT_DIR=$1
REPO_DIR=$JP4AGENT_DIR

components=(
    jp4agent
    utils
    pi
    afi
    targets/aft/aft
    targets/aft
)

if [ "$#" -eq 0 ]; then
   REPORT_DIR=cpplint-report
   else
      REPORT_DIR=$1 
      fi

for component in "${components[@]}"
do
    echo "===== Generating code coverage report for ${component} ====="
    COMPONENT_SRC_DIR=$REPO_DIR/src/${component}/src
    mkdir -p $REPORT_DIR/${component} 
    OUTPUT_FILE=$REPORT_DIR/${component}/coverage.html
    echo COMPONENT_SRC_DIR=$COMPONENT_SRC_DIR
    echo OUTPUT_FILE=$OUTPUT_FILE
    cd $COMPONENT_SRC_DIR; gcovr --object-directory=../obj -r . --html --html-details -o $OUTPUT_FILE
    #mv coverage.* $REPORT_DIR/${component}/
done

#files=`find .. -type f | grep "gcda\|gcno"`
#for i in $files
#do 
#    rm  $i 
#done

exit 0
