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

#include "Afi.h"
#include "ControllerConnection.h"
#include "pvtPI.h"
#include "JaegerLog.h"
#include <vector>

const std::string _debugmode = "debug-afi-objects:debug-pi";

Status
P4RuntimeServiceImpl::SetForwardingPipelineConfig(
    ServerContext *                               context,
    const p4::SetForwardingPipelineConfigRequest *request,
    p4::SetForwardingPipelineConfigResponse *     rep)
{
    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "P4Runtime SetForwardingPipelineConfig\n";
        Log(DEBUG) << request->DebugString();
    }
    (void)rep;
    // Create a span for route table
    JaegerLog::getInstance()->startSpan("Route Table");

    p4::SetForwardingPipelineConfigRequest_Action a      = request->action();
    p4::ForwardingPipelineConfig                  config = request->config();

    p4::config::P4Info p4info_proto = config.p4info();

    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "action:" << a;
        Log(DEBUG) << "request->configs_size():" << request->configs_size();
        Log(DEBUG) << p4info_proto.DebugString();
        Log(DEBUG) << "________ P4 INFO __________";

        Log(DEBUG) << "p4info_proto.tables_size():"
                   << p4info_proto.tables_size();
        Log(DEBUG) << "p4info_proto.actions_size():"
                   << p4info_proto.actions_size();
    }
    Log(DEBUG) << p4info_proto.DebugString();

    const auto &actions = p4info_proto.actions();
    for (const auto &action : actions) {
        const auto &pre = action.preamble();
        if (_debugmode.find("debug-pi") != std::string::npos) {
            Log(DEBUG) << "_________ Action _______";
            Log(DEBUG) << "Id:" << pre.id();
            Log(DEBUG) << "Name:" << pre.name();
        }


        P4InfoResourcePtr res(new P4InfoAction(action));
        P4Info::instance().insert2IdMap(res);
        P4Info::instance().insert2NameMap(res);
    }

    const auto &tables = p4info_proto.tables();
    Log(DEBUG) << "tables.size():" << tables.size();
    for (const auto &table : tables) {
        const auto &pre = table.preamble();
        if (_debugmode.find("debug-pi") != std::string::npos) {
            Log(DEBUG) << "_________ Table _______";
            Log(DEBUG) << "Id:" << pre.id();
            Log(DEBUG) << "Name:" << pre.name();
        }

        P4InfoResourcePtr res(new P4InfoTable(table));
        P4Info::instance().insert2IdMap(res);
        P4Info::instance().insert2NameMap(res);
    }

    p4::tmp::P4DeviceConfig p4_device_config;
    if (!p4_device_config.ParseFromString(config.p4_device_config())) {
        Log(ERROR) << "Invalid 'p4_device_config', not an instance of "
                   << "p4::tmp::P4DeviceConfig";
    }

    const auto &device_data = p4_device_config.device_data();

    std::string dd_str =
        std::string((char *)device_data.data(), device_data.size());
    JaegerLog::getInstance()->Log("PI:SetFwdPpln:Device Data", dd_str);

    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "device_data.data():" << device_data.data();
        Log(DEBUG) << "device_data.size():" << device_data.size();
        Log(DEBUG) << "_____ JASON ________\n";
        Log(DEBUG) << dd_str;
    }

    std::istringstream ss(dd_str);
    std::istream *     is = &ss;

    Json::Value cfg_root;
    (*is) >> cfg_root;

    Log(DEBUG) << "_____ Calling AFI handlePipelineConfig ________\n";
    AFIHAL::Afi::instance().handlePipelineConfig(cfg_root);
    return Status::OK;
}

