//
// Juniper P4 Agent
//
/// @file  AfiCreator.cpp
/// @brief Afi Creator
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

#ifndef __AfiCreator__
#define __AfiCreator__

namespace AFIHAL {

template <class AfiRes, class AfiPtr>
class AfiCreator {
public:
    using AfiObjectCreator = std::function<AfiPtr(const AfiRes &res)>;
    using AfiObjectCreatorMap = std::map<std::string, AfiObjectCreator>;

protected:
    AfiObjectCreatorMap  _creatorMap;

public:
    //
    // Constructor and destructor
    //
    AfiCreator() {};
    ~AfiCreator() {};

    //
    // Set creator
    //
    void setCreator(const std::string type, AfiObjectCreator objectCreator) {
        _creatorMap[type] = objectCreator;
    };

    bool find(const std::string type, AfiObjectCreator &objectCreator)
    {
        typename AfiObjectCreatorMap::iterator it;

        it = _creatorMap.find(type);
        if (it != _creatorMap.end()) {
            objectCreator = it->second;
            return true;
        }

        return false;
    };

    AfiPtr create(const std::string type,
                  const AfiRes &res) {
        AfiObjectCreator objectCreator;

        if (find(type, objectCreator)) {
            return objectCreator(res);
        } else {
            Log(ERROR) << " objectCreator not found for " << type;
            return nullptr;
        }
    }
};

}  // namespace AFIHAL

#endif // __AfiCreator__
