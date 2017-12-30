#
# Makefile.inc -- Makefile to build Juniper P4 Agent
#
# JP4Agent : Juniper P4 Agent
#
# Created by Sandesh Kumar Sodhi, November 2017
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

AFI_LIB = ../lib/afi
AFI_INCLUDE = ../lib/afi/include

CXX = g++
PROG = jp4agent

SRCS = Main.cpp 
OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))

CXXFLAGS += -g -O0 -std=c++14 

CPPFLAGS += \
	-I. \
	-I../include/ \
	-I../protos/ \
	-I../protos/p4/tmp/ \
	-I../AFI/protos \
	-I../AFI/protos/juniper/ \
	-I/usr/include/jsoncpp \
	-I$(AFI_INCLUDE)/afi-transport \
	-I$(AFI_INCLUDE)/aft-client

AFI_LDLIBS = \
	-lafi \
	-lworkqueue

LDLIBS = -lprotobuf \
		 -lgrpc++ \
		 -lprotoc \
		 -lprotobuf \
		 -lgrpc++ \
		 -lgrpc++_unsecure \
		 -lgrpc_unsecure \
		 -lboost_system \
		 -lpthread \
		 -ljsoncpp

LDFLAGS += -L/usr/local/lib `pkg-config --libs grpc++ grpc`       \
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
           -lprotobuf -lpthread -ldl $(LDLIBS) $(AFI_LDLIBS)

all: $(PROG)
	@echo $(PROG) compilation success!

PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ../protos

vpath %.proto $(PROTOS_PATH)

SRCS = \
	AfiClient.cpp \
	AftPacket.cpp \
	base64.cpp \
	JP4Agent.cpp \
	Main.cpp \
	P4Info.cpp \
	AfiObjects.cpp \
	../protos/p4runtime.pb.cc \
	../protos/p4runtime.grpc.pb.cc \
	../protos/p4/config/p4info.pb.cc \
	../protos/p4/config/p4info.grpc.pb.cc \
	../protos/p4/tmp/p4config.pb.cc \
	../protos/p4/tmp/p4config.grpc.pb.cc \
	../protos/google/rpc/status.pb.cc \
	uint128.cpp \
	Utils.cpp \
	../AFI/protos/juniper/enums/enums.pb.cc \
	../AFI/protos/juniper/afi_tree/afi_tree.pb.cc \
	../AFI/protos/yext/yext.pb.cc \
	../AFI/protos/ywrapper/ywrapper.pb.cc


OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))

$(PROG): $(OBJS)
	LIBRARY_PATH=$(AFI_LIB) $(CXX) $^ $(LDFLAGS) -o $@

../include/google/rpc/status.pb.cc: ../protos/google/rpc/status.proto
	$(PROTOC) --proto_path=../protos/google/rpc -I $(PROTOS_PATH) --proto_path=. --cpp_out=../include/google/rpc/ $<

p4info.pb.cc: ../protos/p4/config/p4info.proto
	$(PROTOC) --proto_path=../protos/p4/config -I $(PROTOS_PATH) --proto_path=. --cpp_out=. $<

../protos/p4runtime.grpc.pb.cc: ../protos/p4runtime.proto
	$(PROTOC) --proto_path=../protos/p4/config -I $(PROTOS_PATH) --proto_path=. --grpc_out=../protos/ --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
	sleep 2
	@echo _________ Correcting strings _____
	sed -i -e 's/protobuf_p4_2fconfig_2fp4info_2eproto/protobuf_p4info_2eproto/g' ../protos/p4runtime.pb.cc
	sed -i -e 's/protobuf_google_2frpc_2fstatus_2eproto/protobuf_status_2eproto/g' ../protos/p4runtime.pb.cc

../protos/p4runtime.pb.cc: ../protos/p4runtime.proto
	$(PROTOC) --proto_path=../protos/p4/config -I $(PROTOS_PATH) --proto_path=. --cpp_out=../protos/ $<

../protos/p4/tmp/p4config.pb.cc: ../protos/p4/tmp/p4config.proto
	$(PROTOC) --proto_path=../protos/p4/tmp --cpp_out=../protos/p4/tmp/ $<

../protos/p4/tmp/p4config.grpc.pb.cc: ../protos/p4/tmp/p4config.proto
	$(PROTOC) --proto_path=../protos/p4/tmp -I $(PROTOS_PATH) --proto_path=. --grpc_out=../protos/p4/tmp --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

../protos/p4/config/p4info.pb.cc: ../protos/p4/config/p4info.proto
	$(PROTOC) --proto_path=../protos/p4/config --cpp_out=../protos/p4/config/ $<

../protos/p4/config/p4info.grpc.pb.cc: ../protos/p4/config/p4info.proto
	$(PROTOC) --proto_path=../protos/p4/config -I $(PROTOS_PATH) --proto_path=. --grpc_out=../protos/p4/config --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

../protos/google/rpc/code.pb.cc: ../protos/google/rpc/code.proto
	$(PROTOC) --proto_path=../protos/google/rpc/ --cpp_out=../protos/google/rpc/ $<

../protos/google/rpc/status.pb.cc: ../protos/google/rpc/status.proto
	$(PROTOC) --proto_path=../protos/google/rpc/ --cpp_out=../protos/google/rpc/ $<

clean:
	rm -f $(OBJS) $(PROG) ./.depend

depend: .depend

.depend: $(SRCS) ../protos/google/rpc/code.pb.cc
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend
