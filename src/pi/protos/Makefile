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

VERSION = 0

ifdef UBUNTU
CXX = g++
CPPFLAGS += -DUBUNTU
endif

TARGET_LIB = libpi_proto.so.$(VERSION)
RM = rm -f

CXXFLAGS += -fPIC -g -O0 -std=c++14 

CPPFLAGS += \
	-I. \
	-Ip4/config \
	-Ip4/tmp \
	-Igoogle/rpc \
	-Icli

LDFLAGS = -shared

all: $(TARGET_LIB)
	@echo $(TARGET_LIB) compilation success!

PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = .

vpath %.proto $(PROTOS_PATH)

ifdef UBUNTU
SRCS = \
	p4/config/p4info.pb.cc \
	p4/config/p4info.grpc.pb.cc \
	p4/tmp/p4config.pb.cc \
	p4/tmp/p4config.grpc.pb.cc \
	google/rpc/status.pb.cc \
	google/rpc/code.pb.cc \
	p4runtime.pb.cc \
	p4runtime.grpc.pb.cc \
	cli/jp4cli.pb.cc \
	cli/jp4cli.grpc.pb.cc
else
SRCS = \
	p4info.pb.cc \
	p4info.grpc.pb.cc \
	p4/tmp/p4config.pb.cc \
	p4/tmp/p4config.grpc.pb.cc \
	status.pb.cc \
	google/rpc/code.pb.cc \
	p4runtime_wrl.pb.cc \
	p4runtime_wrl.grpc.pb.cc \
	cli/jp4cli.pb.cc \
	cli/jp4cli.grpc.pb.cc
endif

PROTO_GENERATED_HEADERS=$(subst .cc,.h, $(SRCS))
OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))

$(TARGET_LIB): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^
	rm -f ${basename $@}
	ln -s ${notdir $@} ${basename $@}

ifdef UBUNTU
p4runtime.pb.cc: p4runtime.proto
	$(PROTOC) --cpp_out=. $<
	sleep 2
	@echo Correcting strings ...
	sed -i -e 's/_p4_2fconfig_2fp4info_2eproto/_p4info_2eproto/g' p4runtime.pb.cc
	sed -i -e 's/_google_2frpc_2fstatus_2eproto/_status_2eproto/g' p4runtime.pb.cc

p4runtime.grpc.pb.cc: p4runtime.proto
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

else

p4runtime_wrl.pb.cc: p4runtime_wrl.proto
	$(PROTOC) --cpp_out=. $<
	sleep 2
	@echo Correcting strings ...
	sed -i -e 's/_p4_2fconfig_2fp4info_2eproto/_p4info_2eproto/g' p4runtime_wrl.pb.cc
	sed -i -e 's/_google_2frpc_2fstatus_2eproto/_status_2eproto/g' p4runtime_wrl.pb.cc

p4runtime_wrl.grpc.pb.cc: p4runtime_wrl.proto
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
endif

p4/tmp/p4config.pb.cc: p4/tmp/p4config.proto
	$(PROTOC) --proto_path=p4/tmp --cpp_out=p4/tmp/ $<

p4/tmp/p4config.grpc.pb.cc: p4/tmp/p4config.proto
	$(PROTOC) --proto_path=p4/tmp --grpc_out=p4/tmp --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

ifdef UBUNTU
p4/config/p4info.pb.cc: p4/config/p4info.proto
	$(PROTOC) --proto_path=p4/config --cpp_out=p4/config/ $<

p4/config/p4info.grpc.pb.cc: p4/config/p4info.proto
	$(PROTOC) --proto_path=p4/config --grpc_out=p4/config --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
else
p4info.pb.cc: p4info.proto
	$(PROTOC) --cpp_out=. $<

p4info.grpc.pb.cc: p4info.proto
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
endif

google/rpc/code.pb.cc: google/rpc/code.proto
	$(PROTOC) --proto_path=google/rpc --cpp_out=google/rpc/ $<

ifdef UBUNTU
google/rpc/status.pb.cc: google/rpc/status.proto
	$(PROTOC) --proto_path=google/rpc --cpp_out=google/rpc/ $<
else
status.pb.cc: status.proto
	$(PROTOC) --cpp_out=. $<
endif

cli/jp4cli.pb.cc: cli/jp4cli.proto
	$(PROTOC) --cpp_out=. $<

cli/jp4cli.grpc.pb.cc: cli/jp4cli.proto
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

clean:
	$(RM) $(OBJS) $(TARGET_LIB) $(SRCS) $(PROTO_GENERATED_HEADERS) ./.depend

ifeq ($(prefix),)
    prefix := /usr/local
endif

.PHONY: install
install: $(TARGET_LIB)
	install -d $(DESTDIR)$(prefix)/lib64/
	install -m 644 $(wildcard *.so.*) $(DESTDIR)$(prefix)/lib64/
	rm -f $(DESTDIR)$(prefix)/lib64/${basename $(TARGET_LIB)}
	ln -s $(DESTDIR)$(prefix)/lib64/${notdir $(TARGET_LIB)} $(DESTDIR)$(prefix)/lib64/${basename $(TARGET_LIB)}
	install -d $(DESTDIR)$(prefix)/include/jp4agent
	install -d $(DESTDIR)$(prefix)/include/jp4agent/src
	install -d $(DESTDIR)$(prefix)/include/jp4agent/src/pi
	install -d $(DESTDIR)$(prefix)/include/jp4agent/src/pi/protos
	cp --parents `find -name \*.h` $(DESTDIR)$(prefix)/include/jp4agent/src/pi/protos

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend
