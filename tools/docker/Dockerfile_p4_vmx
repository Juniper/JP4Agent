#
# Dockerfile_p4_vmx
# Build docker container where VMX can also be run along with Juniper P4 Agent
#
# JP4Agent : Juniper P4 Agent
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

FROM juniper-p4
LABEL maintainer "Sandesh Kumar Sodhi"

#
# Build
#  [sudo] docker build -f Dockerfile_p4_vmx -t juniper-p4-vmx .
#
# Run:
#  [sudo] docker run --name jnprp4vmx --privileged -i -t juniper-p4-vmx /bin/bash
#

ARG DEBIAN_FRONTEND=noninteractive

#
# Packages needed for VMX
#
RUN apt-get update && apt-get install -y --no-install-recommends \
 apparmor \
 apparmor-utils \
 apparmor-profiles \
 qemu-kvm \
 libvirt-bin \
 python \
 python-netifaces \
 vnc4server \
 libyaml-dev \
 python-yaml \
 numactl \
 libparted0-dev \
 libpciaccess-dev \
 libnuma-dev \
 libyajl-dev \
 libxml2-dev \
 libglib2.0-dev \
 libnl-3-dev \
 python-pip \
 python-dev \
 libxml2-dev \
 libxslt-dev \
 rsyslog \
 net-tools \
 iproute2 \
 virtinst \
 tshark

#
# Openconfig/NetConf related packages
#
RUN cd /root/go && go get github.com/Juniper/go-netconf/netconf
RUN cd /root/go && go get github.com/spf13/pflag
RUN cd /root/go && go get golang.org/x/sys/unix


COPY entrypoint.sh /root/entrypoint.sh

ENTRYPOINT ["/root/entrypoint.sh"]
