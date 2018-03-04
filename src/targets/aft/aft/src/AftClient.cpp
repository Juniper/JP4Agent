//
// Juniper P4 Agent AFI Client
//
/// @file  AftClient.cpp
/// @brief AFT client for Juniper P4 Agent
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

#include "AftClient.h"
#include <string>
#include <utility>
#include <vector>

// namespace AFTHALP {

//
// init
//
void
AftClient::init(const std::string &configFile)
{
    readConfig(configFile);
    createTransportToAftServer();
    int status = openSandbox();
    assert(status == 0);
}

//
// read configuration
//
int
AftClient::readConfig(const std::string &configFile)
{
    Log(DEBUG) << "Reading config file: " << configFile;
    return 0;
}

//
// @fn
// openSandbox
//
// @brief
// Open AFI sandbox created via Junos CLI configuration
//
// @param[in] void
// @return 0 - Success, -1 - Failure
//

int
AftClient::openSandbox()
{
    std::string proxy_sandbox_name = _sandbox_name + "-JUNOS";

    if (_aft_debugmode.find("debug-aft-client") != std::string::npos) {
        Log(DEBUG) << "sandbox_name: " << _sandbox_name;
        Log(DEBUG) << "proxy_sandbox_name: " << proxy_sandbox_name;
    }

    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        Log(DEBUG) << "_aft_debugmode: " << _aft_debugmode
                   << " No alloc/open sandbox";
        return 0;
    }

    bool status = _transport->alloc("proxy", proxy_sandbox_name, 1, 1);
    if (status == 0) {
        Log(ERROR) << "_transport->alloc (" << proxy_sandbox_name
                   << ") failed!";
        return -1;
    } else {
        if (_aft_debugmode.find("debug-aft-client") != std::string::npos) {
            Log(DEBUG) << "_transport->alloc(" << proxy_sandbox_name
                       << ") SUCCESS!";
        }
    }

    if (_transport->open(proxy_sandbox_name, _sandbox)) {
        Log(DEBUG) << "Got Sandbox (" << proxy_sandbox_name << ") handle!";
    } else {
        Log(ERROR) << "Open Sandbox (" << proxy_sandbox_name << ") failed!";
        return -1;
    }

    if (_aft_debugmode.find("debug-aft-client") != std::string::npos) {
        Log(DEBUG) << "_sandbox->inputPortCount(): "
                   << _sandbox->inputPortCount();
        Log(DEBUG) << "_sandbox->inputPortMax(): " << _sandbox->inputPortMax();
        Log(DEBUG) << "_sandbox->outputPortCount(): "
                   << _sandbox->outputPortCount();
        Log(DEBUG) << "_sandbox->outputPortMax(): "
                   << _sandbox->outputPortMax();
    }

    AftPortTablePtr inputPorts = _sandbox->inputPortTable();

    Log(DEBUG) << "InputPorts: ";
    if (_aft_debugmode.find("debug-aft-client") != std::string::npos) {
        Log(DEBUG) << "inputPorts->maxIndex(): " << inputPorts->maxIndex();
    }

    AftPortPtr port;
    for (AftIndex i = 0; i < inputPorts->maxIndex(); i++) {
        if (inputPorts->portForIndex(i, port)) {
            Log(DEBUG) << "index: " << i << " port name: " << port->portName()
                       << " port If name: " << port->portIfName()
                       << " token: " << port->nodeToken();
        } else {
            Log(DEBUG) << "Error getting port for port index " << i;
        }
    }

    AftPortTablePtr outputPorts = _sandbox->outputPortTable();

    Log(DEBUG) << "OutputPorts: ";
    if (_aft_debugmode.find("debug-aft-client") != std::string::npos) {
        Log(DEBUG) << "outputPorts->maxIndex(): " << outputPorts->maxIndex();
    }

    for (AftIndex i = 0; ((i < outputPorts->maxIndex()) && (i < 20)); i++) {
        if (outputPorts->portForIndex(i, port)) {
            Log(DEBUG) << "index: " << i << " port name: " << port->portName()
                       << " port If name: " << port->portIfName()
                       << " token: " << port->nodeToken();
        } else {
            Log(DEBUG) << "Error getting port for port index " << i;
        }
    }

    return 0;
}

