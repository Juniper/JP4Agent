//
// Juniper P4 Agent
//
/// @file  AftTree.h
/// @brief Aft Tree
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

#ifndef __AftTree__
#define __AftTree__

namespace AFTHALP {

class AftTree;
using AftTreePtr = std::shared_ptr<AftTree>;
using AftTreeWeakPtr = std::weak_ptr<AftTree>;

class AftTree: public AftObjectTemplate<AFIHAL::AfiTree, AftTree>
{
    using AftObjectTemplate::AftObjectTemplate;
public:
    //AftTree (const AfiJsonResource &jsonRes) : AfiTree(jsonRes) {}
    //~AftTree () {}

    /// 
    /// @brief  Create the JNH handler
    /// 
    /// @param [in] sandbox Em sandbox pointer which holds the JnhSandbox pointer
    /// 
    /// @return Jnh handle shared pointer
    /// 
    void _bind() override;
    
    //
    // Debug
    //
    std::ostream &description (std::ostream &os) const;
    
    friend std::ostream &operator<< (std::ostream &os,
                                     const AftTreePtr &AftTree) {
        return AftTree->description(os);
    }
    
    const AftNodeToken token() {
        return _token;
    }
    
    void setDefaultTragetToken(AftNodeToken token) {
        _defaultTragetToken = token;
    }
    
private:
    AftNodeToken               _defaultTragetToken{AFT_NODE_TOKEN_DISCARD};
    AftNodeToken               _token{AFT_NODE_TOKEN_NONE};
};

}  // namespace AFTHALP

#endif // __AftTree__
