//
//  AfiTransportSession.h
//  Aft - A Forwarding Toolkit
//
//  Server implementation of Afi transport
//
//  Created by Scott Mackie on 6/1//16.
//  Copyright (c) 2016 Juniper Networks Inc. All rights reserved.
//

#ifndef __AfiTransportSession__
#define __AfiTransportSession__

#include "jnx/AftServer.h"
#include "jnx/AftTransportSession.h"
#include "jnx/AftWorkQueue.h"

class AfiTransportSession;
typedef std::shared_ptr<AfiTransportSession> AfiTransportSessionPtr;
typedef std::weak_ptr<AfiTransportSession>   AfiTransportSessionWeakPtr;

class AfiTransportSessionThread: public net::juniper::workqueue::WorkerPeerThread {
public:
    typedef std::unique_ptr<AfiTransportSessionThread> UPtr;  ///< Pointer type for transport worker
    
    using WorkerPeerThread::WorkerPeerThread;
};

class AfiTransportSession: public AftTransportSession {
protected:
    AftServerPtr                    _server;    ///< Pointer to server
    AftWorkQueuePtr                 _workqueue; ///< Workqueue to sandbox thread
    AfiTransportSessionThread::UPtr _thread;    ///< Sandbox Session thread
    
public:
    //
    // Constructor and destructor
    //
    AfiTransportSession(const std::string &newName): AftTransportSession(newName) {};
    ~AfiTransportSession() {};

    static AfiTransportSessionPtr create(const std::string &newName);
    static AfiTransportSessionPtr create(const std::string &newName, const AftServerPtr &server);
;
    static AfiTransportSessionPtr downcast(const AftTransportSessionPtr &session) {
        return std::static_pointer_cast<AfiTransportSession>(session);
    }
   
    //
    // Accessors
    //
    void setServer(const AftServerPtr &newServer) { _server = newServer; };
    AftServerPtr server() const { return _server; };
    bool isServer() const { return _server != nullptr; };
    AftWorkQueuePtr workqueue() const { return _workqueue; };
    
    ///
    /// @brief                    Receive an incoming AftOperation for the server
    /// @param [in] newOperation  Reference to the AftOperation
    ///
    virtual bool receive(const AftOperationPtr &newOperation) {
        //
        // Simply execute the operation for the receiver
        //
        if (_receiver != nullptr) {
            return newOperation->execute(_receiver, newOperation);
        } else {
            return false;
        }
    };
    
    virtual void start() {
        //
        // Create the work queue
        //
        _workqueue = AftWorkQueue::create(_name, _receiver, true);
        
        //
        // Create our session thread
        //
        _thread = std::make_unique<AfiTransportSessionThread>(_workqueue->serverPeer(), false);
        
        _thread->Start(false);
    };
    
    virtual void stop() {
        _thread->Stop();
        _thread->Join();
    }
};

#endif