AftNodeToken
AftClient::outputPortToken(AftIndex portIndex)
{
    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        Log(DEBUG) << "_aft_debugmode: " << _aft_debugmode
                   << " Returning 20000";
        return 20000;
    }

    AftPortTablePtr outputPorts = _sandbox->outputPortTable();

    AftPortPtr port;
    for (AftIndex i = 0; i < outputPorts->maxIndex(); i++) {
        if ((i == portIndex) && (outputPorts->portForIndex(i, port))) {
            Log(DEBUG) << "Found output port " << portIndex << "Returning "
                       << port->nodeToken();
            return port->nodeToken();
        }
    }
    std::cout << "output port " << portIndex << " not found\n";

    return AFT_NODE_TOKEN_NONE;
}

AftNodeToken
AftClient::puntPortToken(void)
{
    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        Log(DEBUG) << "_aft_debugmode: " << _aft_debugmode
                   << " Returning 30000";
        return 30000;
    }

    AftPortTablePtr outputPorts = _sandbox->outputPortTable();

    AftPortPtr port;
    for (AftIndex i = 0; i < outputPorts->maxIndex(); i++) {
        if ((outputPorts->portForIndex(i, port)) &&
            (port->portName().compare("punt") == 0)) {
            Log(DEBUG) << "Found punt port "
                       << "Returning " << port->nodeToken();
            return port->nodeToken();
        }
    }

    Log(ERROR) << "punt port not found\n";

    return AFT_NODE_TOKEN_NONE;
}

//
// @fn
// setInputPortNextNode
//
// @brief
// Set next node for an input port
//
// @param[in]
//     inputPortIndex Input port index
// @param[in]
//     nextToken Next node token
// @return 0 - Success, -1 - Error
//

int
AftClient::setInputPortNextNode(AftIndex inputPortIndex, AftNodeToken nextToken)
{
    AftInsertPtr insert = AftInsert::create(_sandbox);

    AftNodePtr inputPort =
        _sandbox->setInputPortByIndex(inputPortIndex, nextToken);

    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        return 0;
    }

    insert->push(inputPort);

    //
    // Send the new input port to the user sandbox
    //
    // _sandbox->send(insert);
    sendboxSend(insert);
    return 0;
}

//
// @fn
// setIngressStart
//
// @brief
// Set ingress start: Make all input ports point to ingress start node.
//
// @param[in]
//     ingressStartToken Ingress start token
// @return routing table node token
//

int
AftClient::setIngressStart(AftNodeToken ingressStartToken)
{
    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        return 0;
    }

    AftPortTablePtr inputPorts = _sandbox->inputPortTable();

    for (AftIndex i = 0; i < inputPorts->maxIndex(); i++) {
        setInputPortNextNode(i, ingressStartToken);
        std::cout << "AftClient:: setting ingress port " << i << " --> token"
                  << ingressStartToken << "\n";
    }
    return 0;
}

//
// @fn
// addTable
//
// @brief
// Add table
//
// @param[in]
//     tableName Routing table name
// @param[in]
//     defaultTargetToken  Default route target token
// @return routing table node token
//

