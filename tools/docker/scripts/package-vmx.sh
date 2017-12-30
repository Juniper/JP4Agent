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

echo Packaging VMX ...
VFPC_IMG=vFPC-20170524.img
METADATA_USB_ZT_IMG=metadata_usb_zt.img
VMXHDD_IMG=vmxhdd.img

VMX_BUNDLE=`ls -1 | grep vmx | grep bundle | grep tgz`
JUNOS_VMDK=`ls -1 | grep junos | grep vmdk`
JUNOS_OPENCONFIG_PKG=`ls -1 | grep "junos-openconfig" | grep tgz`

if [ -z "$VMX_BUNDLE" ]; then
    echo "No junos VMX bundle found in current folder"
    exit 1
fi

if [ -z "$JUNOS_VMDK" ]; then
    echo "No junos vmdk found in current folder"
    exit 1
fi

if [ -z "$JUNOS_OPENCONFIG_PKG" ]; then
    echo "No junos openconfig package found in current folder"
    exit 1
fi

JUNOS_QCOW2=${JUNOS_VMDK::-5}.qcow2

if [ ! -f "$VFPC_IMG" ]; then
    echo $VFPC_IMG not found in current folder. Copy it to current folder.
    exit 1
fi

if [ ! -f "$METADATA_USB_ZT_IMG" ]; then
    echo $METADATA_USB_ZT_IMG not found in current folder. Copy it to current folder.
    exit 1
fi

if [ ! -f "$VMXHDD_IMG" ]; then
    echo $VMXHDD_IMG not found in current folder. Copy it to current folder.
    exit 1
fi

echo PACKAGE_SRC_LOCATION=$PACKAGE_SRC_LOCATION
echo VMX_BUNDLE=$VMX_BUNDLE
echo JUNOS_VMDK=$JUNOS_VMDK
echo JUNOS_QCOW2=$JUNOS_QCOW2
echo VFPC_IMG=$VFPC_IMG
echo METADATA_USB_ZT_IMG=$METADATA_USB_ZT_IMG
echo VMXHDD_IMG=$VMXHDD_IMG
echo JUNOS_OPENCONFIG_PKG=$JUNOS_OPENCONFIG_PKG

echo "Do you want to Proceed? (answer yes to proceed)"
read -t 10 -n 3 answer
if [ $? == 0 ]; then
    echo "Your answer is: $answer"
    if [ $answer == "yes" ]; then
        echo "Your answer is: $answer. Proceeding with packaging"
    else
        echo "Answer is not 'yes'! Exiting..."
        exit 1
    fi  
else
    echo "Can't wait anymore!"
    exit 1
fi  

if [ ! -f "$JUNOS_QCOW2" ]; then
    echo Converting junos vmdk to qcow2...
    qemu-img convert -O qcow2 $JUNOS_VMDK $JUNOS_QCOW2
else
    echo Junos qcow2 is present. Skipping converting junos vmdk to qcow2
fi

if [ ! -f "jnprP4vmx.tgz" ]; then
    tar cvzf jnprP4vmx.tgz $VMX_BUNDLE $JUNOS_QCOW2 $VFPC_IMG $METADATA_USB_ZT_IMG $VMXHDD_IMG $JUNOS_OPENCONFIG_PKG
else 
    echo jnprP4vmx.tgz is already present.
fi
