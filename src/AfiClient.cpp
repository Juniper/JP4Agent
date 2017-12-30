//
// Juniper P4 Agent AFI Client
//
/// @file  AfiClient.cpp
/// @brief AFI client for Juniper P4 Agent
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

#include "AfiClient.h"

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
AfiClient::openSandbox ()
{
    std::string proxy_sandbox_name = _sandbox_name + "-JUNOS";


    if (_debugmode.find("debug-afi-client") != std::string::npos) {
        Log(DEBUG) << "sandbox_name: "<< _sandbox_name;
        Log(DEBUG) << "proxy_sandbox_name: "<< proxy_sandbox_name;
    }

    if (_debugmode.find("no-afi-server") != std::string::npos) {
        Log(DEBUG) << "_debugmode: "<< _debugmode << " No alloc/open sandbox";
        return 0;
    }

    bool status = _transport->alloc("proxy", proxy_sandbox_name, 1, 1); 
    if (status == 0) {
        Log(ERROR) << "_transport->alloc ("<< proxy_sandbox_name <<") failed!";
        return -1;
    } else {
        if (_debugmode.find("debug-afi-client") != std::string::npos) {
            Log(DEBUG) << "_transport->alloc("<< proxy_sandbox_name <<") SUCCESS!";
        }
    }

    if (_transport->open(proxy_sandbox_name, _sandbox)) {
        Log(DEBUG) << "Got Sandbox ("<< proxy_sandbox_name <<") handle!";
    } else {
        Log(ERROR) << "Open Sandbox ("<< proxy_sandbox_name <<") failed!";
        return -1;
    }

    if (_debugmode.find("debug-afi-client") != std::string::npos) {
        Log(DEBUG) << "_sandbox->inputPortCount(): "<< _sandbox->inputPortCount();
        Log(DEBUG) << "_sandbox->inputPortMax(): "<< _sandbox->inputPortMax();
        Log(DEBUG) << "_sandbox->outputPortCount(): "<< _sandbox->outputPortCount();
        Log(DEBUG) << "_sandbox->outputPortMax(): "<< _sandbox->outputPortMax();
    }

    AftPortTablePtr inputPorts = _sandbox->inputPortTable();

    Log(DEBUG) << "InputPorts: ";
    if (_debugmode.find("debug-afi-client") != std::string::npos) {
        Log(DEBUG) << "inputPorts->maxIndex(): "<< inputPorts->maxIndex();
    }

    AftPortPtr port;
    for (AftIndex  i = 0 ; i < inputPorts->maxIndex();  i++) {
        if (inputPorts->portForIndex(i, port)) {
            Log(DEBUG) << "index: " << i
                       << " port name: " << port->portName()
                       << " port If name: " << port->portIfName()
                       << " token: " << port->nodeToken();
        } else {
            Log(DEBUG) << "Error getting port for port index " << i;
        }
    }

    AftPortTablePtr outputPorts = _sandbox->outputPortTable();

    Log(DEBUG) << "OutputPorts: ";
    if (_debugmode.find("debug-afi-client") != std::string::npos) {
        Log(DEBUG) << "outputPorts->maxIndex(): "<< outputPorts->maxIndex();
    }

    for (AftIndex  i = 0 ; ((i < outputPorts->maxIndex()) && (i < 20));  i++) {
        if (outputPorts->portForIndex(i, port)) {
            Log(DEBUG) << "index: " << i
                       << " port name: " << port->portName()
                       << " port If name: " << port->portIfName()
                       << " token: " << port->nodeToken();
        } else {
            Log(DEBUG) << "Error getting port for port index " << i;
        }
    }

    return 0;
}

AftNodeToken 
AfiClient::outputPortToken (AftIndex portIndex)
{
    if (_debugmode.find("no-afi-server") != std::string::npos) {
        Log(DEBUG) << "_debugmode: "<< _debugmode << " Returning 20000";
        return 20000;
    }

    AftPortTablePtr outputPorts = _sandbox->outputPortTable();

    AftPortPtr port;
    for (AftIndex  i = 0 ; i < outputPorts->maxIndex();  i++) {
        if ((i == portIndex) && (outputPorts->portForIndex(i, port))) {
            Log(DEBUG) << "Found output port " << portIndex
                       << "Returning " << port->nodeToken();
            return port->nodeToken();
        }
    }
    std::cout << "output port " << portIndex << " not found\n";

    return AFT_NODE_TOKEN_NONE;
}

AftNodeToken
AfiClient::puntPortToken(void)
{
    if (_debugmode.find("no-afi-server") != std::string::npos) {
        Log(DEBUG) << "_debugmode: "<< _debugmode << " Returning 30000";
        return 30000;
    }

    AftPortTablePtr outputPorts = _sandbox->outputPortTable();

    AftPortPtr port;
    for (AftIndex  i = 0 ; i < outputPorts->maxIndex();  i++) {
        if ((outputPorts->portForIndex(i, port)) &&
            (port->portName().compare("punt") == 0)) {
            Log(DEBUG)  << "Found punt port "
                        << "Returning " << port->nodeToken();
            return port->nodeToken();
        }
    }

    Log(ERROR) << "punt port not found\n";

    return AFT_NODE_TOKEN_NONE;
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
//     defaultTragetToken  Default route target token
// @return int status
//

int 
AfiClient::crossConnectPortsOp (const std::string &endpoint1,
                                const std::string &endpoint2,
                                AfiClient::CrossConnectOp op)
{
    //
    // This code assumes that AFT server supports overwriting
    // the cross connect information
    //

    AftPortTablePtr inputPorts = _sandbox->inputPortTable();

    AftIndex ep1InputPortIndex      = INVALID_PORT_INDEX;
    AftIndex ep2InputPortIndex      = INVALID_PORT_INDEX;
    AftNodeToken ep1OutputPortToken = AFT_NODE_TOKEN_NONE;
    AftNodeToken ep2OutputPortToken = AFT_NODE_TOKEN_NONE;

    AftPortPtr port;
    for (AftIndex  i = 0 ; i < inputPorts->maxIndex();  i++) {
        if (inputPorts->portForIndex(i, port)) {
            if (port->portName() == endpoint1) {
                ep1InputPortIndex = i;
            } 
            if (port->portName() == endpoint2) {
                ep2InputPortIndex = i;
            }
        }
    }

    if (ep1InputPortIndex == INVALID_PORT_INDEX) {
        Log(DEBUG) << "Endpoint " << endpoint1
                   << " not found in inputs ports";
        return -1; 
    }   

    if (ep2InputPortIndex == INVALID_PORT_INDEX) {
        Log(DEBUG) << "Endpoint " << endpoint2
                   << " not found in inputs ports";
        return -1;
    }

    AftPortTablePtr outputPorts = _sandbox->outputPortTable();

    for (AftIndex  i = 0 ; i < outputPorts->maxIndex();  i++) {
        if (outputPorts->portForIndex(i, port)) {
            if (port->portName() == endpoint1) {
                ep1OutputPortToken = port->nodeToken();
            } 
            if (port->portName() == endpoint2) {
                ep2OutputPortToken = port->nodeToken();
            }
        }
    }

    if (ep1OutputPortToken == AFT_NODE_TOKEN_NONE) {
        Log(DEBUG) << "Endpoint " << endpoint1
                   << " not found in outputs ports";
        return -1;
    }

    if (ep2OutputPortToken == AFT_NODE_TOKEN_NONE) {
        Log(DEBUG) << "Endpoint " << endpoint2
                   << " not found in outputs ports";
        return -1;
    }

    //
    // Send the new input port to the user sandbox
    //
    AftInsertPtr insert = AftInsert::create(_sandbox);
    AftNodePtr inputPort1, inputPort2;

    if (op == AfiClient::CC_ADD) {
        Log(DEBUG) << "Connecting port index " << ep1InputPortIndex
                   << " to output token " << ep2OutputPortToken ;
        inputPort1 = _sandbox->setInputPortByIndex(ep1InputPortIndex,
                                                   ep2OutputPortToken);
        //Log(DEBUG) << "Connecting port index " << ep2InputPortIndex
        //           << " to output token " << ep1OutputPortToken ;
        //inputPort2 = _sandbox->setInputPortByIndex(ep2InputPortIndex,
        //                                           ep1OutputPortToken);

    } else { //AfiClient::CC_DELETE
        Log(DEBUG) << "Connecting port index " << ep1InputPortIndex
                   << " to output token DISCARD";
        inputPort1 = _sandbox->setInputPortByIndex(ep1InputPortIndex,
                                                   AFT_NODE_TOKEN_DISCARD);
        Log(DEBUG) << "Connecting port index " << ep2InputPortIndex
                   << " to output token DISCARD";
        inputPort2 = _sandbox->setInputPortByIndex(ep2InputPortIndex,
                                                   AFT_NODE_TOKEN_DISCARD);
    }


    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 0;
    }

    insert->push(inputPort1);
    //insert->push(inputPort2);
    //_sandbox->send(insert);
    sendboxSend(insert);

    return 0;
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
AfiClient::setInputPortNextNode (AftIndex     inputPortIndex,
                                 AftNodeToken nextToken)
{
    AftInsertPtr insert = AftInsert::create(_sandbox);

    AftNodePtr  inputPort = _sandbox->setInputPortByIndex(inputPortIndex, nextToken);

    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 0;
    }

    insert->push(inputPort);

    //
    // Send the new input port to the user sandbox
    //
    //_sandbox->send(insert);
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
AfiClient::setIngressStart (AftNodeToken ingressStartToken)
{

    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 0;
    }

    AftPortTablePtr inputPorts = _sandbox->inputPortTable();

    for (AftIndex  i = 0 ; i < inputPorts->maxIndex();  i++) {
         setInputPortNextNode(i, ingressStartToken);
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
//     defaultTragetToken  Default route target token
// @return routing table node token
//

AftNodeToken
AfiClient::addTable (const std::string &tableName,
                     const std::string &fieldName,
                     AftNodeToken defaultTragetToken)
{
    AftNodeToken        tableNodeToken;
    AftNodeToken        discardNodeToken;
    AftInsertPtr        insert;


    std::cout << "AfiClient::addTable Adding table ... \n";

    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 10000;
    }

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    //
    // Create a route lookup tree
    //
    auto treePtr = AftTree::create(AftField(fieldName),
                                   defaultTragetToken);

    //
    // Stitch the optional params
    //
    uint32_t skipBits = 16;
    treePtr->setNodeParameter<std::string>("app", "NH");
    treePtr->setNodeParameter<std::string>("rt.nhType", "route");
    treePtr->setNodeParameter<uint32_t>("rt.skipBits", skipBits);


    tableNodeToken = insert->push(treePtr);
    std::cout << "AfiClient::addTable tableNodeToken:" << tableNodeToken << "\n";
    insert->push(tableName, tableNodeToken);

    //
    // Send all the nodes to the sandbox
    //
    //_sandbox->send(insert);
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
//     defaultTragetToken  Default route target token
// @return routing table node token
//

AftNodeToken
AfiClient::addRouteTable (const std::string &rttName,
                          AftNodeToken defaultTragetToken)
{
    AftNodeToken        rttNodeToken;
    AftNodeToken        discardNodeToken;
    AftInsertPtr        insert;

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    //
    // Create a route lookup tree
    //
    auto treePtr = AftTree::create(AftField("packet.lookupkey"),
                                   defaultTragetToken);

    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 0;
    }

    rttNodeToken = insert->push(treePtr);
    insert->push(rttName, rttNodeToken);

    //
    // Send all the nodes to the sandbox
    //
    //_sandbox->send(insert);
    sendboxSend(insert);

    return rttNodeToken;
}


int
AfiClient::addRoute (AftNodeToken  rttNodeToken,
                     const char *prefix_bytes,
                     int num_prefix_bytes,
                     int prefix_len,
                     AftNodeToken  routeTragetToken)
{
    AftInsertPtr        insert;
    AftNodeToken        outputPortToken;

    AftDataBytes aftdatabytes_prefix;
    for(int t = 0; t < num_prefix_bytes; ++t){
        uint8_t byte = prefix_bytes[t];
        aftdatabytes_prefix.push_back(byte);
    }


    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 0;
    }


    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    std::cout <<"Adding route ---> Node token " << routeTragetToken << std::endl;

    //
    // Create a route
    //
    // Ref: aftman/rt/rt-trio/src/RtEntryTrio.cpp
    AftDataPtr  daddr    = AftDataPrefix::create(aftdatabytes_prefix, prefix_len); // TBD: Check if 32 is fine
    AftEntryPtr entryPtr = AftEntryRoute::create(rttNodeToken, std::move(daddr), routeTragetToken, true);

    insert->push(entryPtr);

    //
    // Send all the nodes to the sandbox
    //
    //_sandbox->send(insert);
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
//     routeTragetToken Route target token
// @return 0 - Success, -1 - Error
//

int
AfiClient::addRoute (AftNodeToken       rttNodeToken,
                     const std::string &prefix,
                     AftNodeToken       routeTragetToken)
{
    AftInsertPtr        insert;
    AftNodeToken        outputPortToken;

    std::vector<std::string> prefix_sub_strings;
    boost::split(prefix_sub_strings, prefix, boost::is_any_of("./"));

    if (prefix_sub_strings.size() != 5) {
        std::cout << "Invalid prefix" << std::endl;
        return -1;
    }

    AftDataBytes aftdatabytes_prefix;
    for(int t = 0; t < (prefix_sub_strings.size() - 1); ++t){
        uint8_t byte = std::strtoul(prefix_sub_strings.at(t).c_str(), NULL, 0);
        aftdatabytes_prefix.push_back(byte);
    }

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    std::cout <<"Adding route ";
    std::cout << prefix << " ---> Node token " << routeTragetToken << std::endl;

    //
    // Create a route
    //
    // Ref: aftman/rt/rt-trio/src/RtEntryTrio.cpp
    AftDataPtr  daddr    = AftDataPrefix::create(aftdatabytes_prefix, 32); // TBD: Check if 32 is fine
    AftEntryPtr entryPtr = AftEntryRoute::create(rttNodeToken, std::move(daddr), routeTragetToken, true);

    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 0;
    }

    insert->push(entryPtr);

    //
    // Send all the nodes to the sandbox
    //
    //_sandbox->send(insert);
    sendboxSend(insert);

    return 0;
}

