//
// Juniper P4 Agent
//
/// @file  JP4Agent.cpp
/// @brief Juniper P4 Agent
//
// Created by Sandesh Kumar Sodhi, November 2017
// Copyright (c) [2017] Juniper Networks, Inc. All rights reserved.
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

#ifndef __ControllerConnection__
#define __ControllerConnection__

#include <mutex>
#include "pvtPI.h"

class ControllerConnection;
extern ControllerConnection controller_conn;

// This connection represents the bi-directional streaming channel between the
// controller and the JP4Agent. For now, assume that there is only one
// controller connected at a time.
class ControllerConnection
{
 public:
    void set_stream(StreamChannelReaderWriter *stream)
    {
        std::lock_guard<std::mutex> lock{scm};
        stream_ = stream;
    }
    void clear_stream()
    {
        std::lock_guard<std::mutex> lock{scm};
        stream_ = nullptr;
    }

    // Send pkt on the stream channel to the controller.
    bool send_pkt_in(p4::PacketIn *pkt) const
    {
        bool                        pkt_sent = false;
        std::lock_guard<std::mutex> lock{scm};
        if (stream_) {
            p4::StreamMessageResponse response;
            response.set_allocated_packet(pkt);
            stream_->Write(response);
            response.release_packet();
            pkt_sent = true;
        }
        return pkt_sent;
    }

 private:
    mutable std::mutex         scm;  // Guards access to stream channel ptr.
    StreamChannelReaderWriter *stream_{nullptr};
};

#endif  // __ControllerConnection__
