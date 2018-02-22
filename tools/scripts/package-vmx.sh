#!/bin/bash
#
# Package VMX
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

HOST_VMX_LOC=$(xmllint --xpath "//vmxConfig/hostVMXLocation/text()" $VMX_CONFIG_FILE)

JNPR_P4_VMX_PKG=jnprP4vmx.tgz
JNPR_P4_VMX_PKG_MD5=${JNPR_P4_VMX_PKG}.md5
JNPR_P4_VMX_PKG_SHA1=${JNPR_P4_VMX_PKG}.sha1

cd $HOST_VMX_LOC
echo Packaging...

VFPC_IMG=vFPC-20170524.img
METADATA_USB_ZT_IMG=metadata_usb_zt.img
METADATA_USB_RE_IMG=metadata-usb-re.img
VMXHDD_IMG=vmxhdd.img
AFT_PKG=aft.tgz
AFT_PKG_SHA1=aft.tgz.sha1
AFI_LIB_AND_INCLUDES=afi

VMX_BUNDLE=`ls -1 | grep vmx | grep bundle | grep tgz`
JUNOS_VMDK=`ls -1 | grep junos | grep vmdk`
JUNOS_OPENCONFIG_PKG=`ls -1 | grep "junos-openconfig" | grep tgz`

if [ -z "$VMX_BUNDLE" ]; then
    echo "No junos VMX bundle found in $HOST_VMX_LOC"
    exit 1
fi

if [ -z "$JUNOS_VMDK" ]; then
    echo "No junos vmdk found in $HOST_VMX_LOC"
    exit 1
fi

if [ -z "$JUNOS_OPENCONFIG_PKG" ]; then
    echo "No junos openconfig package found in $HOST_VMX_LOC"
    exit 1
fi

JUNOS_QCOW2=${JUNOS_VMDK::-5}.qcow2

if [ ! -f "$VFPC_IMG" ]; then
    echo $VFPC_IMG not found in $HOST_VMX_LOC. Copy it to $HOST_VMX_LOC.
    exit 1
fi

if [ ! -f "$METADATA_USB_RE_IMG" ]; then
    echo $METADATA_USB_RE_IMG not found in $HOST_VMX_LOC. Copy it to $HOST_VMX_LOC.
    exit 1
fi

if [ ! -f "$METADATA_USB_ZT_IMG" ]; then
    echo $METADATA_USB_ZT_IMG not found in $HOST_VMX_LOC. Copy it to $HOST_VMX_LOC.
    exit 1
fi

if [ ! -f "$VMXHDD_IMG" ]; then
    echo $VMXHDD_IMG not found in $HOST_VMX_LOC. Copy it to $HOST_VMX_LOC.
    exit 1
fi

if [ ! -f "$AFT_PKG" ]; then
    echo $AFT_PKG not found in $HOST_VMX_LOC. Copy it to $HOST_VMX_LOC.
    exit 1
fi

if [ ! -f "$AFT_PKG_SHA1" ]; then
    echo $AFT_PKG_SHA1 not found in $HOST_VMX_LOC. Copy it to $HOST_VMX_LOC.
    exit 1
fi

if [ ! -d "$AFI_LIB_AND_INCLUDES" ]; then
    echo "Error: Dir '$AFI_LIB_AND_INCLUDES' does not exist"
    exit 1
fi

echo PACKAGE_SRC_LOCATION=$PACKAGE_SRC_LOCATION
echo VMX_BUNDLE=$VMX_BUNDLE
echo JUNOS_VMDK=$JUNOS_VMDK
echo JUNOS_QCOW2=$JUNOS_QCOW2
echo VFPC_IMG=$VFPC_IMG
echo METADATA_USB_RE_IMG=$METADATA_USB_RE_IMG
echo METADATA_USB_ZT_IMG=$METADATA_USB_ZT_IMG
echo VMXHDD_IMG=$VMXHDD_IMG
echo JUNOS_OPENCONFIG_PKG=$JUNOS_OPENCONFIG_PKG
echo AFI_LIB_AND_INCLUDES=$AFI_LIB_AND_INCLUDES

echo "Do you want to Proceed? (answer yes to proceed)"
read -t 10 -n 3 answer
if [ $? == 0 ]; then
    echo "Your answer is: $answer"
    if [ "dummy"$answer == "dummyyes" ]; then
        echo "Your answer is: $answer. Proceeding with packaging"
    else
        echo "Answer is not 'yes'! Exiting..."
        exit 1
    fi  
else
    echo "Can't wait anymore! Exiting..."
    exit 1
fi  

if [ ! -f "$JUNOS_QCOW2" ]; then
    echo Converting junos vmdk to qcow2...
    qemu-img convert -O qcow2 $JUNOS_VMDK $JUNOS_QCOW2
else
    echo Junos qcow2 is present. Skipping converting junos vmdk to qcow2
fi

if [ ! -f "$JNPR_P4_VMX_PKG" ]; then
    log_debug "Creating $JNPR_P4_VMX_PKG tarball..."    
    run_command tar cvzf $JNPR_P4_VMX_PKG \
        $VMX_BUNDLE \
        $JUNOS_VMDK \
        $JUNOS_QCOW2 \
        $VFPC_IMG \
        $METADATA_USB_ZT_IMG \
        $METADATA_USB_RE_IMG \
        $VMXHDD_IMG \
        $JUNOS_OPENCONFIG_PKG \
        $AFT_PKG $AFT_PKG_SHA1 \
        $AFI_LIB_AND_INCLUDES
else 
    echo $JNPR_P4_VMX_PKG is already present.
fi

log_debug "Calculating md5 checksum of $JNPR_P4_VMX_PKG..."    
md5sum $JNPR_P4_VMX_PKG > $JNPR_P4_VMX_PKG_MD5
cat $JNPR_P4_VMX_PKG_MD5

log_debug "Calculating sha1 checksum of $JNPR_P4_VMX_PKG..."    
sha1sum $JNPR_P4_VMX_PKG > $JNPR_P4_VMX_PKG_SHA1
cat $JNPR_P4_VMX_PKG_SHA1
