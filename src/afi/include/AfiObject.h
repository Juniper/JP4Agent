//
// Juniper P4 Agent
//
/// @file  AfiObject.h
/// @brief Afi Object base class
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

#ifndef __AfiObject__
#define __AfiObject__

#include "Afi.h"

namespace AFIHAL {

class AfiObject;

using AfiObjectPtr = std::shared_ptr<AfiObject>;
using AfiObjectWeakPtr = std::weak_ptr<AfiObject>;

using AfiObjectNameMap = std::map<std::string, AfiObjectPtr>;
using AfiObjectIdMap = std::map<uint32_t, AfiObjectPtr>;

class AfiObject : public AfiNext {
    
protected:
    AfiJsonResource  _jsonRes; ///< Json Resource
    
public:
    AfiObject (const AfiJsonResource &jsonRes) : _jsonRes(jsonRes) {}

    virtual ~AfiObject () {};

    virtual bool bind() = 0;
    virtual bool unbind() = 0;
    virtual std::ostream& description (std::ostream &os) const = 0;

    //virtual bool update(void);
    //virtual bool change(void);
    
    /// @returns AfiObject id
    AfiObjectId id() const { return _jsonRes.id(); };  /// TBD : revisit <<<<<< AfiObjectId ~~~ AfiJasonResourceId

    /// @returns AfiObject name
    const std::string name()  const { return _jsonRes.name(); };
    
    /// @returns AfiObject type
    const std::string type()  const { return _jsonRes.type(); };
    
    /// @returns AfiObject objStr
    const std::string objStr()  const { return _jsonRes.objStr(); };
};

}  // namespace AFIHAL

#endif // __AfiObject__
