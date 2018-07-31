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

AFI_LIB = /root/VMX/afi/lib

ifdef UBUNTU
CXX = g++
CPPFLAGS += -DUBUNTU
endif

PROG = jp4agent
RM = rm -rf
OBJDIR  = ../obj
BINDIR  = ../bin

CXXFLAGS += -std=c++14 -Wall -Werror -Wl,--no-undefined

ifdef DEBUG_BUILD
	CXXFLAGS += -g -O0
endif

ifdef CODE_COVERAGE
	CXXFLAGS += -fprofile-arcs -ftest-coverage
endif

CPPFLAGS += \
	-I. \
	-I../include/ \
	-I../../../../AFI/protos \
	-I../../../../AFI/protos/juniper \
	-I../../../afi/include/ \
	-I../../../jp4agent/include/ \
	-I../../../pi/include/ \
	-I../../../pi/protos \
	-I../../../pi/protos/p4/config \
	-I../../../pi/protos/p4/tmp \
	-I../../../utils/include/

ifdef UBUNTU
CPPFLAGS += \
        -I/usr/include/jsoncpp
endif

AFI_LDLIBS = \
	-lafi \
	-lworkqueue

LDLIBS = \
	-ljp4agent \
	-laft_target_halp \
	-lafi_yang \
	-lafi_hal \
	-lpi \
	-lpi_proto \
	-lutils \
	-lgrpc++ \
	-lprotoc \
	-lprotobuf \
	-lgrpc++ \
	-lgrpc++_unsecure \
	-lgrpc_unsecure \
	-lboost_system \
	-lpthread \
	-ljsoncpp \
	-ljaegertracing \
	-lyaml-cpp

ifdef CODE_COVERAGE
	LDLIBS += -fprofile-arcs -ftest-coverage -lgcov
endif

LDFLAGS += \
	-L/usr/local/lib \
	-L../../../jp4agent/obj/ \
	-L../../../utils/obj/ \
	-L../../../pi/obj/ \
	-L../../../afi/obj/ \
	-L../../../pi/protos \
	-L../../../../AFI/ \
	-L../aft/obj/ \
	`pkg-config --libs grpc++ grpc`       \
	-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed \
	-lprotobuf -lpthread -ldl $(LDLIBS) $(AFI_LDLIBS)

all: $(BINDIR)/$(PROG)
	@echo $(PROG) compilation success!

SRCS = \
	Main.cpp

OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))

$(BINDIR)/$(PROG): $(OBJS)
	LIBRARY_PATH=$(AFI_LIB) $(CXX) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o : %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJDIR) $(BINDIR)/$(PROG) ./.depend

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend
