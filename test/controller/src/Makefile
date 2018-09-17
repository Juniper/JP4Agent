#
# Makefile.inc -- Makefile to test controller
#
# JP4Agent Test Controller
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

ifdef UBUNTU
CXX = g++
CPPFLAGS += -DUBUNTU
endif

RM = rm -rf
PROG = controller
LIB  = libcontroller.a
PROTOS = ../../../src/pi/protos
OBJDIR  = ../obj

CXXFLAGS += -std=c++14 -Wall -Werror -Wl,--no-undefined

ifdef DEBUG_BUILD
	CXXFLAGS += -g -O0
endif

SRCS = \
	Controller.cpp \
	Main.cpp \
	P4InfoUtils.cpp

OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))

CPPFLAGS += \
	-isystem \
	-I. \
	-I../include \
	-I$(PROTOS)

LDLIBS = \
	-lpi_proto \
	-lprotobuf \
	-lgrpc++ \
	-lprotoc \
	-lprotobuf \
	-lgrpc++ \
	-lgrpc++_unsecure \
	-lgrpc_unsecure \
	-lboost_system \
	-lpthread \
	-lboost_thread

ifdef CODE_COVERAGE
	LDLIBS += -fprofile-arcs -ftest-coverage -lgcov
endif

LDFLAGS += \
	-L$(PROTOS)

all: $(OBJDIR)/$(LIB) $(OBJDIR)/$(PROG)
	@echo $(PROG) has been compiled

$(OBJDIR)/$(LIB): $(OBJS)
	ar rcs $(OBJDIR)/$(LIB) $(OBJS)

$(OBJDIR)/$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o : %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJDIR) ./.depend

install:
	@echo Nothing to install!

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend
