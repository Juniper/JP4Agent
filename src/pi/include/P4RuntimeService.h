//
// Juniper P4 Agent
//
/// @file  P4RuntimeService.h
/// @brief P4 Runtime service
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

#ifndef __P4RuntimeService__
#define __P4RuntimeService__

#include "Hostpath.h"

class P4RuntimeServiceImpl : public p4::P4Runtime::Service
{
 public:
    explicit P4RuntimeServiceImpl(Hostpath &hpPktIO) : _hpPktHdl{hpPktIO} {}

 private:
    Hostpath &_hpPktHdl;  // Handle to the hostpath packet IO methods.

    // Methods
    Status tableInsert(const p4::TableEntry &tableEntry);
    Status tableWrite(p4::Update_Type       update,
                      const p4::TableEntry &table_entry);
    Status _write(const p4::WriteRequest &request);

    Status Write(ServerContext *context, const p4::WriteRequest *request,
                 p4::WriteResponse *rep) override;

    Status Read(ServerContext *context, const p4::ReadRequest *request,
                ServerWriter<p4::ReadResponse> *writer) override;

    Status SetForwardingPipelineConfig(
        ServerContext *                               context,
        const p4::SetForwardingPipelineConfigRequest *request,
        p4::SetForwardingPipelineConfigResponse *     rep) override;

    Status GetForwardingPipelineConfig(
        ServerContext *                               context,
        const p4::GetForwardingPipelineConfigRequest *request,
        p4::GetForwardingPipelineConfigResponse *     rep) override;

    Status StreamChannel(ServerContext *            context,
                         StreamChannelReaderWriter *stream) override;

    static Uint128 convert_u128(const p4::Uint128 &from)
    {
        return Uint128(from.high(), from.low());
    }
};

#endif  // __P4RuntimeService__