//
// @fn
// createIndexTable
//
// @brief
// Create index table
//
// @param[in]
//     field_name Index table lookup field name
// @param[in]
//     iTableSize Index table size
// @return Index table node token
//

AftNodeToken
AfiClient::createIndexTable (const std::string &field_name,
                             AftIndex           iTableSize)
{
    AftInsertPtr        insert;

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    AftNodePtr iTablePtr = AftTable::create(AftField(field_name), iTableSize,
                                            AFT_NODE_TOKEN_DISCARD);
    //iTablePtr->setNodeParameter("index.app", AftDataString::create("NH"));

    //AftNodeToken iTableToken = insert->push(iTablePtr, "IndexTable");

    //
    // Send all the nodes to the sandbox
    //
    _sandbox->send(insert);

    //return iTableToken;
    return 0;
}

//
// @fn
// addIndexTableEntry
//
// @brief
// Add entry into index table
//
// @param[in]
//     iTableToken Index table token
// @param[in]
//     entryIndex Index at which entry is to be added
// @param[in]
//     entryTargetToken Entry target token
// @return 0 - Success, -1 - Error
//

int
AfiClient::addIndexTableEntry (AftNodeToken iTableToken,
                               u_int32_t    entryIndex,
                               AftNodeToken entryTargetToken)
{
    AftInsertPtr        insert;

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

#if 0
    AftEntryPtr entry = AftEntry::create(iTableToken,
                                         entryIndex,
                                         entryTargetToken);

    insert->push(entry);
    std::cout << "Index table entry pushed. ";
    std::cout <<"(Index: " << entryIndex << " target token: "<< entryTargetToken << ")" << std::endl;

    //
    // Send all the nodes to the sandbox
    //
    _sandbox->send(insert);
#endif

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
AfiClient::createList (AftTokenVector tokVec)
{
    AftInsertPtr        insert;

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    //
    // Build a list of provided tokens
    //
    //AftNodePtr list = AftList::create(token1, token2);
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
AfiClient::getOuputPortToken(AftIndex outputPortIndex)
{
    AftNodeToken        outputPortToken;

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
AfiClient::addReceiveNode(uint32_t receiveCode, uint64_t context)
{
    //
    // Allocate an insert context
    //
    AftInsertPtr insert = AftInsert::create(_sandbox);

    //
    // Create aft encap node
    //
    auto aftReceivePtr = AftReceive::create(receiveCode, context);


    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 0;
    }

    AftNodeToken  nhReceiveToken = insert->push(aftReceivePtr);

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
AfiClient::addEtherEncapNode(const std::string &dst_mac,
                             const std::string &src_mac,
                             AftNodeToken       nextToken)
{


    if (_debugmode.find("no-afi-server") != std::string::npos) {
        return 0;
    }

    //
    // Allocate an insert context
    //
    AftInsertPtr insert = AftInsert::create(_sandbox);
    //
    // Create a key vector of ethernet data
    //

    AftKeyVector encapKeys = { AftKey(AftField("packet.ether.saddr"),
                                      AftDataEtherAddr::create(src_mac)),
                               AftKey(AftField("packet.ether.daddr"),
                                      AftDataEtherAddr::create(dst_mac)) };
    //
    // Create aft encap node
    //
    auto aftEncapPtr = AftEncap::create("ethernet", encapKeys);

    //
    // Set the List node token as next pointer to encap
    //
    aftEncapPtr->setNodeNext(nextToken);


    AftNodeToken  nhEncapToken = insert->push(aftEncapPtr);

    //
    // Send all the nodes to the sandbox
    //
    _sandbox->send(insert);

    return nhEncapToken;
}

//
// @fn
// addLabelEncap
//
// @brief
// Add label encap node
//
// @param[in]
//     outerLabelStr Outer label
// @param[in]
//     innerLabelStr Inner label
// @param[in]
//     nextToken Next node token
// @return Label encap node's token
//

AftNodeToken
AfiClient::addLabelEncap(const std::string &outerLabelStr,
                         const std::string &innerLabelStr,
                         AftNodeToken nextToken)
{

    uint64_t outerLabel = std::strtoull(outerLabelStr.c_str(),NULL,0);
    uint64_t innerLabel = std::strtoull(innerLabelStr.c_str(),NULL,0);
    AftNodeToken        listToken;
    AftInsertPtr        insert;
    AftNodeToken nhLabelEncapToken2;

    if (outerLabel == 0) {
        std::cout << "Outer label cannot be 0" << std::endl;
        return -1;
    }

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    //
    // Create a key vector of ethernet data
    //
    AftKeyVector encapKeys =
        { AftKey(AftField("packet.ether.saddr"),
                 AftDataEtherAddr::create("00:00:00:01:02:03")),
          AftKey(AftField("packet.ether.daddr"),
                 AftDataEtherAddr::create("00:00:00:04:05:06")) };

    //
    // Create aft encap node
    //
    AftEncap::Ptr aftEncapPtr = AftEncap::create("label", encapKeys);

    //
    // Set the label value
    //
    //aftEncapPtr->setNodeParameter("label.value",
    //                              AftDataInt::create(outerLabel));


    AftNodeToken nhLabelEncapToken = insert->push(aftEncapPtr);

    if (innerLabel != 0) {
        //
        // Create a key vector of ethernet data
        //
        AftKeyVector encapKeys2 =
            { AftKey(AftField("packet.ether.saddr"),
                     AftDataEtherAddr::create("00:00:00:01:02:03")),
              AftKey(AftField("packet.ether.daddr"),
                     AftDataEtherAddr::create("00:00:00:04:05:06")) };

        //
        // Create aft encap node
        //
        AftEncap::Ptr aftEncapPtr2 = AftEncap::create("label", encapKeys2);

        //aftEncapPtr2->setNodeParameter("label.value",
         //                              AftDataInt::create(innerLabel));


        nhLabelEncapToken2 = insert->push(aftEncapPtr2);
    }

    AftTokenVector tokVec;

    if (innerLabel != 0) {
        tokVec = {nhLabelEncapToken2, nhLabelEncapToken, nextToken};
    } else {
        tokVec = {nhLabelEncapToken, nextToken};
    }

    AftNodePtr nhList = AftList::create(tokVec);

    u_int64_t set_val = 1;
    //nhList->setNodeParameter("list.allocDesc",  AftDataInt::create(set_val));
    listToken = insert->push(nhList);

    //
    // Send all the nodes to the sandbox
    //
    _sandbox->send(insert);

    return listToken;
}

//
// @fn
// addLabelDecap
//
// @brief
// Add label decap node
//
// @param[in]
//     nextToken Next node token
// @return Label decap node's token
//

AftNodeToken
AfiClient::addLabelDecap(AftNodeToken nextToken)
{
    AftNodeToken        listToken;
    AftInsertPtr        insert;

    //
    // Allocate an insert context
    //
    insert = AftInsert::create(_sandbox);

    auto aftDecapPtr = AftDecap::create("label");

    //
    // Stich the optional params
    //
    static u_int64_t nhId = 1000; nhId++;
    //aftDecapPtr->setNodeParameter("meta.nhid", AftDataInt::create(nhId));

    //
    // Set the List node token as next pointer to encap
    //
    //aftDecapPtr->setNodeNext(nextToken);
    AftNodeToken  nhDecapToken = insert->push(aftDecapPtr);

    AftTokenVector tokVec = {nhDecapToken, nextToken};

    AftNodePtr list = AftList::create(tokVec);

    u_int64_t set_val = 1;
    //list->setNodeParameter("list.allocDesc",  AftDataInt::create(set_val));
    listToken = insert->push(list);

    //
    // Send all the nodes to the sandbox
    //
    _sandbox->send(insert);
    return listToken;
}

//
// @fn
// addDiscardNode
//
// @brief
// Add counter node
//
// @param[in] void
// @return Counter node token
//

AftNodeToken
AfiClient::addCounterNode (void)
{
    AftNodeToken        counterNodeToken;
    AftInsertPtr        insert;

    insert = AftInsert::create(_sandbox);

#if 0
    AftNodePtr counter =  AftCounter::create(0, 0, false);

    counterNodeToken = insert->push(counter, "Counter1");
    _sandbox->send(insert);

    return counterNodeToken;
#endif
    return 0;
}

//
// @fn
// addDiscardNode
//
// @brief
// Add discard node
//
// @param[in] void
// @return Discard node token
//

AftNodeToken
AfiClient::addDiscardNode (void)
{
    AftNodeToken        discardNodeToken;
    AftInsertPtr        insert;

    insert = AftInsert::create(_sandbox);
#if 0
    AftNodePtr discard = AftDiscard::create();
    discardNodeToken = insert->push(discard, "Discard");
    _sandbox->send(insert);

    return discardNodeToken;
#endif
    return 0;
}

