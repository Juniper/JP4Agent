#!/bin/bash
#
# VMX setup scripts
#
# Created by Sandesh Kumar Sodhi, December 2017
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

#
# Stop VMX
#
cd $VMX_DIR
./vmx.sh --stop
./vmx.sh --cleanup
./vmx.sh --unbind-dev

log_debug "Before bridge cleanup"
brctl show
ip link set br-int-vmx1 down
brctl delbr br-int-vmx1


$VIRSH net-undefine br-ext
$VIRSH net-undefine br-int-vmx1

$BRCTL delbr br-ext

log_debug "After bridge cleanup"
brctl show
exit 0
