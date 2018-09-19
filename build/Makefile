#
# Makefile -- Top level Makefile
#
# JP4Agent
#
# Created by Sudheendra Gopinath, September 2018
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

COMPONENTS = \
	../AFI \
	../src/pi/protos \
	../src/jp4agent/src \
	../src/utils/src \
	../src/pi/src \
	../src/afi/src \
	../test/controller/src \
	../test/gtest/src \
	../src/targets/null/null/src \
	../src/targets/null/src

.PHONY: all $(COMPONENTS)
all: $(COMPONENTS)
$(COMPONENTS):
ifeq ($(ubuntu), 1)
	make -j1 srcs -C ../AFI
endif
	make -j1 -C $@ UBUNTU=${ubuntu} DEBUG_BUILD=1 CODE_COVERAGE=1

INSTALL_COMPONENTS = $(COMPONENTS:%=install-%)
.PHONY: install $(INSTALL_COMPONENTS)
install: $(INSTALL_COMPONENTS)
$(INSTALL_COMPONENTS):
	make install -C $(@:install-%=%)

CLEAN_COMPONENTS = $(COMPONENTS:%=clean-%)
.PHONY: clean $(CLEAN_COMPONENTS)
clean: $(CLEAN_COMPONENTS)
$(CLEAN_COMPONENTS):
	make clean -C $(@:clean-%=%) UBUNTU=${ubuntu}

