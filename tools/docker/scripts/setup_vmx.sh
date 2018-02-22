#!/bin/bash
#
# VMX setup scripts
#
# setup_vmx.sh: Setup VMX
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

scriptname=`basename "$0"`

#
# Regression scripts use this messagees to detect vmx setup status.
# If you change this message, please make the same change in regression scripts.
# Always echo one of these messages while exiting this script:
#
error_status="setup VMX: ___error___"
success_status="setup VMX: ___success___"

#
# echo status and exit
#
function echo_status_and_exit {
    if [ $1 -eq 0 ] 
    then
      $CRONTAB $CFG_FILES_DIR/cron.cfg
      $CRONTAB -l
      echo $success_status
    else
      echo $error_status
    fi  
    exit $1
}

#!/bin/bash
print_usage()
{
    log_error "Incorrect Usage"
    echo "Usage: setup_vmx"
    echo "       setup_vmx [--if <if1> <if2> --vcp <vcp-ip> --vfp <vfp-ip>]"
}

ext_tester=0

while [ $# -gt 0 ]
do
    case "$1" in
        --if)   if1=$2; if2=$3; shift; shift;
                log_debug "if1=$if1, if2=$if2";;
        --vcp)   vcp_ip=$2;  shift;
                 log_debug "vcp_ip = $vcp_ip";;
        --vfp)   vfp_ip=$2;  shift;
                 log_debug "vfp_ip = $vfp_ip";;
            *)   print_usage "$0";exit 0 ;;
    esac
    shift
    ext_tester=1
done

if [ "$ext_tester" == 1 ];
then
    if [ "$if1" == "" ] || [ "$if2" == "" ] || [ "$vcp_ip" == "" ] || [ "$vfp_ip" == "" ] ;
    then
        print_usage "$0";exit 0;
    fi
else
    vcp_ip=$(xmllint --xpath "string(//vmxConfig/vcp/@IP)" $VMX_CONFIG_FILE)
    vfp_ip=$(xmllint --xpath "string(//vmxConfig/vfp/@IP)" $VMX_CONFIG_FILE)
    vcp_console_port=$(xmllint --xpath "string(//vmxConfig/vcp/port)" $VMX_CONFIG_FILE)
    vfp_console_port=$(xmllint --xpath "string(//vmxConfig/vfp/port)" $VMX_CONFIG_FILE)
    vmx_mgmt_if=$(xmllint --xpath "string(//vmxConfig/mgmt-if)" $VMX_CONFIG_FILE)
    if1=$(xmllint --xpath "string(//vmxConfig/IF1)" $VMX_CONFIG_FILE)
    if2=$(xmllint --xpath "string(//vmxConfig/IF2)" $VMX_CONFIG_FILE)
    log_debug "vcp_ip = $vcp_ip"
    log_debug "vfp_ip = $vfp_ip"
    log_debug "vcp_console_port = $vcp_console_port"
    log_debug "vfp_console_port = $vfp_console_port"
    log_debug "if1 = $if1"
    log_debug "if2 = $if2"
fi

VMX_BUNDLE_IMAGE=`ls -1 $VMX_IMAGES_LOC | grep vmx | grep bundle | grep tgz`
JUNOS_VMX_QCOW2_IMG=`ls -1 $VMX_IMAGES_LOC | grep qcow2`
VFPC_IMG=`ls -1 $VMX_IMAGES_LOC | grep vFPC`


#
# Start cron
#
cron_status=`$PS -A | grep cron | wc -l`
if [ "$cron_status" == "0" ]; then
    log_debug "starting cron"
    $CRON
else
    log_debug "cron already running"
fi

#
# Start syslog server
#
rsyslog_status=`$PS -A | grep rsyslogd | wc -l`
if [ "$rsyslog_status" == "0" ]; then
    log_debug "starting rsyslog service"
    service rsyslog start
else
    log_debug "rsyslog service already running"
fi

virtlogd_status=`$PS -A | grep virtlogd | wc -l`
if [ "$virtlogd_status" == "0" ]; then
    log_debug "starting virtlogd service"
    service virtlogd start
else
    log_debug "virtlogd service already running"
fi

#
# Untar vmx bundle image
#
if [ ! -d "$VMX_DIR" ]; then
    run_command $TAR xf $VMX_BUNDLE_IMAGE

    #
    # Check
    #
    if [ ! -d "$VMX_DIR" ]; then
        log_error "VMX bundle untar error"
        echo_status_and_exit 1
    fi
fi

#
# Check if VMX is already running 
#
cd $VMX_DIR
vmx_status=`./vmx.sh --status | grep "Check if vMX is running" | grep No | wc -l`
if [ "$vmx_status" == "0" ]; then
    log_debug "VMX already running"
    echo_status_and_exit 0
else 
    log_debug "VMX not running. Will start it..."
fi

if [ ! -f "/etc/libvirt/qemu.conf.orig" ]; then
    cp /etc/libvirt/qemu.conf /etc/libvirt/qemu.conf.orig
fi
sed -i -e 's/#user = "root"/user = "root"/g' /etc/libvirt/qemu.conf
sed -i -e 's/#group = "root"/group = "kvm"/g' /etc/libvirt/qemu.conf
diff /etc/libvirt/qemu.conf.orig /etc/libvirt/qemu.conf
#
#diff qemu.conf.orig qemu.conf
#229c229
#< #user = "root"
#---
#> user = "root"
#233c233
#< #group = "root"
#---
#> group = "kvm"


$MOUNT -tsecurityfs securityfs /sys/kernel/security
$APPARMOR_PARSER -q -R /etc/apparmor.d/bin.ping          2> /dev/null
$APPARMOR_PARSER -q -R /etc/apparmor.d/usr.sbin.dnsmasq  2> /dev/null
$APPARMOR_PARSER -q -R /etc/apparmor.d/usr.sbin.libvirtd 2> /dev/null
$APPARMOR_PARSER -q -R /etc/apparmor.d/usr.sbin.rsyslogd 2> /dev/null
$APPARMOR_PARSER -q -R /etc/apparmor.d/usr.sbin.tcpdump  2> /dev/null
$CHOWN root:kvm /dev/kvm

#
# VMX configuration
#
if [ ! -f "$VMX_DIR/config/" ]; then
    cp $VMX_DIR/config/vmx.conf $VMX_DIR/config/vmx.conf.orig
fi
$CP -f $CFG_FILES_DIR/vmx.conf $VMX_DIR/config/
if [ ! -f "$VMX_DIR/config/vmx-junosdev.conf" ]; then
    cp $VMX_DIR/config/vmx-junosdev.conf $VMX_DIR/config/vmx-junosdev.conf.orig
fi
$CP -f $CFG_FILES_DIR/vmx-junosdev.conf $VMX_DIR/config/

$SED -i -e "s/REPLACE_WITH_JUNOS_VMX_QCOW2_IMAGE_NAME/$JUNOS_VMX_QCOW2_IMG/g" $VMX_DIR/config/vmx.conf
$SED -i -e "s/REPLACE_WITH_VFPC_IMAGE_NAME/$VFPC_IMG/g" $VMX_DIR/config/vmx.conf

$CP -f $SCRIPTS_DIR/config_vmx_links.sh $NEW_SCRIPTS_DIR/config_vmx_links.sh
$CP -f $SCRIPTS_DIR/config_vfp_ext_if.exp $NEW_SCRIPTS_DIR/config_vfp_ext_if.exp
$CP -f $CFG_FILES_DIR/junos_config.txt $NEW_SCRIPTS_DIR/junos_config.txt

$CP -f $VMX_IMAGES_LOC/metadata-usb-re.img $VMX_DIR/images/

$SED -i -e "s/REPLACE_WITH_VCP_IP/$vcp_ip/g" $VMX_DIR/config/vmx.conf
$SED -i -e "s/REPLACE_WITH_VCP_PORT/$vcp_console_port/g" $VMX_DIR/config/vmx.conf
$SED -i -e "s/REPLACE_WITH_VFP_IP/$vfp_ip/g" $VMX_DIR/config/vmx.conf
$SED -i -e "s/REPLACE_WITH_VFP_PORT/$vfp_console_port/g" $VMX_DIR/config/vmx.conf
$SED -i -e "s/REPLACE_WITH_MGMT_IF/$vmx_mgmt_if/g" $VMX_DIR/config/vmx.conf
$SED -i -e "s/REPLACE_WITH_IF1/$if1/g" $VMX_DIR/config/vmx-junosdev.conf
$SED -i -e "s/REPLACE_WITH_IF2/$if2/g" $VMX_DIR/config/vmx-junosdev.conf
$SED -i -e "s/REPLACE_WITH_VFP_IP/$vfp_ip/g" $NEW_SCRIPTS_DIR/config_vfp_ext_if.exp
$SED -i -e "s/REPLACE_WITH_VCP_IP/$vcp_ip/g" $NEW_SCRIPTS_DIR/junos_config.txt

#
#  Increase sleep in vmx_system_setup_restart_libvirt() to fix following issue
#  Attempt to start libvirt-bin......................[OK]
#  Sleep 2 secs......................................[OK]
#  Check libvirt support for hugepages...............[Failed]
#  ls: cannot access /HugePage_vPFE/libvirt: No such file or directory
#  Error! Try restarting libvirt
#

