//
// Juniper P4 Agent
//
/// @file  Utils.h
/// @brief Utilities
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

#ifndef __Utils__
#define __Utils__

#include <memory>
#include <iostream>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <boost/atomic.hpp>

class spinlock {
private:
  typedef enum {Locked, Unlocked} LockState;
  boost::atomic<LockState> state_;

public:
  spinlock() : state_(Unlocked) {}

  void lock()
  {
    while (state_.exchange(Locked, boost::memory_order_acquire) == Locked) {
      /* busy-wait */
    }
  }
  void unlock()
  {
    state_.store(Unlocked, boost::memory_order_release);
  }
};

int convertHexStringToBinary(const char* source, 
                             char* target_buff, 
                             int   target_buff_len);
void getRidOfSpacesFromString(char* source);
extern int  convertHexPktStrToPkt(char* hex_pkt_str, 
                                  char* pkt_buff, 
                                  int pkt_buff_len);
extern void pktTrace(const std::string &ctx, char *pkt, int pkt_len);


std::string base64_encode(char const* , unsigned int len);
int base64_decode(std::string const& encoded_string,
                  char *bytes, unsigned int len);

#endif // __Utils__
