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
PROG = jp4agent-gtest

# Points to the root of Google Test
GTEST_DIR = ../../../downloads/googletest-release-1.8.0/googletest
CONTROLLER_DIR = ../controller

SRCS = GTest.cpp \
	TestUtils.cpp \
	TestPacket.cpp \
	TapIf.cpp \
	../../src/Utils.cpp


OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))


#TESTS = sample1_unittest

CXXFLAGS += -L$(GTEST_DIR)/make \
            -L$(CONTROLLER_DIR) \
            -g -O0 \
            -std=c++11 \
            -Wall

CPPFLAGS += -isystem $(GTEST_DIR)/include \
            -I. \
            -I$(GTEST_DIR)/include \
            -I../../src/ \
            -I$(CONTROLLER_DIR)

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

LDLIBS = -lcontroller \
         -lgrpc++ \
         -lprotoc \
         -lprotobuf \
         -lgrpc++ \
         -lgrpc++_unsecure \
         -lgrpc_unsecure \
         -lboost_system \
         -lpthread \
         -lboost_thread \
         -lboost_chrono

LIBS = gtest.a 

all:    $(PROG)
	@echo $(PROG) has been compiled


$(PROG): $(OBJS) gtest.a
	LIBRARY_PATH=.. \
    $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(PROG) $(OBJS) $(LDLIBS) gtest.a -pthread

#    -static $(LIBS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -f *.a *.o ../*.o $(PROG) ./.depend

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend
