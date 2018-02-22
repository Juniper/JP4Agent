#!/bin/bash
#
# Regression scripts
#
# Created by Sandesh Kumar Sodhi, January 2018
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

function validate_jp4agent_repo() {
    if [ "dummy$JP4AGENT_REPO" == "dummy" ]; then
        echo "Please set env variable 'JP4AGENT_REPO' to JP4Agent repo"
        echo "E.g., export JP4AGENT_REPO=/home/sandesh/JP4Agent"
        exit 1
    fi

    if [ ! -d "$JP4AGENT_REPO" ]; then
        echo "Error: Repo '$JP4AGENT_REPO' does not exist"
        exit 1
    fi
}

validate_jp4agent_repo

REPO_DIR=$JP4AGENT_REPO

DOCKER_HUB_TAG=juniper/p4-vmx:latest
CONFIG_DIR_NAME=config
SCRIPTS_DIR_NAME=scripts
REPORT_DIR_NAME=report
LOG_DIR_NAME=log
REPO_CONFIG_DIR=$REPO_DIR/tools/$CONFIG_DIR_NAME
REPO_SCRIPTS_DIR=$REPO_DIR/tools/$SCRIPTS_DIR_NAME
REPO_REPORT_DIR=$REPO_DIR/tools/$REPORT_DIR_NAME
REPO_LOG_DIR=$REPO_DIR/tools/$LOG_DIR_NAME
VMX_CONFIG_FILE=$REPO_CONFIG_DIR/vmx-cfg.xml
REGRESSION_CONFIG_FILE=$REPO_CONFIG_DIR/regression-cfg.xml
mkdir -p $REPO_LOG_DIR


REGRESSION_DIR=$REPO_DIR/tools
REGRESSION_CONFIG_DIR=$REGRESSION_DIR/$CONFIG_DIR_NAME
REGRESSION_REPORT_DIR=$REGRESSION_DIR/$REPORT_DIR_NAME
REGRESSION_LOG_DIR=$REGRESSION_DIR/$LOG_DIR_NAME
REGRESSION_SCRIPTS_DIR=$REGRESSION_DIR/$SCRIPTS_DIR_NAME


#
# Validations
#

function is_valid_ip() {
    local IP=$1
    #
    # result=0 : $IP is a valid ip
    # result=1 : $IP is invalid ip
    #
    local result=1  

    invalid_ips='
        0.0.0.0
        255.255.255.255
        '

    for invalid_ip in $invalid_ips
    do
        if [ $IP == $invalid_ip ]; then
            return $result
        fi
    done

    #
    # Success: $IP is a valid ip
    # 
    result=0
    return $result
}

function validate_vmx_config() {
    HOST_VMX_LOC=$(xmllint --xpath "//vmxConfig/hostVMXLocation/text()" $VMX_CONFIG_FILE)
    if [ ! -d "$HOST_VMX_LOC" ]; then
        echo "Error: Location '$HOST_VMX_LOC' does not exist"
        exit 1
    fi

    #
    # Read config
    #
    VCP_IP=$(xmllint --xpath "string(//vmxConfig/vcp/@IP)" $VMX_CONFIG_FILE)
    VFP_IP=$(xmllint --xpath "string(//vmxConfig/vfp/@IP)" $VMX_CONFIG_FILE)

    if ! is_valid_ip $VCP_IP; then
        echo "Invalid vcp IP: $VCP_IP. Please configure a valid vcp IP in $VMX_CONFIG_FILE" 
        exit 1
    fi

    if ! is_valid_ip $VFP_IP; then
        echo "Invalid vfp IP: $VFP_IP. Please configure a valid vfp IP in $VMX_CONFIG_FILE" 
        exit 1
    fi
}

function validate_cpplint() {
    if [ "dummy$CPPLINT" == "dummy" ]; then
        echo "Please set env variable 'CPPLINT' to cpplint.py"
        echo "E.g., export CPPLINT=/root/styleguide/cpplint/cpplint.py"
        exit 1
    fi

    if [ ! -f "$CPPLINT" ]; then
        echo "Error: '$CPPLINT' not found"
        exit 1
    fi
}

function validate_regression_config() {
    run_regression=$(xmllint --xpath "string(//regressionConfig/@run_regression)" $REGRESSION_CONFIG_FILE)

    if [ "dummy$run_regression" == "dummy0" ]
    then
      #echo "run_regression=$run_regression (File $REGRESSION_SCRIPTS_DIR/config.txt)"
      echo "Regression disabled. Please check config/regression-cfg.xml"
      echo "(//regressionConfig/@run_regression: 1 = Enabled, 0 = Disabled)"
      echo "Do you want to run regression? (answer yes to proceed)"
      read -t 10 -n 3 answer
      if [ $? == 0 ]; then
          echo "Your answer is: $answer"
          if [ "dummy"$answer == "dummyyes" ]; then
              echo "Your answer is: $answer. Proceeding with regression"
          else
              echo "Answer is not 'yes'! Exiting..."
              exit 1
          fi

      else
          echo "Can't wait anymore! Exiting..."
          exit 1
      fi
    fi

    cfg_run_cpplint=$(xmllint --xpath "//regressionConfig/runCpplint/text()" $REGRESSION_CONFIG_FILE)

    if [ "$cfg_run_cpplint" == "yes" ]; then
        validate_cpplint
    fi
}
 
