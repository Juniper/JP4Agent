//
// Juniper P4 Agent
//
/// @file  NullObject.h
/// @brief Null Object
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


#ifndef __NullObject__
#define __NullObject__

#include "Afi.h"
#include <fstream>

using namespace std;

namespace NULLHALP {

class NullObject;
class AfiTree;       /// TBD: remove me

using NullObjectPtr = std::shared_ptr<NullObject>;
using NullObjectWeakPtr = std::weak_ptr<NullObject>;

using NullObjectNameMap = std::map<std::string, NullObjectPtr>;
using NullObjectIdMap = std::map<uint32_t, NullObjectPtr>;

class NullObject
{
    
public:
    NullObject () {};
    virtual ~NullObject () {};
};

///
/// Forward decalaration
///
template<typename AfiObjType, typename NullObjType>
class NullObjectTemplate;

///
/// @class NullObjectTemplate
/// @brief Template base class for all Null Objects
///
template <typename AfiObjType, typename NullObjType>
class NullObjectTemplate: public NullObject, public AfiObjType {
public:
    ///
    /// Template share pointer alias
    ///
    using Ptr = std::shared_ptr<NullObjectTemplate<AfiObjType, NullObjType>>;

    ///
    /// Construction and destruction
    ///
    NullObjectTemplate<AfiObjType, NullObjType>(const AFIHAL::AfiJsonResource &res): AfiObjType(res) {}
    virtual ~NullObjectTemplate<AfiObjType, NullObjType>() {}

    ///
    /// @brief Utility creation function. This can be overriden by subclasses if required
    ///        to implement any node specific features
    ///
    /// @param [in] newNullObj   Null Object pointer
    ///
    /// @return Shared pointer to EmTrio object
    ///
    std::shared_ptr<NullObjType> _create(const std::shared_ptr<NullObjType> &newNullObj)
    {
        return newNullObj;
    };

    ///
    /// @breif  Factory builder Null objects
    ///
    /// @param [in] res Json resource
    ///
    /// @return Shared pointer to Null object
    ///
    ///
    static std::shared_ptr<NullObjType> create(const AFIHAL::AfiJsonResource &res)
    {
        //
        // Create our object and return a shared pointer to it
        //
        std::shared_ptr<NullObjType> _newNullObj = std::make_shared<NullObjType>(res);

        //
        // See if there's any subclass specific noodling
        //
        return _newNullObj->_create(_newNullObj);
    }


    ///
    /// @brief  Default bind function for EmTrio class.i
    ///         Derived classes can have specific implementation.
    ///
    /// @param [in] sandbox  Em sandbox pointer
    ///
    /// @return Jnh handle pointer
    ///
    ///
    virtual void _bind()
    {
         //return nullptr;
    }


    ///
    /// @brief  Bind function which creates JNH handle
    ///
    /// @param [in] sandbox  Em sandbox pointer
    ///
    /// @return true, on successul bind
    ///
    virtual bool bind() override
    {

        //
        // Let the caller know whether it worked or not
        //
        std::cout << "NullObjectTemplate: bind" << std::endl;
        _bind();
        return true;
    }

    ///
    /// Destroy routine to uninstall JNH handle
    ///
    void destroy ()
    {
        //
        // Release any hardware memory
        //
        std::cout << "NullObjectTemplate: destroy" << std::endl;
        this->unbind();
    }

    ///
    /// Unbind routine to release JNH handle
    ///
    virtual bool unbind() override
    {
        //
        // Free the counter in the hardware
        //
        std::cout << "NullObjectTemplate: unbind" << std::endl;
        return true;
    }

    ///
    /// Debugging
    ///
    virtual std::ostream& description (std::ostream &os) const override
    {
        os << "NullObjectTemplate: description \n";
        return os;
    }

    ofstream gtestFile;
};

}  // namespace NULLHALP

#endif // __NullObject__
