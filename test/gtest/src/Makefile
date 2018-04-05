#
# Makefile.inc -- Makefile to build test 
#
# JP4Agent GTESTs
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

CXX = g++
RM = rm -rf
PROG = jp4agent-gtest
OBJDIR  = ../obj
PROTOS = ../../../src/pi/protos
# Points to the root of Google Test
GTEST_DIR = /root/downloads/googletest-release-1.8.0/googletest
CONTROLLER_DIR = ../../controller

CXXFLAGS += \
	-L$(GTEST_DIR)/make \
	-L$(CONTROLLER_DIR)/obj \
	-std=c++14 \
	-Wall \

ifdef DEBUG_BUILD
	CXXFLAGS += -g -O0
endif

SRCS = \
	GTest.cpp \
	TestUtils.cpp \
	TestPacket.cpp \
	TapIf.cpp

OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))

#TESTS = sample1_unittest

CPPFLAGS += \
	-isystem \
	-I. \
	-I../include \
	-I../../../src/utils/include \
	-I$(GTEST_DIR)/include \
	-I$(CONTROLLER_DIR)/include \
	-I$(PROTOS)

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

LDFLAGS += \
	-L/usr/local/lib \
	-L../../../src/pi/protos \
	-L../../../src/utils/obj

LDLIBS = \
	-lcontroller \
	-lpi_proto \
	-lgrpc++ \
	-lprotoc \
	-lprotobuf \
	-lgrpc++ \
	-lgrpc++_unsecure \
	-lgrpc_unsecure \
	-lboost_system \
	-lboost_filesystem \
	-lpthread \
	-lpcap \
	-lnet \
	-lutils \
	-ljaegertracing \
	-lyaml-cpp


ifdef CODE_COVERAGE
	LDLIBS += -fprofile-arcs -ftest-coverage -lgcov
endif

LIBS = gtest.a 

all: $(OBJDIR)/$(PROG)
	@echo $(PROG) has been compiled

$(OBJDIR)/$(PROG): $(OBJS) $(OBJDIR)/gtest.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o : %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

#    -static $(LIBS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
$(OBJDIR)/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c -o $@\
            $(GTEST_DIR)/src/gtest-all.cc

$(OBJDIR)/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

$(OBJDIR)/gtest.a : $(OBJDIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(OBJDIR)/gtest_main.a : $(OBJDIR)/gtest-all.o $(OBJDIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

clean:
	$(RM) $(OBJDIR) ./.depend

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend
