#
# Makefile.inc -- Makefile to build AFI Controller
#
# afi-controller : AFI controller
#
# Created by Sandesh Kumar Sodhi, December, 2017
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

PROG = afi-controller

ifdef UBUNTU
CXX = g++
endif
CXXFLAGS += -g -O0 -std=c++14 

CPPFLAGS += \
	-I. \
	-I../yang/AFI/protos/ \
	-I../yang/AFI/protos/juniper/ \
	-I/usr/include/jsoncpp


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
           -lprotobuf -lpthread -ldl $(LDLIBS)

all:    $(PROG)
	@echo $(PROG) compilation success!

PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ../protos

vpath %.proto $(PROTOS_PATH)

all: system-check $(PROG)

SRCS = \
	base64.cpp \
	main.cpp \
	../yang/AFI/protos/juniper/afi_tree/afi_tree.pb.cc \
	../yang/AFI/protos/yext/yext.pb.cc \
	../yang/AFI/protos/ywrapper/ywrapper.pb.cc


OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))

$(PROG): $(OBJS)
	LIBRARY_PATH=$(AFI_LIB) $(CXX) $^ $(LDFLAGS) -o $@


../yang/AFI/protos/yext/yext.pb.cc: ../yang/AFI/protos/yext/yext.proto
	$(PROTOC) --proto_path=../yang/AFI/protos/yext/ --cpp_out=../yang/AFI/protos/yext/ $<

../yang/AFI/protos/ywrapper/ywrapper.pb.cc: ../yang/AFI/protos/ywrapper/ywrapper.proto
	$(PROTOC) --proto_path=../yang/AFI/protos/ywrapper/ --cpp_out=../yang/AFI/protos/ywrapper/ $<

../yang/AFI/protos/juniper/afi_tree/afi_tree.pb.cc: ../yang/AFI/protos/juniper/afi_tree/afi_tree.proto
	$(PROTOC) --proto_path=../yang/AFI/protos/juniper/afi_tree/  -I $(AFI_PROTOS_PATH) --cpp_out=../yang/AFI/protos/juniper/afi_tree/ $<
	sleep 2
	@echo _________ Correcting strings _____
	find ../yang/AFI/protos -name '*.pb.*' -exec sed -i 's/protobuf_ywrapper_2fywrapper_2eproto/protobuf_ywrapper_2eproto/g' {} \;
	find ../yang/AFI/protos -name '*.pb.*' -exec sed -i 's/protobuf_yext_2fyext_2eproto/protobuf_yext_2eproto/g' {} \;



clean:
	rm -f $(OBJS) $(PROG) ./.depend

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend


# The following is to test your system and ensure a smoother experience.
# They are by no means necessary to actually compile a grpc-enabled software.

PROTOC_CMD = which $(PROTOC)
PROTOC_CHECK_CMD = $(PROTOC) --version | grep -q libprotoc.3
PLUGIN_CHECK_CMD = which $(GRPC_CPP_PLUGIN)
HAS_PROTOC = $(shell $(PROTOC_CMD) > /dev/null && echo true || echo false)
ifeq ($(HAS_PROTOC),true)
HAS_VALID_PROTOC = $(shell $(PROTOC_CHECK_CMD) 2> /dev/null && echo true || echo false)
endif
HAS_PLUGIN = $(shell $(PLUGIN_CHECK_CMD) > /dev/null && echo true || echo false)

SYSTEM_OK = false
ifeq ($(HAS_VALID_PROTOC),true)
ifeq ($(HAS_PLUGIN),true)
SYSTEM_OK = true
endif
endif

system-check:
ifneq ($(HAS_VALID_PROTOC),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have protoc 3.0.0 installed in your path."
	@echo "Please install Google protocol buffers 3.0.0 and its compiler."
	@echo "You can find it here:"
	@echo
	@echo "   https://github.com/google/protobuf/releases/tag/v3.0.0"
	@echo
	@echo "Here is what I get when trying to evaluate your version of protoc:"
	@echo
	-$(PROTOC) --version
	@echo
	@echo
endif
ifneq ($(HAS_PLUGIN),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have the grpc c++ protobuf plugin installed in your path."
	@echo "Please install grpc. You can find it here:"
	@echo
	@echo "   https://github.com/grpc/grpc"
	@echo
	@echo "Here is what I get when trying to detect if you have the plugin:"
	@echo
	-which $(GRPC_CPP_PLUGIN)
	@echo
	@echo
endif
ifneq ($(SYSTEM_OK),true)
	@false
endif
