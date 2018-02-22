//
// Juniper P4 Agent
//
/// @file  Main.cpp
/// @brief Juniper P4 Agent starts here...
//
// Created by Sandesh Kumar Sodhi, February 2018
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

#include <getopt.h>
#include <signal.h>
#include "JP4Agent.h"
//
// Defaults
//
const std::string defConfigFile("/root/JP4Agent/config/jp4agent-cfg.json");
const std::string _debugmode = "debug-afi-objects";

//
// Usage
//
void
displayUsage (void)
{
    std::cout << "\n\tUsage:\n";
    std::cout << "\tjp4agent OPTIONS\n";
    std::cout << "\tOPTIONS: \n";
    std::cout << "\t\t[-c <configuration-file>]\n";
    std::cout << "\t\t[-h\n\n";
    std::cout << "\tExamples:\n";
    std::cout << "\tjp4agent -c jp4agent-cfg.json\n\n";
}

//
// Handle Signal
//
void
handle_signal(int signum)
{
    if (signum == SIGINT) {
        std::cout << "\nJP4Agent does not quit on SIGINT\n";
        exit(1);
    } else if (signum == SIGUSR1) {
        std::cout << "\nGot SIGUSR1. Exiting gracefully...\n";
        exit(0);
    }
}

//
// JP4Agent main
//
int
main(int argc, char *argv[])
{
    Log(DEBUG) << "===== Juniper P4 Agent (Null target) =====";
    //
    // ctrl+ c should not terminate the program
    //
    signal(SIGINT, handle_signal);
    signal(SIGUSR1, handle_signal);

    std::string configFile = defConfigFile;

#define no_argument       0
#define required_argument 1
#define optional_argument 2

    const struct option longopts[] =
    {
      {"config", required_argument,  0, 'c'},
      {"help",   no_argument,        0, 'h'},
      {0,0,0,0},
    };

    int index;
    int iarg=0;

    opterr=1;
    while(iarg != -1)
    {
      iarg = getopt_long(argc, argv, "c:h", longopts, &index);

      switch (iarg)
      {
        case 'c':
          configFile = optarg;
          break;

        case 'h':
          displayUsage();
          exit(1);
          return 0;
      }
    }

    auto jP4Agent = std::make_unique<JP4Agent>(configFile);
    jP4Agent->init();

    //
    // Keep the main alive
    //
    while(1) { sleep(1000); }
    return 0;
}
