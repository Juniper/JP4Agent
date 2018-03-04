//
// Juniper P4 Agent
//
/// @file  AfiJsonResource.h
/// @brief Afi Json Resorce
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

#ifndef SRC_AFI_INCLUDE_AFIJSONRESOURCE_H_
#define SRC_AFI_INCLUDE_AFIJSONRESOURCE_H_

#include <memory>
#include <string>
#include "AfiTypes.h"

namespace AFIHAL
{
class AfiJsonResource;

using AfiJsonResourcePtr     = std::shared_ptr<AfiJsonResource>;
using AfiJsonResourceWeakPtr = std::weak_ptr<AfiJsonResource>;

class AfiJsonResource
{
 protected:
    std::string       _type;    ///< Name
    AfiJsonResourceId _id;      ///< Object Id
    std::string       _name;    ///< Name
    std::string       _objStr;  ///< Name

 public:
    AfiJsonResource(const std::string &type, const AfiJsonResourceId id,
                    const std::string &name, const std::string &objStr)
        : _type(type), _id(id), _name(name), _objStr(objStr)
    {
    }

    //
    // Debug
    //
    std::ostream &description(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &            os,
                                    const AfiJsonResourcePtr &resource)
    {
        return resource->description(os);
    }

    /// @returns resource type
    const std::string type() const { return _type; }

    /// @returns resource id
    AfiJsonResourceId id() const { return _id; }

    /// @returns resource name
    const std::string name() const { return _name; }

    /// @returns resource object string
    const std::string objStr() const { return _objStr; }
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFIJSONRESOURCE_H_