Status
P4RuntimeServiceImpl::tableInsert(const p4::TableEntry &tableEntry)
{
    const auto tableId  = tableEntry.table_id();
    const auto priority = tableEntry.priority();
    Log(DEBUG) << "tableInsert: tableId: " << tableId;
    Log(DEBUG) << "tableInsert: priority: " << priority;

    std::stringstream ts, ps;
    ts << tableId;
    ps << priority;
    JaegerLog::getInstance()->Log("PI:Tbl Insert:Table ID", ts.str());
    JaegerLog::getInstance()->Log("PI:Tbl Insert:Priority", ps.str());

    Status status = Status::OK;

    if (tableEntry.is_default_action()) {
        if (!tableEntry.match().empty()) {
            Log(ERROR) << "Default tableEntry has non-empty key";
            // TBD: Return error status and translate it to
            // grpc status
        }
        if (tableEntry.priority() != 0) {
            Log(ERROR) << "Default tableEntry has non-zero priority";
        }
        return status;
    }

    Log(DEBUG) << "tableInsert: match size: "
               << static_cast<size_t>(tableEntry.match().size());

    std::vector<AFIHAL::AfiTEntryMatchField> afiMFs;

    for (const auto &mf : tableEntry.match()) {
        Log(DEBUG) << "match field id: " << mf.field_id();

        if (mf.has_valid()) {
            Log(DEBUG) << "match field has valid: ";
        }
        if (mf.has_exact()) {
            Log(DEBUG) << "match field has exact: ";
        }
        if (mf.has_lpm()) {
            Log(DEBUG) << "match field has lpm: ";

            auto               lpm       = mf.lpm();
            const std::string &keystr    = lpm.value();
            int                prefixLen = lpm.prefix_len();
            Log(DEBUG) << "keystr.size(): " << keystr.size();
            Log(DEBUG) << "bytes: ";
            if (keystr.size() == 4) {
                // Print IPv4 prefix
                const char *c = keystr.c_str();
                std::cout << +c[0] << "." << +c[1] << "." << +c[2] << "."
                          << +c[3] << "\n";
            }

            Log(DEBUG) << "prefixLen: " << prefixLen;
            AFIHAL::Afi::instance().addEntry(keystr, prefixLen);
        }

        if (mf.has_ternary()) {
            Log(DEBUG) << "match field has ternary: ";

            auto ternary = mf.ternary();
            AFIHAL::AfiTEntryMatchField afiMF(mf.field_id(),
                                              ternary.value(),
                                              ternary.mask());

            afiMFs.push_back(afiMF);

        }

        if (mf.has_range()) {
            Log(DEBUG) << "match field has range: ";
        }
    }

    std::vector<AFIHAL::AfiAEntry> afiActions;
    P4InfoResourceId actionId;

    const p4::TableAction &tableAction = tableEntry.action();
    if (tableAction.type_case() == p4::TableAction::kAction) {
        const p4::Action &action = tableAction.action();
        actionId = tableAction.action().action_id();
        for (const auto &p : action.params()) {
            Log(DEBUG) << "paramt id: " << p.param_id();
            AFIHAL::AfiAEntry afiAEntry(p.param_id(), p.value());
            afiActions.push_back(afiAEntry);
        }
    }

    AFIHAL::Afi::instance().afiAddObjEntry(tableId,
                                           actionId,
                                           afiMFs,
                                           afiActions);

#if 0
    uint16_t               portId      = 0;
    const p4::TableAction &tableAction = tableEntry.action();

    switch (tableAction.type_case()) {
        case p4::TableAction::kAction: {
            Log(DEBUG) << "tableAction : Action ";
            const p4::Action &action = tableAction.action();
            actionId                 = tableAction.action().action_id();
            Log(DEBUG) << "actionId: " << actionId;

            P4InfoResourcePtr resAction =
                P4Info::instance().p4InfoResource(actionId);
            P4InfoActionPtr p4InfoAction =
                std::dynamic_pointer_cast<P4InfoAction>(resAction);

            p4InfoAction->display();

            for (const auto &p : action.params()) {
                Log(DEBUG) << "paramt id: " << p.param_id();

                const char *c = p.value().data();

                size_t s = p.value().size();
                Log(DEBUG) << "bytes size: " << s;

                for (size_t i = 0; i < s; i++) {
                    std::cout << int(c[i]) << "\n";
                }

                std::string paramName =
                    p4InfoAction->actionParamName(p.param_id());

                if (paramName.compare("port") == 0) {
                    assert(p.value().size() == 2);
                    portId = ntohs(*(uint16_t *)(c));
                    // std::cout<<"portId :" << portId << "\n";
                }
            }
        } break;
        case p4::TableAction::kActionProfileMemberId:
            Log(DEBUG) << "tableAction : ActionProfileMemberId ";
            break;
        case p4::TableAction::kActionProfileGroupId:
            Log(DEBUG) << "tableAction : ActionProfileGroupId";
            break;
        default:
            Log(ERROR) << "tableAction : error";
            break;
    }

    P4InfoResourcePtr resTable = P4Info::instance().p4InfoResource(tableId);
    P4InfoTablePtr    p4InfoTable =
        std::dynamic_pointer_cast<P4InfoTable>(resTable);

    p4InfoTable->display();

    P4InfoResourcePtr resAction = P4Info::instance().p4InfoResource(actionId);
    P4InfoActionPtr   p4InfoAction =
        std::dynamic_pointer_cast<P4InfoAction>(resAction);

    p4InfoAction->display();

    Log(DEBUG) << "portId              :" << portId;

    AfiObjectPtr afiObjPtr = _afiDevCfg.getAfiTree(p4InfoTable->name());

    AfiTreePtr afiTreePtr = std::dynamic_pointer_cast<AfiTree>(afiObjPtr);

    std::cout << "afiTree :" << afiTreePtr << "\n";

    AftNodeToken afiTreeToken = 0;
    if (afiTreePtr != nullptr) {
        afiTreeToken = afiTreePtr->token();
    }
    std::cout << "afiTreePtr->token() :" << afiTreeToken << "\n";

    AftNodeToken outputPortToken =
        jP4Agent->afiClient().outputPortToken(portId);
    std::cout << "outputPortToken:" << outputPortToken << "\n";

    AftNodeToken etherEncapToken = jP4Agent->afiClient().addEtherEncapNode(
        "32:26:0a:2e:ff:f1", "5e:d8:f9:32:bd:85", outputPortToken);

    std::cout << "etherEncapToken:" << etherEncapToken << "\n";
    // jP4Agent->afiClient().addRoute(afiTreeToken, "1.1.1.1/10",
    // etherEncapToken);
    std::cout << "Adding route...\n";
    for (int i = 0; i < num_prefix_bytes; i++) {
        std::cout << int(prefix_bytes[i])
                  << "."
                     "\n";
    }
    jP4Agent->afiClient().addRoute(afiTreeToken, prefix_bytes, num_prefix_bytes,
                                   prefix_len, etherEncapToken);
#endif

    return status;
}

Status
P4RuntimeServiceImpl::tableWrite(p4::Update_Type       update,
                                 const p4::TableEntry &table_entry)
{
    Log(DEBUG) << "tableWrite: table_id: " << table_entry.table_id();
    // if (!check_p4_id(table_entry.table_id(), P4ResourceType::TABLE))
    //  return make_invalid_p4_id_status();
    if (table_entry.has_meter_config() || table_entry.has_counter_data()) {
        Log(DEBUG) << "Direct resources not supported in TableEntry yet: ";
        return Status::OK;
    }
    // Status status;
    Status status = Status::OK;
    switch (update) {
        case p4::Update_Type_UNSPECIFIED:
            Log(DEBUG) << "p4::Update_Type_UNSPECIFIED";
            break;
        case p4::Update_Type_INSERT:
            Log(DEBUG) << "p4::Update_Type_INSERT";
            return tableInsert(table_entry);
            break;
        case p4::Update_Type_MODIFY:
            Log(DEBUG) << "p4::Update_Type_MODIFY";
            break;
        case p4::Update_Type_DELETE:
            Log(DEBUG) << "p4::Update_Type_DELETE";
            break;
        default:
            Log(DEBUG) << "tableWrite: ____ default";
            break;
    }
    return status;
}

Status
P4RuntimeServiceImpl::_write(const p4::WriteRequest &request)
{
    Status status = Status::OK;
    // status.set_code(Code::OK);
    for (const auto &update : request.updates()) {
        const auto &entity = update.entity();
        switch (entity.entity_case()) {
            case p4::Entity::kExternEntry:
                Log(DEBUG) << "p4::Entity::kExternEntry";
                break;
            case p4::Entity::kTableEntry:
                Log(DEBUG) << "p4::Entity::kTableEntry";
                status = tableWrite(update.type(), entity.table_entry());
                break;
            case p4::Entity::kActionProfileMember:
                Log(DEBUG) << "p4::Entity::kActionProfileMember";
                break;
            case p4::Entity::kActionProfileGroup:
                Log(DEBUG) << "p4::Entity::kActionProfileGroup";
                break;
            case p4::Entity::kMeterEntry:
                Log(DEBUG) << "p4::Entity::kMeterEntry";
                break;
            case p4::Entity::kDirectMeterEntry:
                Log(DEBUG) << "p4::Entity::kDirectMeterEntry";
                break;
            case p4::Entity::kCounterEntry:
                Log(DEBUG) << "p4::Entity::kCounterEntry";
                break;
            case p4::Entity::kDirectCounterEntry:
                Log(DEBUG) << "p4::Entity::kDirectCounterEntry";
                break;
            default:
                Log(DEBUG) << "_____default";
                break;
        }
        // if (status.code() != Code::OK) break;
    }
    return status;
}

Status
P4RuntimeServiceImpl::Write(ServerContext *         context,
                            const p4::WriteRequest *request,
                            p4::WriteResponse *     rep)
{
    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "_____ P4Runtime Write _____\n";
        Log(DEBUG) << request->DebugString();
    }
    Log(DEBUG) << "_____ P4Runtime Write _____\n";
    // Log(DEBUG) << request->DebugString();
    (void)rep;

    auto deviceId = request->device_id();
    Log(DEBUG) << "Device id :" << deviceId;
    if (request->has_election_id()) {
        Log(DEBUG) << "Request has election id";
    }
    auto electionId = convert_u128(request->election_id());
    // Log(DEBUG) << "Election id :" << static_cast<int>(electionId);
    // std::cout << "Election id :" << electionId << "\n";

    std::stringstream ds, es;
    ds << deviceId;
    es << electionId;
    JaegerLog::getInstance()->Log("PI:Write:Device ID", ds.str());
    JaegerLog::getInstance()->Log("PI:Write:Election ID", es.str());

    auto status = _write(*request);

    std::this_thread::sleep_for(std::chrono::seconds{5});
    // return Status::OK;
    return status;
}

Status
P4RuntimeServiceImpl::Read(ServerContext *                 context,
                           const p4::ReadRequest *         request,
                           ServerWriter<p4::ReadResponse> *writer)
{
    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "_____ P4Runtime Read _____\n";
        Log(DEBUG) << request->DebugString();
    }
    p4::ReadResponse response;
    return Status::OK;
}

Status
P4RuntimeServiceImpl::GetForwardingPipelineConfig(
    ServerContext *                               context,
    const p4::GetForwardingPipelineConfigRequest *request,
    p4::GetForwardingPipelineConfigResponse *     rep)
{
    if (_debugmode.find("debug-pi") != std::string::npos) {
        Log(DEBUG) << "_____ P4Runtime GetForwardingPipelineConfig _____\n";
        Log(DEBUG) << request->DebugString();
    }
    (void)rep;
    return Status::OK;
}

Status
P4RuntimeServiceImpl::StreamChannel(ServerContext *            context,
                                    StreamChannelReaderWriter *stream)
{
    // Update the handle to the StreamChannelReaderWriter.
    controller_conn.set_stream(stream);

    p4::StreamMessageRequest request;
    while (stream->Read(&request)) {
        switch (request.update_case()) {
            case p4::StreamMessageRequest::kArbitration: {
                if (_debugmode.find("debug-pi") != std::string::npos) {
                    Log(DEBUG) << "p4::StreamMessageRequest::kArbitration\n";
                }
                const auto device_id = request.arbitration().device_id();
                const auto election_id =
                    convert_u128(request.arbitration().election_id());
                if (_debugmode.find("debug-pi") != std::string::npos) {
                    Log(DEBUG) << "device_id:" << device_id;
                    Log(DEBUG) << "election_id:" << election_id;
                }
                p4::StreamMessageResponse response;
                auto arbitration = response.mutable_arbitration();
                auto status      = arbitration->mutable_status();
                status->set_code(::google::rpc::Code::OK);

                stream->Write(response);
            } break;

            case p4::StreamMessageRequest::kPacket: {
                if (_debugmode.find("debug-pi") != std::string::npos) {
                    Log(DEBUG) << "p4::StreamMessageRequest::kPacket\n";
                }
                const std::string &payload = request.packet().payload();

                // Received L2 pkt. Send to the device on the UDP socket.
                _hpPktHdl.sendPacketOut(payload);
            } break;

            default:
                break;
        }
    }

    // This stream channel is closing. Clear handle now.
    controller_conn.clear_stream();
    return Status::OK;
}
