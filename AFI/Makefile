#
# Makefile.inc -- Makefile to generate protobuf for AFI yang model
#
# AFI yang model
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

VERSION = 0

PYANG=pyang
PROTOG=/root/go/src/github.com/openconfig/ygot/proto_generator/proto_generator

ifdef UBUNTU
CXX = g++
CPPFLAGS += -DUBUNTU
endif

TARGET_LIB = libafi_yang.so.$(VERSION)
RM = rm -f

CXXFLAGS += -fPIC -g -O0 -std=c++14 

CPPFLAGS += \
	-I. \
	-Iprotos \
	-IAFI/protos/juniper \

LDFLAGS = -shared

all: $(TARGET_LIB)
	@echo $(TARGET_LIB) compilation success!

ifdef UBUNTU
SRCS = \
	protos/juniper/enums/enums.pb.cc \
	protos/juniper/afi_tree/afi_tree.pb.cc \
	protos/juniper/afi_tree_entry/afi_tree_entry.pb.cc \
	protos/juniper/afi_encap/afi_encap.pb.cc \
	protos/juniper/afi_encap_entry/afi_encap_entry.pb.cc \
	protos/juniper/afi_tree_encap/afi_tree_encap.pb.cc \
	protos/juniper/afi_tree_encap_entry/afi_tree_encap_entry.pb.cc \
	protos/juniper/afi_cap/afi_cap.pb.cc \
	protos/juniper/afi_cap_match/afi_cap_match.pb.cc \
	protos/juniper/afi_cap_action/afi_cap_action.pb.cc \
	protos/juniper/afi_cap_entry/afi_cap_entry.pb.cc \
	protos/juniper/afi_cap_entry_match/afi_cap_entry_match.pb.cc \
	protos/juniper/afi_cap_entry_action/afi_cap_entry_action.pb.cc \
	protos/yext/yext.pb.cc \
	protos/ywrapper/ywrapper.pb.cc
else
SRCS = \
	protos/yext/yext.pb.cc \
	protos/ywrapper/ywrapper.pb.cc \
	protos/juniper/enums/enums.pb.cc \
	protos/juniper/afi_tree/afi_tree.pb.cc \
	protos/juniper/afi_tree_entry/afi_tree_entry.pb.cc \
	protos/juniper/afi_encap/afi_encap.pb.cc \
	protos/juniper/afi_encap_entry/afi_encap_entry.pb.cc \
	protos/juniper/afi_tree_encap/afi_tree_encap.pb.cc \
	protos/juniper/afi_tree_encap_entry/afi_tree_encap_entry.pb.cc \
	protos/juniper/afi_cap/afi_cap.pb.cc \
	protos/juniper/afi_cap_match/afi_cap_match.pb.cc \
	protos/juniper/afi_cap_action/afi_cap_action.pb.cc \
	protos/juniper/afi_cap_entry/afi_cap_entry.pb.cc \
	protos/juniper/afi_cap_entry_match/afi_cap_entry_match.pb.cc \
	protos/juniper/afi_cap_entry_action/afi_cap_entry_action.pb.cc
endif


OBJS=$(subst .cc,.o, $(subst .cpp,.o, $(SRCS)))

$(TARGET_LIB): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^
	rm -f ${basename $@}
	ln -s ${notdir $@} ${basename $@}

srcs: generate_protobuf $(SRCS)
	@echo Generated protobufs:
	find protos

generate_protobuf: validate_yang
	@echo Generating protobufs...
	$(PROTOG) -output_dir ./protos/ \
			-package_name juniper \
			-yext_path yext \
			-ywrapper_path ywrapper\
			-add_schemapaths=false \
			*.yang 

validate_yang:
	@echo Validating...
	$(PYANG) *.yang

PROTOC = protoc
AFI_PROTOS_PATH = protos

protos/yext/yext.pb.cc: protos/yext/yext.proto
	$(PROTOC) --proto_path=protos/yext/ --cpp_out=protos/yext/ $<

protos/ywrapper/ywrapper.pb.cc: protos/ywrapper/ywrapper.proto
	$(PROTOC) --proto_path=protos/ywrapper/ --cpp_out=protos/ywrapper/ $<

protos/juniper/afi_tree/afi_tree.pb.cc: protos/juniper/afi_tree/afi_tree.proto
	$(PROTOC) --proto_path=protos/juniper/afi_tree/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_tree/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_tree_entry/afi_tree_entry.pb.cc: protos/juniper/afi_tree_entry/afi_tree_entry.proto
	$(PROTOC) --proto_path=protos/juniper/afi_tree_entry/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_tree_entry/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_encap/afi_encap.pb.cc: protos/juniper/afi_encap/afi_encap.proto
	$(PROTOC) --proto_path=protos/juniper/afi_encap/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_encap/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_encap_entry/afi_encap_entry.pb.cc: protos/juniper/afi_encap_entry/afi_encap_entry.proto
	$(PROTOC) --proto_path=protos/juniper/afi_encap_entry/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_encap_entry/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_tree_encap/afi_tree_encap.pb.cc: protos/juniper/afi_tree_encap/afi_tree_encap.proto
	$(PROTOC) --proto_path=protos/juniper/afi_tree_encap/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_tree_encap/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_tree_encap_entry/afi_tree_encap_entry.pb.cc: protos/juniper/afi_tree_encap_entry/afi_tree_encap_entry.proto
	$(PROTOC) --proto_path=protos/juniper/afi_tree_encap_entry/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_tree_encap_entry/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_cap/afi_cap.pb.cc: protos/juniper/afi_cap/afi_cap.proto
	$(PROTOC) --proto_path=protos/juniper/afi_cap/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_cap/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_cap_match/afi_cap_match.pb.cc: protos/juniper/afi_cap_match/afi_cap_match.proto
	$(PROTOC) --proto_path=protos/juniper/afi_cap_match/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_cap_match/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_cap_action/afi_cap_action.pb.cc: protos/juniper/afi_cap_action/afi_cap_action.proto
	$(PROTOC) --proto_path=protos/juniper/afi_cap_action/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_cap_action/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_cap_entry/afi_cap_entry.pb.cc: protos/juniper/afi_cap_entry/afi_cap_entry.proto
	$(PROTOC) --proto_path=protos/juniper/afi_cap_entry/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_cap_entry/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_cap_entry_match/afi_cap_entry_match.pb.cc: protos/juniper/afi_cap_entry_match/afi_cap_entry_match.proto
	$(PROTOC) --proto_path=protos/juniper/afi_cap_entry_match/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_cap_entry_match/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/afi_cap_entry_action/afi_cap_entry_action.pb.cc: protos/juniper/afi_cap_entry_action/afi_cap_entry_action.proto
	$(PROTOC) --proto_path=protos/juniper/afi_cap_entry_action/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/afi_cap_entry_action/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

protos/juniper/enums/enums.pb.cc: protos/juniper/enums/enums.proto
	$(PROTOC) --proto_path=protos/juniper/enums/  -I $(AFI_PROTOS_PATH) --cpp_out=protos/juniper/enums/ $<
	sleep 2
	find protos -name '*.pb.*' -exec sed -i 's/_ywrapper_2fywrapper_2eproto/_ywrapper_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_yext_2fyext_2eproto/_yext_2eproto/g' {} \;
	find protos -name '*.pb.*' -exec sed -i 's/_juniper_2fenums_2fenums_2eproto/_enums_2eproto/g' {} \;

clean:
ifdef UBUNTU
	rm -rf protos/juniper protos/yext/yext.pb.* protos/ywrapper/ywrapper.pb.*
endif
	$(RM) -f $(OBJS) $(TARGET_LIB) ./.depend

ifeq ($(prefix),)
    prefix := /usr/local
endif

.PHONY: install
install: $(TARGET_LIB)
	install -d $(DESTDIR)$(prefix)/lib64/
	install -m 644 ./*so* $(DESTDIR)$(prefix)/lib64/
	install -m 644 $(TARGET_LIB) $(DESTDIR)$(prefix)/lib64/
	rm -f $(DESTDIR)$(prefix)/lib64/${basename $(TARGET_LIB)}
	ln -s ${notdir $(TARGET_LIB)} $(DESTDIR)$(prefix)/lib64/${basename $(TARGET_LIB)}
	install -d $(DESTDIR)$(prefix)/include/jp4agent
	install -d $(DESTDIR)$(prefix)/include/jp4agent/AFI
	cp --parents `find -name \*.h` $(DESTDIR)$(prefix)/include/jp4agent/AFI/

#depend: .depend
#
#.depend: $(SRCS)
#	$(RM) -f ./.depend
#	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >  ./.depend;
#
#include .depend
