//
// Juniper P4 Agent
//
/// @file  pvtPI.h
/// @brief prirave PI declaration and includes
//
// Created by Sandesh Kumar Sodhi, January 2018
// Copyright (c) [2018] Juniper Networks, Inc. All rights reserved.
//
// All rights reserved.
//
// Notice and Disclaimer: This code is licensed to you under the Apache
// License 2.0 (the "License"). You may not use this code except in compliance
// with the License. This code is not an official Juniper product. You can
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Third-Party Code: This code may depend on other components under separate
// copyright notice and license terms. Your use of the source code for those
// components is subject to the terms and conditions of the respective license
// as noted in the Third-Party source code file.
//

#ifndef __pvtPI__
#define __pvtPI__

#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>
#include <map>
#include <cassert>
#include <arpa/inet.h>
#include <algorithm>
#include <cstddef>
#include <mutex>
#include <jsoncpp/json/json.h>

#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/io/ios_state.hpp>

// TBD: Remove grpc:: in code inline
#include <grpc++/grpc++.h>
// #include <grpc++/support/error_details.h>

#include "Log.h"
#include "google/rpc/code.pb.h"
#include "p4runtime.grpc.pb.h"
#include "p4runtime.pb.h"
#include "p4config.pb.h"
#include "uint128.h"
#include "Utils.h"
#include "Afi.h"

using SandboxId         = uint16_t;        ///< Sandbox Id
using DevicePortIndex   = uint16_t;        ///< Device port index 

using StreamChannelReaderWriter = grpc::ServerReaderWriter<
  p4::StreamMessageResponse, p4::StreamMessageRequest>;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

#include "ControllerConnection.h"
#include "DeviceHPPacket.h"
#include "Hostpath.h"
#include "P4Info.h"
#include "P4RuntimeService.h"
#include "PIServer.h"


#endif // __pvtPI__
