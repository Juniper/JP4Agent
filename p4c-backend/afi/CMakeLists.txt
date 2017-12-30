#
# Advanced Forwarding Interface : AFI client examples
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

# Makefile for a backend that generates code for the Juniper's Advanced Forwarding Interface (AFI)
# compiling for the simple_switch target.

# sources for backend executable

include_directories(AFI/protos)

set (AFI_SRCS afi.cpp)
add_cpplint_files (${CMAKE_CURRENT_SOURCE_DIR} "${AFI_SRCS}")

set (AFI_BACKEND_SRCS
  analyzer.cpp
  action.cpp
  backend.cpp
  control.cpp
  deparser.cpp
  errorcode.cpp
  expression.cpp
  extern.cpp
  header.cpp
  helpers.cpp
  extractArchInfo.cpp
  JsonObjects.cpp
  lower.cpp
  midend.cpp
  metermap.cpp
  parser.cpp
  portableSwitch.cpp
  sharedActionSelectorCheck.cpp
  afiSwitch.cpp
  base64.cpp
  AFI/protos/juniper/enums/enums.pb.cc
  AFI/protos/juniper/afi_tree/afi_tree.pb.cc
  AFI/protos/yext/yext.pb.cc
  AFI/protos/ywrapper/ywrapper.pb.cc
  )

#portableSwitch.cpp
#simpleSwitch.cpp

set (AFI_BACKEND_HDRS
  analyzer.h
  action.h
  backend.h
  control.h
  deparser.h
  errorcode.h
  expression.h
  extern.h
  header.h
  helpers.h
  extractArchInfo.h
  JsonObjects.h
  lower.h
  midend.h
  metermap.h
  options.h
  parser.h
  portableSwitch.h
  sharedActionSelectorCheck.h
  afiSwitch.h
  base64.h
  AFI/protos/juniper/enums/enums.pb.h
  AFI/protos/juniper/afi_tree/afi_tree.pb.h
  AFI/protos/yext/yext.pb.h
  AFI/protos/ywrapper/ywrapper.pb.h
  )

#  portableSwitch.h

set (IR_DEF_FILES ${IR_DEF_FILES} PARENT_SCOPE)
add_cpplint_files (${CMAKE_CURRENT_SOURCE_DIR} "${AFI_BACKEND_SRCS}")

build_unified(AFI_BACKEND_SRCS)
add_library(afibackend ${AFI_BACKEND_SRCS})
add_dependencies(afibackend genIR)

build_unified(AFI_SRCS)
add_executable(p4c-afi ${AFI_SRCS})
target_link_libraries (p4c-afi afibackend ${P4C_LIBRARIES} ${P4C_LIB_DEPS})

install(TARGETS p4c-afi RUNTIME DESTINATION ${P4C_RUNTIME_OUTPUT_DIRECTORY})

# hack to get around the fact that the test scripts expect the backend
# binary to be in the top level directory. This should go away when we
# remove automake and fix the scripts.
add_custom_target(linkafi
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/p4c-afi ${P4C_BINARY_DIR}/p4c-afi
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${P4C_BINARY_DIR}/p4include ${CMAKE_CURRENT_BINARY_DIR}/p4include
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${P4C_BINARY_DIR}/p4_14include ${CMAKE_CURRENT_BINARY_DIR}/p4_14include
  )
add_dependencies(p4c_driver linkafi)

