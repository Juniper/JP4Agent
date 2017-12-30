//
// Juniper P4 Agent
//
/// @file  P4Info.h
/// @brief P4 Info
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

#ifndef __P4Info__
#define __P4Info__

#include <map>
#include "Base.h"
#include "Log.h"
#include "jnx/Aft.h"
#include "google/rpc/code.pb.h"
#include "p4runtime.grpc.pb.h"
#include "p4runtime.pb.h"
#include "p4config.pb.h"

class P4InfoResource;

using P4InfoResourceId  = uint32_t;                                ///< P4Info Resource Id
using P4InfoActionParamId = uint32_t;                              ///< TBD

using P4InfoResourcePtr = std::shared_ptr<P4InfoResource>;         ///< Pointer type of all P4Info resources
using P4InfoResourceWeakPtr = std::weak_ptr<P4InfoResource>;

using P4InfoResourceNameMap = std::map<std::string, P4InfoResourcePtr>;
using P4InfoResourceIdMap = std::map<uint32_t, P4InfoResourcePtr>;

class P4InfoResource {
    
protected:
    P4InfoResourceId _id;
    std::string      _name;  ///< Name
    std::string      _alias; ///< Alias
    
public:
    P4InfoResource (const P4InfoResourceId id,
                    const std::string &name,
                    const std::string &alias)
                   : _id(id),
                     _name(name),
                     _alias(alias) {};
    
    virtual ~P4InfoResource () {};
    
    /// @returns P4Info resource
    P4InfoResourceId id() const { return _id; };

    /// @returns P4Info resource name
    const std::string name()  const { return _name; };

    /// @returns P4Info resource alias
    const std::string alias()  const { return _alias; };


    virtual void display() = 0;

};

class P4InfoTable;
using P4InfoTablePtr = std::shared_ptr<P4InfoTable>;         ///< Pointer type of all P4Info resources
using P4InfoTableWeakPtr = std::weak_ptr<P4InfoTable>;

class P4InfoTable: public P4InfoResource {
public:
    P4InfoTable (const p4::config::Table &table) : 
        P4InfoResource(table.preamble().id(),
                       table.preamble().name(),
                       table.preamble().alias()),
        _table(table) {}
    ~P4InfoTable () {}

    void display() {
        const auto &pre = _table.preamble();
		std::cout << "_________ P4InfoTable _______" << std::endl;
		std::cout << "pre.id():" << pre.id() << std::endl;
		std::cout << "pre.name().c_str():" << pre.name().c_str() << std::endl;
		std::cout << "table.match_fields().size():" << _table.match_fields().size() << std::endl;
		std::cout << "table.action_refs().size():" << _table.action_refs().size() << std::endl;
		std::cout << "table.size():" << _table.size() << std::endl;
    }


    //  
    // Debug
    //  
    std::ostream &description (std::ostream &os) const;

    /// 
    /// @brief           Define << class operator to append description to an output stream
    /// @param [in] os   Reference to output stream to append description to
    /// @param [in] p4infotable Reference to p4infotable to generate description for
    /// @returns         Reference to output stream with text description of p4infotable appended
    /// 
    friend std::ostream &operator<< (std::ostream &os, const P4InfoTablePtr &p4infotable) {
        return p4infotable->description(os);
    }   

private:
    p4::config::Table _table;
    AftNodeToken      _token{AFT_NODE_TOKEN_NONE}; 
};


class P4InfoAction;
using P4InfoActionPtr = std::shared_ptr<P4InfoAction>;         ///< Pointer type of all P4Info actions
using P4InfoActionWeakPtr = std::weak_ptr<P4InfoAction>;


class P4InfoAction: public P4InfoResource {
public:
    P4InfoAction (const p4::config::Action &action) : 
        P4InfoResource(action.preamble().id(),
                       action.preamble().name(),
                       action.preamble().alias()),
        _action(action) {}
    ~P4InfoAction () {}

    void display() {
		std::cout << "_________ P4InfoAction _______" << std::endl;

        const auto &params = _action.params();
        for (const auto &param : params) {
            std::cout << "param id:" << param.id() << "\n";
            std::cout << "param name:" << param.name() << "\n";
            const auto &annotations = param.annotations();
            for (const auto &annotation : annotations) {
                std::cout << "annotation:" << annotation << "\n";
            }
            std::cout << "param bitwidth:" << param.bitwidth() << "\n";
        }
    }
	
    //  
    // Debug
    //  
    std::ostream &description (std::ostream &os) const;

    /// 
    /// @brief           Define << class operator to append description to an output stream
    /// @param [in] os   Reference to output stream to append description to
    /// @param [in] p4infoaction Reference to p4infoaction to generate description for
    /// @returns         Reference to output stream with text description of p4infoaction appended
    /// 
    friend std::ostream &operator<< (std::ostream &os, const P4InfoActionPtr &p4infoaction) {
        return p4infoaction->description(os);
    }   

    const std::string actionParamName(const P4InfoActionParamId paramId) {
        const auto &params = _action.params();
        for (const auto &param : params) {
            if (param.id() == paramId) {
                return param.name();
            }
        }
        //TBD: Handle not found case
    }

private:
    p4::config::Action _action;
};

class P4Info {

public:
    P4Info () {}
    ~P4Info () {}

    void insert2IdMap(const P4InfoResourcePtr &res)
    {
        _idMap[res->id()] = res;
    }

    void insert2NameMap(const P4InfoResourcePtr &res)
    {
        _nameMap[res->name()] = res;
    }

    const P4InfoResourcePtr p4InfoResource(P4InfoResourceId id)
    {
        return(_idMap[id]);
    }

    const P4InfoResourcePtr p4InfoResource(std::string &name)
    {
        return(_nameMap[name]);
    }

private:
    P4InfoResourceNameMap _nameMap;
    P4InfoResourceIdMap _idMap;
};

#endif // __P4Info__
