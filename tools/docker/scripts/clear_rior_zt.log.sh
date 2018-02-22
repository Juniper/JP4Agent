#!/usr/bin/expect
#
# VMX setup scripts
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

source [file join [file dirname [info script]] lib.exp]
#
# Disable inline ICMP
#
log_debug "Will connect to VFP via ssh and clear /var/log/riot_zt.log file"
set  script_name [file tail $argv0]

proc display_usage {} {
    global script_name
    puts "Usage  : $script_name"
    return 1
}

set spawn_id [login_vfp_ssh]

#
# timeout 30 seconds
#
set timeout 30

expect "$vfp_prompt" { send "ls -lh /var/log/riot_zt.log\r" }
expect "$vfp_prompt" { send "> /var/log/riot_zt.log\r" }
expect "$vfp_prompt" { send "ls -lh /var/log/riot_zt.log\r" }
expect "$vfp_prompt" { send "\r" }
send "exit\r"

close
exit 0