#
# Config
#
#GIT_REPO_LOCATION=git@github.com:Juniper
GIT_REPO_LOCATION=git@git.juniper.net:sksodhi
REPO_NAME=JP4Agent
VMX_BUNDLE_LOCATION=$REGRESSION_DIR
VMX_BUNDLE=vmx-bundle-17.4R1.6.tgz
DOCKER_REGISTRY_SERVER=10.102.144.118
DOCKER_REGISTRY_PORT=5000
DOCKER_REGISTRY=$DOCKER_REGISTRY_SERVER:$DOCKER_REGISTRY_PORT
DOCKER_TAG=juniper/p4-vmx:latest


INDIVIDUAL_VMX_SETUP_DIR_NAME_PREFIX=vmxsetup
INDIVIDUAL_REGRESSION_REPORT_DIR_NAME_PREFIX=regress
INDIVIDUAL_REGRESSION_LOG_FILE_NAME=log.txt
INDIVIDUAL_REGRESSION_REPORT_FILE_NAME=report.txt
FINAL_REGRESSION_REPORT_FILE_NAME=regression.xml
FINAL_REGRESSION_REPORT_FILE=$REGRESSION_REPORT_DIR/$FINAL_REGRESSION_REPORT_FILE_NAME

#
# Docker
#
DOCKER_NAME_PREFIX=jp4_


#
# GTEST
#
GTEST_RESULT_DTD_FILENAME="gtest-result.dtd"
GTEST_RESULT_XSL_FILENAME="gtest-result.xsl"
GTEST_RESULT_XML_FILENAME="gtest.xml"
GTEST_NOT_RUN_XML_FILENAME="gtest-not-run.xml"

#
# Deleting old reports
#
NUM_LAST_REPORTS_TO_PRESERVE=100

GIT=/usr/bin/git
DOCKER=/usr/bin/docker
ECHO=/bin/echo
DATE=/bin/date
CP=/bin/cp
RM=/bin/rm
DATE=/bin/date
MKDIR=/bin/mkdir
TOUCH=/usr/bin/touch
XMLLINT=/usr/bin/xmllint
SED=/bin/sed
CUT=/usr/bin/cut
AWK=/usr/bin/awk
DF=/bin/df

#TIME=`$DATE +%m%d%y%H%M%S`
#TIME=`$DATE`
TIME=`$DATE "+%F %T"`
DATE_TAG=`$DATE +%m%d%y_%H%M`

function log_debug {
    DATE_STR=`$DATE`
    echo "[$DATE_STR] DEBUG: \"$@\""
}

function log_prominent {
    DATE_STR=`$DATE`
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "[$DATE_STR] \"$@\""
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
}

function log_error {
    DATE_STR=`$DATE`
    echo "~~~~~~~~~~~ ERROR ~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo "[$DATE_STR] ERROR:\"$@\""
    echo "~~~~~~~~~~~ ERROR ~~~~~~~~~~~~~~~~~~~~~~~~~~~"
}

#
# Run a bash shell command
#
function run_command {
    DATE_STR=`$DATE`
    echo "[$DATE_STR] Executing command \"$@\" ..."
    $@
    if [ $? -eq 0 ]
    then
      DATE_STR=`$DATE`
      echo "~~~~~~~~~~~ SUCCESS ~~~~~~~~~~~~~~~~~~~~~~~~~"
      echo "[$DATE_STR] Command \"$@ \" : SUCCESS"
      echo "~~~~~~~~~~~ SUCCESS ~~~~~~~~~~~~~~~~~~~~~~~~~"
    else
      DATE_STR=`$DATE`
      echo "~~~~~~~~~~~ ERROR ~~~~~~~~~~~~~~~~~~~~~~~~~~~"
      echo "[$DATE_STR] Command \"$@ \": ERROR"
      echo "~~~~~~~~~~~ ERROR ~~~~~~~~~~~~~~~~~~~~~~~~~~~"
      exit 1
    fi
}

#
# Repo git pull
#
function do_repo_git_pull {
    DO_GIT_PULL=$(xmllint --xpath "//regressionConfig/doGitPull/text()" $REGRESSION_CONFIG_FILE)

    if [ "dummy$DO_GIT_PULL" == "dummyyes" ]
    then
        cd $REPO_DIR
        run_command $GIT stash
        run_command $GIT pull
        run_command $GIT stash pop
    fi
    run_command $GIT --no-pager diff
}

#
# Add stylesheet tag to an XML file
#
function add_stylesheet_tag_to_xml_file {
    _xml_f_=$1
    if grep -q "xml-stylesheet" "$_xml_f_"; then
        log_debug "$_xml_f_ already has 'xml-stylesheet' tag"    
    else
        #
        # Adding xml-stylesheet tag and dtd
        #
        $SED -i '1 a <?xml-stylesheet type="text/xsl" href="./gtest-result.xsl" ?>' $_xml_f_
        $SED -i '2 a <!DOCTYPE testsuites SYSTEM "./gtest-result.dtd">'             $_xml_f_
        log_debug "Adding xml-stylesheet to $_xml_f_"
    fi
}

