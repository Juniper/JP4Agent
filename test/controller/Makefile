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

CXX = g++
PROG = controller
LIB  = libcontroller.a
PROTOS = ../../protos

SRCS = Controller.cpp \
	Utils.cpp \
	$(PROTOS)/p4runtime.pb.cc \
	$(PROTOS)/p4runtime.grpc.pb.cc \
	$(PROTOS)/p4/config/p4info.pb.cc \
	$(PROTOS)/p4/tmp/p4config.pb.cc \
	$(PROTOS)/google/rpc/status.pb.cc

OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))

CXXFLAGS += -g -O0 \
            -std=c++11

CPPFLAGS += -isystem \
            -I. \
            -I../../src/ \
            -I$(PROTOS) \
            -Wall

LDLIBS = -lprotobuf \
         -lgrpc++ \
         -lprotoc \
         -lprotobuf \
         -lgrpc++ \
         -lgrpc++_unsecure \
         -lgrpc_unsecure \
         -lboost_system \
         -lpthread \
         -lboost_thread

all: $(LIB) $(PROG)
	@echo $(PROG) has been compiled

$(LIB): $(OBJS)
	ar rcs $(LIB) $(OBJS)

$(PROG): $(OBJS) Main.o
	LIBRARY_PATH=.. \
    $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(PROG) $(OBJS) Main.o $(LDLIBS) -pthread

clean:
	rm -f *.a *.o ../*.o $(PROG) ./.depend

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;

include .depend
