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

report_dirs=`ls -1t $REGRESSION_REPORT_DIR | grep regress_`

printf "\nGenerating final report...\n"

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"                 > $FINAL_REGRESSION_REPORT_FILE
echo "<?xml-stylesheet type=\"text/xsl\" href=\"./regression.xsl\" ?>" >> $FINAL_REGRESSION_REPORT_FILE
echo "<rafi>"                                                    >> $FINAL_REGRESSION_REPORT_FILE
echo "    <header>"                                              >> $FINAL_REGRESSION_REPORT_FILE
echo "        <step name=\"regress\" > </step>"                  >> $FINAL_REGRESSION_REPORT_FILE
echo "        <step name=\"GTEST\" > </step>"                    >> $FINAL_REGRESSION_REPORT_FILE
echo "    </header>"                                             >> $FINAL_REGRESSION_REPORT_FILE

for dir in $report_dirs; 
do
    echo ""                                                      >> $FINAL_REGRESSION_REPORT_FILE
    while IFS='' read -r line || [[ -n "$line" ]]; do
	echo "    $line"                                             >> $FINAL_REGRESSION_REPORT_FILE
    done < "$REGRESSION_REPORT_DIR/$dir/$INDIVIDUAL_REGRESSION_REPORT_FILE_NAME"
done

echo "</rafi>"                                                   >> $FINAL_REGRESSION_REPORT_FILE
exit 0
