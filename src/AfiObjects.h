//
// Juniper P4 Agent
//
/// @file  AfiObjects.h
/// @brief Afi Objects
//
// Created by Sandesh Kumar Sodhi, December 2017
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

#ifndef __AfiObjects__
#define __AfiObjects__

#include <map>
#include <jsoncpp/json/json.h>
#include "Base.h"
#include "Log.h"
#include "AfiDM.h"
#include "jnx/Aft.h"
#include "google/rpc/code.pb.h"
#include "p4runtime.grpc.pb.h"
#include "p4runtime.pb.h"
#include "p4config.pb.h"

class AfiObject;

using AfiObjectId  = int;                               ///< AFI object Id

using AfiObjectPtr = std::shared_ptr<AfiObject>;        ///< Pointer type of all AFI objects
using AfiObjectWeakPtr = std::weak_ptr<AfiObject>;

using AfiObjectNameMap = std::map<std::string, AfiObjectPtr>;
using AfiObjectIdMap = std::map<uint32_t, AfiObjectPtr>;

class AfiObject {
    
protected:
    AfiObjectId        _id;   ///< Object Id
    std::string      _name;   ///< Name
    
public:
    AfiObject (const AfiObjectId id,
                    const std::string &name)
                   : _id(id),
                     _name(name) {};
    
    virtual ~AfiObject () {};

    virtual AftNodeToken bind(void) = 0;
    //virtual bool update(void);
    //virtual bool change(void);
    
    /// @returns P4Info resource
    AfiObjectId id() const { return _id; };

    /// @returns P4Info resource name
    const std::string name()  const { return _name; };
};

class AfiTree;
using AfiTreePtr = std::shared_ptr<AfiTree>;         ///< Pointer type of all P4 trees
using AfiTreeWeakPtr = std::weak_ptr<AfiTree>;

class AfiTree: public AfiObject
{
public:
    AfiTree (const AfiObjectId id,
             const std::string &name,
             juniper::afi_tree::AfiTree &tree)
             : AfiObject(id, name),
               _tree(tree) {}

    ~AfiTree () {}

    AftNodeToken bind(void);

    //  
    // Debug
    //  
    std::ostream &description (std::ostream &os) const;

    friend std::ostream &operator<< (std::ostream &os, const AfiTreePtr &p4table) {
        return p4table->description(os);
    }   

    const AftNodeToken token() {
         return _token;
    }

    void setDefaultTragetToken(AftNodeToken token) {
        _defaultTragetToken = token;
    }

private:
    juniper::afi_tree::AfiTree _tree;
    AftNodeToken               _defaultTragetToken{AFT_NODE_TOKEN_DISCARD};
    AftNodeToken               _token{AFT_NODE_TOKEN_NONE}; 
};

class AfiDeviceCfg
{

public:
    AfiDeviceCfg () {}
    ~AfiDeviceCfg () {}

    void populateAfiObjects(const Json::Value &cfg_root);
    void bindAfiObjects();

    void insert2TreeMap(const AfiObjectPtr &obj)
    {
        std::cout << "insert2TreeMap... obj->name():"<< obj->name() << "\n";
        _treesMap[obj->name()] = obj;
    }


    const AfiObjectPtr getAfiTree(const std::string &name)
    {
        std::cout << "getAfiTree name:"<< name << "\n";
        return _treesMap[name];
    }

    //
    // Sets
    //
    void setIngressStartToken(AftNodeToken token) { 
        _ingressStartToken = token;
    }


    //
    // Accessors
    //
    AftNodeToken ingressStartToken() { return _ingressStartToken; }

private:
    AfiObjectNameMap _treesMap;
    AftNodeToken     _ingressStartToken{AFT_NODE_TOKEN_NONE}; 
};

#endif // __AfiObjects__
