#!/bin/bash
#
# Clean up bridges from previous run, in case vmx wasn't properly shut down.
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

# Clean up external bridge
brctl show | grep br-ext
if [ $? = 0 ];
then
    cd $HOST_VMX_LOC/vmx; ./vmx.sh --stop
fi

# Clean up internal bridge
int_br_name="br-int-vmx1"
brctl show | grep $int_br_name
if [ $? = 0 ];
then
    ifconfig $int_br_name down
    brctl delbr $int_br_name
fi

# Clean up interface bridges
done=0
while [ $done = 0 ]; do
    vmx_link=`brctl show | grep vmx_link | head -n1 | awk '{print $1}'`
    if [ "$vmx_link" = "" ];
    then
        done=1
    else
        echo "Removing link $vmx_link"
        ifconfig $vmx_link down
        brctl delbr $vmx_link
    fi
done

# Make sure default network is active
default_network_status=`virsh net-list --all|grep default |awk '{print $2}'`
if [ "$default_network_status" == "inactive" ]
then
    log_debug "Making default network active"
    ifconfig virbr0 down
    brctl delbr virbr0
fi