if [ ! -f "$VMX_DIR/scripts/kvm/common/vmx_kvm_system_setup.sh" ]; then
    cp $VMX_DIR/scripts/kvm/common/vmx_kvm_system_setup.sh $VMX_DIR/scripts/kvm/common/vmx_kvm_system_setup.sh.orig
fi
$SED -i -e "s/Sleep 2 secs/Sleep 30 secs/g" $VMX_DIR/scripts/kvm/common/vmx_kvm_system_setup.sh
$SED -i -e "s/sleep 2/sleep 30/g" $VMX_DIR/scripts/kvm/common/vmx_kvm_system_setup.sh

diff $VMX_DIR/scripts/kvm/common/vmx_kvm_system_setup.sh.orig $VMX_DIR/scripts/kvm/common/vmx_kvm_system_setup.sh


#
# Needed for VMXZT on MX86 server
#

if [ ! -f "$VMX_DIR/scripts/templates/_vRE-ref.xml.orig" ]; then
    cp $VMX_DIR/scripts/templates/_vRE-ref.xml $VMX_DIR/scripts/templates/_vRE-ref.xml.orig
fi
$SED -i -e "s/161/33/g" $VMX_DIR/scripts/templates/_vRE-ref.xml
diff $VMX_DIR/scripts/templates/_vRE-ref.xml.orig $VMX_DIR/scripts/templates/_vRE-ref.xml

if [ ! -f "$VMX_DIR/scripts/templates/_vPFE-ref.xml.orig" ]; then
    cp $VMX_DIR/scripts/templates/_vPFE-ref.xml $VMX_DIR/scripts/templates/_vPFE-ref.xml.orig
    #$SED -i '/p4c_src\/p4c.bmv2.cfg/a \ \ p4c_src/p4c.afi.cfg' $VMX_DIR/scripts/templates/_vPFE-ref.xml 
fi
cp -f $CFG_FILES_DIR/_vPFE-ref.xml    $VMX_DIR/scripts/templates/
diff $VMX_DIR/scripts/templates/_vPFE-ref.xml.orig $VMX_DIR/scripts/templates/_vPFE-ref.xml

if [ ! -f "$VMX_DIR/scripts/common/vmx_configure.py.orig" ]; then
    cp $VMX_DIR/scripts/common/vmx_configure.py $VMX_DIR/scripts/common/vmx_configure.py.orig
fi
$SED -i -e "s/161/33/g" $VMX_DIR/scripts/common/vmx_configure.py
diff $VMX_DIR/scripts/common/vmx_configure.py.orig $VMX_DIR/scripts/common/vmx_configure.py


service libvirt-bin start
service libvirt-bin status
#
# Start VMX
#
# Make sure default network is active
default_network_status=`virsh net-list --all|grep default |awk '{print $2}'`
if [ "$default_network_status" == "inactive" ]
then
    log_debug "Making default network active"
    ifconfig virbr0 down
    brctl delbr virbr0
fi

log_prominent "Cleanup previous VMX run, if any, ..." 
$SCRIPTS_DIR/stop_vmx.sh

cd $VMX_DIR

log_prominent "Will start VMX now..." 
./vmx.sh -lv --install
if [ $? -ne 0 ]; then
   log_error "VMX did not start successfully" 
  ./vmx.sh --stop
   log_prominent "VMX did not start successfully. Please resolve issue and try again" 
   echo_status_and_exit 1
fi

#
# Check if VMX is running
#
./vmx.sh --status

#
# Check if VMX started successfully
#
vmx_status=`./vmx.sh --status | grep "Check if vMX is running" | grep No | wc -l`
if [ "$vmx_status" == "0" ]; then
    ./vmx.sh --bind-dev
    if [ $? -ne 0 ]; then
       log_error "vmx.sh --bind-dev not successful" 
      ./vmx.sh --stop
       echo_status_and_exit 1
    fi

    ./vmx.sh --bind-check

    #
    # Show Linux bridges
    #
    $BRCTL show
 
    log_prominent "!!!!VMX started!!!!"
 
    log_prominent "VMX links configuration"
    run_command $NEW_SCRIPTS_DIR/config_vmx_links.sh

    log_prominent "Waiting for prompt"
    run_command $SCRIPTS_DIR/check_vmx.exp

    log_prominent "Config VFP ext interface"
    run_command $NEW_SCRIPTS_DIR/config_vfp_ext_if.exp
 
    log_prominent "Configure br-int-vmx1 IP address"
    run_command $SCRIPTS_DIR/config_br-int-vmx1.sh
 
    log_prominent "VMX Setup Complete!"

    echo_status_and_exit 0
else 
    log_prominent "VMX did not start. Please resolve the issue"
    echo_status_and_exit 1
fi

#Normal script exit
echo_status_and_exit 0
