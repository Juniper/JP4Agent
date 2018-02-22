#!/bin/bash
#
# Regression scripts
#
# Delete old reports
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

report_dirs=`ls -1t $REPORT_DIR | grep $INDIVIDUAL_REGRESSION_REPORT_DIR_NAME_PREFIX`

echo "Deleting old reports...(will preserve last $NUM_LAST_REPORTS_TO_PRESERVE reports)"

COUNTER=0
COUNTER_NUM_PRESERVED=0
COUNTER_NUM_DELETED=0
for dir in $report_dirs; 
do
    let COUNTER+=1
    #echo $COUNTER : $d
    if [  $COUNTER -gt $NUM_LAST_REPORTS_TO_PRESERVE ]; then
	echo "Deleting $COUNTER : $REPORT_DIR/$dir"
        rm -rf $REPORT_DIR/$dir
	let COUNTER_NUM_DELETED+=1
    else
	#echo "Will preserve $COUNTER : $dir"
	let COUNTER_NUM_PRESERVED+=1
    fi
done

echo "Deleted $COUNTER_NUM_DELETED reports (preserved last $COUNTER_NUM_PRESERVED reports)"

exit 0
