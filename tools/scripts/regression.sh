#!/bin/bash
#
# Regression scripts
#
# Created by Sandesh Kumar Sodhi, January 2018
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

if [ "$#" -ne 2 ]; then
    echo "Usage   : ./$me <docker-name> <report-dir>"
    echo "Example : ./$me afiut afiut-report"
    exit 1
else
    DOCKER_NAME=$1
    REGRESS_REPORT_DIR_NAME=$2 
fi

$ECHO ===============================================
$ECHO ===============================================
$DATE 
$ECHO STARTING REGRESSION
$ECHO ===============================================
$ECHO ===============================================

CFG_SETUP_VMX=$(xmllint --xpath "//regressionConfig/setupVMX/text()" $REGRESSION_CONFIG_FILE)
if [ "$CFG_SETUP_VMX" == "yes" ]; then
    for _d_ in `$DOCKER ps -a | grep ${DOCKER_NAME_PREFIX} | $AWK '{print $1}'`; 
    do 
        DOCKER_RM_CMD="$DOCKER rm -f $_d_"
        echo "Removing docker container $_d_  ($DOCKER_RM_CMD)"
        run_command "$DOCKER_RM_CMD"
    done

    #
    # Build docker containers
    # Building container works only when 
    # no container with option '--network=host' is running.
    #
    cd $REGRESSION_SCRIPTS_DIR
    run_command ./build_push_docker.sh
fi

$ECHO ===============================================
$DATE 
$ECHO RUN regression
$ECHO ===============================================

cd $REGRESSION_SCRIPTS_DIR

if [ "$CFG_SETUP_VMX" == "yes" ]; then
    run_command ./docker_setup_vmx.exp $REPO_DIR $DOCKER_NAME $DOCKER_TAG $REGRESS_REPORT_DIR_NAME
fi

CFG_RUN_GTESTS=$(xmllint --xpath "//regressionConfig/runGTESTs/text()" $REGRESSION_CONFIG_FILE)
if [ "$CFG_RUN_GTESTS" == "yes" ]; then
    run_command ./docker_run_gtests.exp $REPO_DIR $DOCKER_NAME $DOCKER_TAG $REGRESS_REPORT_DIR_NAME
fi

run_command "$DOCKER ps -a"

$ECHO ===============================================
$ECHO ===============================================
$DATE 
$ECHO FINISHED REGRESSION
$ECHO ===============================================
$ECHO ===============================================

exit 0
