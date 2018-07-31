#
# Makefile.inc -- Makefile for Brcm target
#
# JP4Agent : Juniper P4 Agent
#
# Created by Sudheendra Gopinath, January 2018
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

ifdef UBUNTU
CXX = g++
endif
TARGET_LIB = libbrcm_target_halp.so
RM = rm -rf
OBJDIR  = ../obj

CXXFLAGS += -fPIC -std=c++14 -Wall -Werror

ifdef DEBUG_BUILD
	CXXFLAGS += -g -O0
endif

ifdef CODE_COVERAGE
	CXXFLAGS += -fprofile-arcs -ftest-coverage
endif

CPPFLAGS += \
	-I. \
	-I../include \
	-I../../../../include \
	-I../../../../afi/include \
	-I../../../../utils/include \
        -I../../../../pi/protos \
        -I../../../../pi/include \
	-I../../../../../AFI/protos \
	-I../../../../../AFI/protos/juniper

ifdef UBUNTU
CPPFLAGS += \
        -I/usr/include/jsoncpp
endif

ifdef CODE_COVERAGE
	LDLIBS += -fprofile-arcs -ftest-coverage -lgcov
endif

LDFLAGS = -shared $(LDLIBS)

all: $(OBJDIR)/$(TARGET_LIB)
	@echo $(TARGET_LIB) compilation success!

SRCS = \
	Brcm.cpp \
	BrcmDevice.cpp \
	BrcmTree.cpp \
	BrcmTreeEntry.cpp \
	BrcmCap.cpp \
	BrcmCapEntry.cpp

OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))

$(OBJDIR)/$(TARGET_LIB): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o : %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJDIR) ./.depend

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include ${BCM_HALP_DIR_PATH}/mk/Makefile.bcm.inc
include .depend
