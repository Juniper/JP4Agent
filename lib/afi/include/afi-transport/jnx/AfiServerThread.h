//
//  AfiServerThread.h
//  Aft - A Forwarding Toolkit
//
//  Basic Afi external server interface
//
//  Created by Scott Mackie on 9/1/17.
//  Copyright (c) 2017 Juniper Networks Inc. All rights reserved.
//

#ifndef __AfiServerThread__
#define __AfiServerThread__

#include "jnx/AftServer.h"
#include <mutex>

class AfiServer;
class AfiServerThread;
typedef std::shared_ptr<AfiServerThread> AfiServerThreadPtr;

class AfiServerThread {
protected:
    std::unique_ptr<AfiServer> _afiServer;      ///< Pointer to Afi server
    std::string                _serverAddress;  ///< URL string
    std::mutex                 _mutex;          ///< synchronize _afiServer

    void launch();
    void runloop();

public:
    //
    // Constructor and destructor
    //
    AfiServerThread(const AftServerPtr &server, const std::string newServerAddress);
    ~AfiServerThread();

    static AfiServerThreadPtr create(const AftServerPtr &server, const std::string newServerAddress);
   
    void shutdown();
};

#endif
