#
# Dockerfile_p4
# Builds docker container to build/run Juniper P4 Agent
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

FROM ubuntu:16.04
LABEL maintainer "Sandesh Kumar Sodhi"

#
# Build
#
# [sudo] docker build -f Dockerfile_p4 -t juniper-p4 .
#
# Run:
#
# [sudo] docker run --name jnprp4 --privileged -i -t juniper-p4 /bin/bash
#
#
#

ARG DEBIAN_FRONTEND=noninteractive

# 
# Note:
# While adding new package to the list, maintain the alphanumeric sorted order.
#
RUN apt-get update && apt-get install -y --no-install-recommends \
 autoconf \
 automake \
 bsdmainutils \
 build-essential \
 ca-certificates \
 curl \
 ethtool \
 expect \
 g++ \
 gdb \
 git \
 gdebi-core \
 iputils-ping \
 less \
 libboost-all-dev \
 libevent-pthreads-2.0-5 \
 libjsoncpp-dev \
 libnet1-dev \
 libpcap-dev \
 libssl-dev \
 libtool \
 libxml2-utils \
 libyaml-cpp-dev \
 nano \
 net-tools \
 nmap \
 openssh-client \
 openssl \
 pkg-config \
 psmisc \
 python-lxml \
 realpath \
 sed \
 software-properties-common \
 ssh \
 sshpass \
 tcpdump \
 telnet \
 tmux \
 tdom \
 tree \
 unzip \
 vim \
 wget


# Clang related packages
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN apt-add-repository -y "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-6.0 main"
RUN apt-get update && apt-get install -y --no-install-recommends clang-6.0 clang-format-6.0 clang-tidy-6.0
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-6.0 10
RUN update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-6.0 10
RUN update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-6.0 10
RUN update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-6.0 10

#
# Packages needed for p4c
#
RUN apt-get update && apt-get install -y --no-install-recommends \
 bison \
 cmake \
 flex \
 libgc-dev \
 libgmp-dev \
 libfl-dev

RUN apt-get update && apt-get install -y --no-install-recommends \
 python \
 pylint \
 python-pip \
 python3-pip \
 python-scapy

RUN pip install --upgrade pip
RUN pip install -U setuptools
RUN pip install scapy
RUN pip install ipaddr
RUN pip install gcovr
RUN pip install lxml
RUN pip3 install grpcio-tools
RUN pip3 install prompt-toolkit

RUN apt-get update && apt-get install -y --no-install-recommends \
 curl \
 git

RUN mkdir -p /root/downloads
RUN cd /root/downloads && git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
RUN cd /root/downloads/grpc && git fetch && git checkout v1.8.1
RUN cd /root/downloads/grpc && git submodule update --init
RUN cd /root/downloads/grpc && make
RUN cd /root/downloads/grpc && make install
RUN cd /root/downloads/grpc/third_party/protobuf/ && ./autogen.sh && ./configure && make && make install

#
# GTEST
#
RUN cd /root/downloads && wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz
RUN cd /root/downloads && tar xzf release-1.8.0.tar.gz

#
# pyang
#
RUN cd /root/downloads && git clone https://github.com/mbj4668/pyang.git
RUN cd /root/downloads/pyang && python setup.py install

#
# GO
#
RUN cd /root/downloads && wget https://redirector.gvt1.com/edgedl/go/go1.9.2.linux-amd64.tar.gz
RUN cd /root/downloads && tar -C /usr/local -xzf go1.9.2.linux-amd64.tar.gz
RUN mkdir -p /root/go
ENV PATH=$PATH:/usr/local/go/bin
ENV GOPATH=/root/go

#
# ygot
#
RUN cd /root/go && go get github.com/golang/protobuf/proto
RUN cd /root/go && go get github.com/openconfig/gnmi; exit 0
RUN cd /root/go && go get github.com/openconfig/goyang
RUN cd /root/go && go get google.golang.org/grpc
RUN cd /root/go && go get github.com/golang/glog
RUN cd /root/go && go get github.com/kylelemons/godebug; exit 0
RUN cd /root/go/src && go get github.com/openconfig/ygot; exit 0
RUN cd /root/go/src/github.com/openconfig/ygot/proto_generator && go build

#
# Opentracing
#
RUN cd /root/downloads && git clone https://github.com/opentracing/opentracing-cpp
RUN cd /root/downloads/opentracing-cpp && mkdir .build
RUN cd /root/downloads/opentracing-cpp/.build && cmake ..
RUN cd /root/downloads/opentracing-cpp/.build && make
RUN cd /root/downloads/opentracing-cpp/.build && make install

#
# Jaeger
#

# Install thrift first. Note Jaeger works only with Thrift 0.9.3
# https://github.com/jaegertracing/jaeger-client-cpp/issues/45
#
RUN cd /root/downloads && git clone https://github.com/apache/thrift
RUN cd /root/downloads/thrift && git checkout 0.9.3
RUN cd /root/downloads/thrift && ./bootstrap.sh
RUN cd /root/downloads/thrift && ./configure --with-cpp --with-java=no --with-python=no \
--with-lua=no --with-perl=no --enable-shared=yes --enable-static=no --enable-tutorial=no --with-qt4=no
RUN cd /root/downloads/thrift && make -s
RUN cd /root/downloads/thrift && make install

# Now install cpp client for Jaeger

RUN cd /root/downloads && git clone https://github.com/jaegertracing/cpp-client
RUN cd /root/downloads/cpp-client && git checkout v0.3.0
RUN cd /root/downloads/cpp-client && mkdir .build
RUN cd /root/downloads/cpp-client/.build && cmake ..
RUN cd /root/downloads/cpp-client/.build && make
RUN cd /root/downloads/cpp-client/.build && make install

COPY env/bash_aliases /root/.bash_aliases
COPY env/vimrc        /root/.vimrc
COPY env/tmux.conf    /root/.tmux.conf

COPY entrypoint.sh /root/entrypoint.sh

ENTRYPOINT ["/root/entrypoint.sh"]

