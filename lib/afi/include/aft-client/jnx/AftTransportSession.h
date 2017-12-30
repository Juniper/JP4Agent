//
//  AftTransportSession.h
//  Aft
//
/// @file  AftTransportSession.h
/// @brief AftTransport session class definitions
//
//  Created by Scott Mackie on 11/11/16.
//  Copyright (c) 2016 Juniper Networks Inc. All rights reserved.
//

#ifndef __AftTransportSession_h__
#define __AftTransportSession_h__

#include "jnx/AftTypes.h"
#include "jnx/AftOperation.h"

///
/// @addtogroup  AftTransport
///
/// @{
///

///
/// @class   AftTransportSession
/// @brief   Base class used for all Aft transport sessions
///
/// The AftTransportSession class is used primarily by the server. For every open sandbox client, an
/// AftTransportSession is usually created. This acts as a session marker for the server and is used
/// to provide endpoint management.
///
/// The receiver is set to point at the incoming operation stream receiver. This will usually be the
/// server's sandbox class.
///
/// When the sandbox client closes the sandbox, the session is ended and the AftTransportSession is
/// freed. In general, the sessions are assumed to be long-lived.
///
class AftTransportSession {
protected:
    std::string     _name;     ///< Name associated with the session
    AftReceiverPtr  _receiver; ///< Pointer to receiver used to handle events
    bool            _tracing;  ///< True if transport debug tracing is enabled
    
public:
    static AftTransportSessionPtr create(const std::string &newName) {
        return std::make_shared<AftTransportSession>(newName);
    }

    //
    // Constructor and destructor
    //
    AftTransportSession(const std::string &newName): _name(newName), _tracing(false) {};
    virtual ~AftTransportSession() {};

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
    
    ///
    /// @brief                    Send an unsolicted AftOperation back to the client
    /// @param [in] newOperation  Reference to the AftOperation
    ///
    virtual bool send (const AftOperationPtr &newOperation) {
        return false;
    }
        
    ///
    /// @brief   Called when the session is active
    ///
    virtual void start() {};

    ///
    /// @brief   Called when any resources associated with the session should be stopped
    ///
    virtual void stop() {};
    
    //
    // Accessors
    //
    
    ///
    /// @brief                   Set the event receiver for the session
    /// @param [in] newReceiver  Reference to the receiver to use for events
    ///
    void setReceiver(const AftReceiverPtr &newReceiver) { _receiver = newReceiver; };
    
    /// @returns Returns pointer to the event receiver
    AftReceiverPtr receiver() const { return _receiver; };
    
    ///
    /// @brief                      Enable or disable session tracing
    /// @param [in] tracingEnabled  True if session tracing should be enabled
    ///
    void setTracing(const bool tracingEnabled) { _tracing = tracingEnabled; };
    
    /// @returns True if session tracing is enabled
    bool tracing() const { return _tracing; };
    
    /// @returns Name of the sandbox that the sesson is associated with
    std::string name() const { return _name; };
};

///
/// @}
///

#endif
