//
// Juniper P4 Agent
//
/// @file  NullTreeEntry.h
/// @brief Null tree entry
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

#ifndef SRC_TARGETS_NULL_NULL_INCLUDE_NULLTREEENTRY_H_
#define SRC_TARGETS_NULL_NULL_INCLUDE_NULLTREEENTRY_H_

#include <memory>
#include "NullObject.h"
#include "NullTree.h"

namespace NULLHALP
{
class NullTreeEntry;
using NullTreeEntryPtr     = std::shared_ptr<NullTreeEntry>;
using NullTreeEntryWeakPtr = std::weak_ptr<NullTreeEntry>;

class NullTreeEntry
    : public NullObjectTemplate<AFIHAL::AfiTreeEntry, NullTreeEntry>
{
    using NullObjectTemplate::NullObjectTemplate;

 public:
    // NullTreeEntry (const AfiJsonResource &jsonRes) : AfiTree(jsonRes) {}
    // ~NullTreeEntry () {}

    ///
    /// @brief  Create the JNH handler
    ///
    /// @param [in] sandbox Em sandbox pointer which holds the JnhSandbox
    /// pointer
    ///
    /// @return Jnh handle shared pointer
    ///
    void _bind() override;

    //
    // Debug
    //
    std::ostream &description(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &          os,
                                    const NullTreeEntryPtr &NullTreeEntry)
    {
        return NullTreeEntry->description(os);
    }
};

}  // namespace NULLHALP

#endif  // SRC_TARGETS_NULL_NULL_INCLUDE_NULLTREEENTRY_H_
