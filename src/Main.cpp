//
// Juniper P4 Agent
//
/// @file  Main.cpp
/// @brief Juniper P4 Agent starts here...
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

#include <getopt.h>
#include <signal.h>
#include "JP4Agent.h"

//
// Defaults
//
//const std::string _debugmode = "debug-afi-objects:no-afi-server";
const std::string _debugmode = "debug-afi-objects";
const std::string defAfiServerAddr   = "128.0.0.16:61162"; // grpc/tcp
const std::string defPIServerAddr    = "0.0.0.0:50051";    // grpc/tcp
const std::string defPktIOServerAddr = "128.0.0.16:64014"; // udp
const short jp4agent_hostpath_port  = 64015;               // udp

//
// Forward declarations
//
void handle_signal(int signum);

//
// Usage
//
void
displayUsage (void)
{
    std::cout << std::endl;
    std::cout << "\tUsage:\n";
    std::cout << "\tjp4agent OPTIONS\n";
    std::cout << "\tOPTIONS: \n";
    std::cout << "\t\t[-a <afi-server-address>]\n";

    std::cout << "\t\t    <afi-server-address>  : AFI Server Address\n";
    std::cout << "\t\t[-l <pi-listen-address>]\n";
    std::cout << "\t\t    <pi-listen-addresss> : \n";
    std::cout << "\t\t    Address where JP4Agent start PI server \n";
    std::cout << "\t\t[-p <pktIO-listen-address>]\n";
    std::cout << "\t\t    Address where JP4Agent listens for packets from PktIO\n";
    std::cout << std::endl;
    std::cout << "\tExamples:\n";
    std::cout << "\tjp4agent\n";
    std::cout << "\tjp4agent -a 128.0.0.16:61162 \\ \n";
    std::cout << "\t         -l 0.0.0.0:50051  \\ \n";
    std::cout << "\t         -p 128.0.0.100:64015 \\ \n";
    std::cout << "\t         -d debug-afi-objects:no-afi-server\n";
    std::cout << std::endl;
}

//
// JP4Agent main
//
int
main(int argc, char *argv[])
{
    std::string afiServerAddr = defAfiServerAddr;;
    std::string piListenAddr  = defPIServerAddr;
    std::string pktIOListenAddr  = defPktIOServerAddr;
    Log(DEBUG) << "===== Juniper P4 Agent =====";

#define no_argument       0
#define required_argument 1
#define optional_argument 2

    const struct option longopts[] =
    {
      {"afi",       required_argument,  0, 'a'},
      {"debugmode", required_argument,  0, 'd'},
      {"help",      no_argument,        0, 'h'},
      {"listen",    required_argument,  0, 'l'},
      {"pktIO",     required_argument,  0, 'p'},
      {0,0,0,0},
    };

    int index;
    int iarg=0;

    opterr=1;
    while(iarg != -1)
    {
      iarg = getopt_long(argc, argv, "a:hl:p:", longopts, &index);

      switch (iarg)
      {
        case 'h':
          displayUsage();
          exit(1);
          return 0;

        case 'a':
          afiServerAddr = optarg;
          break;

        case 'l':
          piListenAddr = optarg;
          break;

        case 'p':
          pktIOListenAddr = optarg;
          break;
      }
    }

    Log(DEBUG) << "afiServerAddr   : "<< afiServerAddr;
    Log(DEBUG) << "piListenAddr    : "<< piListenAddr;
    Log(DEBUG) << "pktIOListenAddr : "<< pktIOListenAddr;

    boost::asio::io_service ioService;

    jP4Agent = new JP4Agent(ioService, afiServerAddr, piListenAddr,
                            jp4agent_hostpath_port, pktIOListenAddr);

    jP4Agent->startAfiClient();
    jP4Agent->startAfiPktRcvr();
    jP4Agent->startPIServer();

    //
    // ctrl+ c should not terminate the program
    //
    signal(SIGINT, handle_signal);
    jP4Agent->cli();

    return 0;
}

//
// Handle Signal
//
void
handle_signal(int signum)
{
    if (signum == SIGINT) {
        std::cout << "\nPlease use quit command to exit\n";
    }
}
