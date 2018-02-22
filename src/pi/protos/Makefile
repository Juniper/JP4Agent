#
# Makefile.inc -- Makefile to build dynamic library for afi layer
#
# JP4Agent : Juniper P4 Agent
#
# Created by Sandesh Kumar Sodhi, January 2018
# Copyright (c) [2018] Juniper Networks, Inc. All rights reserved.
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

CXX = g++
TARGET_LIB = libpi_proto.so
RM = rm -f

CXXFLAGS += -fPIC -g -O0 -std=c++14 

CPPFLAGS += \
	-I. \
	-Ip4/config \
	-Ip4/tmp \
	-Igoogle/rpc \

LDFLAGS = -shared

all: $(TARGET_LIB)
	@echo $(TARGET_LIB) compilation success!

PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = .

vpath %.proto $(PROTOS_PATH)

SRCS = \
	p4/config/p4info.pb.cc \
	p4/config/p4info.grpc.pb.cc \
	p4/tmp/p4config.pb.cc \
	p4/tmp/p4config.grpc.pb.cc \
	google/rpc/status.pb.cc \
	google/rpc/code.pb.cc \
	p4runtime.pb.cc \
	p4runtime.grpc.pb.cc

PROTO_GENERATED_HEADERS=$(subst .cc,.h, $(SRCS))
OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))

$(TARGET_LIB): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

p4runtime.pb.cc: p4runtime.proto
	$(PROTOC) --cpp_out=. $<
	sleep 2
	@echo Correcting strings ...
	sed -i -e 's/protobuf_p4_2fconfig_2fp4info_2eproto/protobuf_p4info_2eproto/g' p4runtime.pb.cc
	sed -i -e 's/protobuf_google_2frpc_2fstatus_2eproto/protobuf_status_2eproto/g' p4runtime.pb.cc

p4runtime.grpc.pb.cc: p4runtime.proto
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

p4/tmp/p4config.pb.cc: p4/tmp/p4config.proto
	$(PROTOC) --proto_path=p4/tmp --cpp_out=p4/tmp/ $<

p4/tmp/p4config.grpc.pb.cc: p4/tmp/p4config.proto
	$(PROTOC) --proto_path=p4/tmp --grpc_out=p4/tmp --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

p4/config/p4info.pb.cc: p4/config/p4info.proto
	$(PROTOC) --proto_path=p4/config --cpp_out=p4/config/ $<

p4/config/p4info.grpc.pb.cc: p4/config/p4info.proto
	$(PROTOC) --proto_path=p4/config --grpc_out=p4/config --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

google/rpc/code.pb.cc: google/rpc/code.proto
	$(PROTOC) --proto_path=google/rpc --cpp_out=google/rpc/ $<

google/rpc/status.pb.cc: google/rpc/status.proto
	$(PROTOC) --proto_path=google/rpc --cpp_out=google/rpc/ $<

clean:
	$(RM) $(OBJS) $(TARGET_LIB) $(SRCS) $(PROTO_GENERATED_HEADERS) ./.depend

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend
