#!/bin/bash
#
# Wrapper script to run cpplint on C++ source code files
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


MY_DIR="$(dirname "$0")"
source "$MY_DIR/cmn.sh"

validate_cpplint

#CPPLINT=/root/styleguide/cpplint/cpplint.py
LINT_REPORT_FILENAME=cpplint.report.txt

me=`basename "$0"`
if [ "$#" -eq 0 ]; then
   REPORT_DIR=cpplint-report
else
   REPORT_DIR=$1 
fi

LINT_REPORT_FILE=$REPORT_DIR/$LINT_REPORT_FILENAME

$ECHO ===============================================
$ECHO ===============================================
$DATE 
$ECHO STARTING CPPLINT
$ECHO ===============================================
$ECHO ===============================================

mkdir -p $REPORT_DIR
touch $LINT_REPORT_FILE
$DATE > $LINT_REPORT_FILE
echo "========== cpplint report ==========" >> $LINT_REPORT_FILE

for f in `find $JP4AGENT_REPO -type f \( -name \*.h -o -name \*.cpp \) ! -name "*.pb.*" -print`; do
    echo $CPPLINT --filter=\'$CPPLINT_FILTER\' $f 
    #$CPPLINT --filter='$CPPLINT_FILTER' $f >> $LINT_REPORT_FILE 2>&1
    $CPPLINT --filter='-whitespace/braces,-build/c++11' $f >> $LINT_REPORT_FILE 2>&1
done

$ECHO ===============================================
$ECHO ===============================================
$DATE 
$ECHO FINISHED CPPLINT
$ECHO ===============================================
$ECHO ===============================================

exit 0