AftNodeToken
AftClient::addTable(const std::string &tableName, const std::string &fieldName,
                    AftNodeToken defaultTargetToken)
{
    AftNodeToken tableNodeToken;
    // AftNodeToken        discardNodeToken;
    AftInsertPtr insert;

    std::cout << "AftClient::addTable Adding table ... \n";

    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        return 10000;
    }

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    //
    // Create a route lookup tree
    //
    auto treePtr = AftTree::create(AftField(fieldName), defaultTargetToken);

    //
    // Stitch the optional params
    //
    uint32_t skipBits = 16;
    treePtr->setNodeParameter<std::string>("app", "NH");
    treePtr->setNodeParameter<std::string>("rt.nhType", "route");
    treePtr->setNodeParameter<uint32_t>("rt.skipBits", skipBits);

    tableNodeToken = insert->push(treePtr);
    std::cout << "AftClient::addTable tableNodeToken:" << tableNodeToken
              << "\n";
    insert->push(tableName, tableNodeToken);

    //
    // Send all the nodes to the sandbox
    //
    // _sandbox->send(insert);
    sendboxSend(insert);

    return tableNodeToken;
}

//
// @fn
// addRouteTable
//
// @brief
// Add routing table
//
// @param[in]
//     rttName Routing table name
// @param[in]
//     defaultTargetToken  Default route target token
// @return routing table node token
//

AftNodeToken
AftClient::addRouteTable(const std::string &rttName,
                         AftNodeToken       defaultTargetToken)
{
    AftNodeToken rttNodeToken;
    // AftNodeToken        discardNodeToken;
    AftInsertPtr insert;

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    //
    // Create a route lookup tree
    //
    auto treePtr =
        AftTree::create(AftField("packet.lookupkey"), defaultTargetToken);

    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        return 0;
    }

    rttNodeToken = insert->push(treePtr);
    insert->push(rttName, rttNodeToken);

    //
    // Send all the nodes to the sandbox
    //
    // _sandbox->send(insert);
    sendboxSend(insert);

    return rttNodeToken;
}

int
AftClient::addRoute(AftNodeToken rttNodeToken, const char *prefix_bytes,
                    int num_prefix_bytes, int prefix_len,
                    AftNodeToken routeTargetToken)
{
    AftInsertPtr insert;
    // AftNodeToken        outputPortToken;

    AftDataBytes aftdatabytes_prefix;
    for (int t = 0; t < num_prefix_bytes; ++t) {
        uint8_t byte = prefix_bytes[t];
        aftdatabytes_prefix.push_back(byte);
    }

    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        return 0;
    }

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    std::cout << "Adding route ---> Node token " << routeTargetToken
              << std::endl;

    //
    // Create a route
    //
    // Ref: aftman/rt/rt-trio/src/RtEntryTrio.cpp
    AftDataPtr daddr = AftDataPrefix::create(
        aftdatabytes_prefix, prefix_len);  // TBD: Check if 32 is fine
    AftEntryPtr entryPtr = AftEntryRoute::create(rttNodeToken, std::move(daddr),
                                                 routeTargetToken, true);

    insert->push(entryPtr);

    //
    // Send all the nodes to the sandbox
    //
    // _sandbox->send(insert);
    sendboxSend(insert);

    return 0;
}

//
// @fn
// addRoute
//
// @brief
// Add route to a routing table
//
// @param[in]
//     rttNodeToken Routing table node token
// @param[in]
//     prefix Route prefix
// @param[in]
//     routeTargetToken Route target token
// @return 0 - Success, -1 - Error
//

int
AftClient::addRoute(AftNodeToken rttNodeToken, const std::string &prefix,
                    AftNodeToken routeTargetToken)
{
    AftInsertPtr insert;
    // AftNodeToken        outputPortToken;

    std::vector<std::string> prefix_sub_strings;
    boost::split(prefix_sub_strings, prefix, boost::is_any_of("./"));

    if (prefix_sub_strings.size() != 5) {
        std::cout << "Invalid prefix" << std::endl;
        return -1;
    }

    AftDataBytes aftdatabytes_prefix;
    for (unsigned int t = 0; t < (prefix_sub_strings.size() - 1); ++t) {
        uint8_t byte = std::strtoul(prefix_sub_strings.at(t).c_str(), NULL, 0);
        aftdatabytes_prefix.push_back(byte);
    }

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    std::cout << "Adding route ";
    std::cout << prefix << " ---> Node token " << routeTargetToken << std::endl;

    //
    // Create a route
    //
    // Ref: aftman/rt/rt-trio/src/RtEntryTrio.cpp
    AftDataPtr  daddr    = AftDataPrefix::create(aftdatabytes_prefix,
                                             32);  // TBD: Check if 32 is fine
    AftEntryPtr entryPtr = AftEntryRoute::create(rttNodeToken, std::move(daddr),
                                                 routeTargetToken, true);

    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        return 0;
    }

    insert->push(entryPtr);

    //
    // Send all the nodes to the sandbox
    //
    // _sandbox->send(insert);
    sendboxSend(insert);

    return 0;
}

//
// @fn
// createList
//
// @brief
// Create List
//
// @param[in]
//     tokVec Token vector
// @return List token
//

AftNodeToken
AftClient::createList(AftTokenVector tokVec)
{
    AftInsertPtr insert;

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    //
    // Build a list of provided tokens
    //
    // AftNodePtr list = AftList::create(token1, token2);
    AftNodePtr list = AftList::create(tokVec);

    insert->push(list);

    //
    // Send all the nodes to the sandbox
    //
    _sandbox->send(insert);

    return list->nodeToken();
}

//
// @fn
// getOuputPortToken
//
// @brief
// Get token for an output port
//
// @param[in]
//     outputPortIndex Output port index
// @return Outport port's token
//

AftNodeToken
AftClient::getOuputPortToken(AftIndex outputPortIndex)
{
    AftNodeToken outputPortToken;

    _sandbox->outputPortByIndex(outputPortIndex, outputPortToken);

    return outputPortToken;
}

//
// @fn
// addReceiveNode
//
// @brief
// Add Receive node
//
// @param[in]
//     receiveCode Receive code
// @param[in]
//     context Context
// @return Receive node's token
//

AftNodeToken
AftClient::addReceiveNode(uint32_t receiveCode, uint64_t context)
{
    //
    // Allocate an insert context
    //
    AftInsertPtr insert = AftInsert::create(_sandbox);

    //
    // Create aft encap node
    //
    auto aftReceivePtr = AftReceive::create(receiveCode, context);

    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        return 0;
    }

    AftNodeToken nhReceiveToken = insert->push(aftReceivePtr);

    //
    // Send all the nodes to the sandbox
    //
    _sandbox->send(insert);

    return nhReceiveToken;
}

//
// @fn
// addEtherEncapNode
//
// @brief
// Add Ethernet encap node
//
// @param[in]
//     dst_mac Destination MAC
// @param[in]
//     src_mac Source MAC
// @param[in]
//     nextToken Next node token
// @return Ethernet encap node's token
//

AftNodeToken
AftClient::addEtherEncapNode(const std::string &dst_mac,
                             const std::string &src_mac, AftNodeToken nextToken)
{
    if (_aft_debugmode.find("no-aft-server") != std::string::npos) {
        return 0;
    }

    //
    // Allocate an insert context
    //
    AftInsertPtr insert = AftInsert::create(_sandbox);
    //
    // Create a key vector of ethernet data
    //

    AftKeyVector encapKeys = {AftKey(AftField("packet.ether.saddr"),
                                     AftDataEtherAddr::create(src_mac)),
                              AftKey(AftField("packet.ether.daddr"),
                                     AftDataEtherAddr::create(dst_mac))};
    //
    // Create aft encap node
    //
    auto aftEncapPtr = AftEncap::create("ethernet", encapKeys);

    //
    // Set the List node token as next pointer to encap
    //
    aftEncapPtr->setNodeNext(nextToken);

    AftNodeToken nhEncapToken = insert->push(aftEncapPtr);

    //
    // Send all the nodes to the sandbox
    //
    _sandbox->send(insert);

    return nhEncapToken;
}

//}  // namespace AFTHALP
