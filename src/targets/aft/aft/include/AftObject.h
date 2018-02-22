//
// Juniper P4 Agent
//
/// @file  AftObject.h
/// @brief Aft Object
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


#ifndef __AftObject__
#define __AftObject__

#include "Afi.h"

namespace AFTHALP {

class AftObject;
class AfiTree;       /// TBD: remove me

using AftObjectPtr = std::shared_ptr<AftObject>;
using AftObjectWeakPtr = std::weak_ptr<AftObject>;

using AftObjectNameMap = std::map<std::string, AftObjectPtr>;
using AftObjectIdMap = std::map<uint32_t, AftObjectPtr>;

class AftObject
{
    
public:
    AftObject () {};
    virtual ~AftObject () {};

};

///
/// Forward decalaration
///
template<typename AfiObjType, typename AftObjType>
class AftObjectTemplate;

///
/// @class AftObjectTemplate
/// @brief Template base class for all Aft Objects
///
template <typename AfiObjType, typename AftObjType>
class AftObjectTemplate: public AftObject, public AfiObjType {
public:
    ///
    /// Template share pointer alias
    ///
    using Ptr = std::shared_ptr<AftObjectTemplate<AfiObjType, AftObjType>>;

    ///
    /// Construction and destruction
    ///
    AftObjectTemplate<AfiObjType, AftObjType>(const AFIHAL::AfiJsonResource &res): AfiObjType(res) {}
    virtual ~AftObjectTemplate<AfiObjType, AftObjType>() {}

    ///
    /// @brief Utility creation function. This can be overriden by subclasses if required
    ///        to implement any node specific features
    ///
    /// @param [in] newAftObj   Aft Object pointer
    ///
    /// @return Shared pointer to EmTrio object
    ///
    std::shared_ptr<AftObjType> _create(const std::shared_ptr<AftObjType> &newAftObj)
    {
        return newAftObj;
    };

    ///
    /// @breif  Factory builder Aft objects
    ///
    /// @param [in] res Json resource
    ///
    /// @return Shared pointer to Aft object
    ///
    ///
    static std::shared_ptr<AftObjType> create(const AFIHAL::AfiJsonResource &res)
    {
        //
        // Create our object and return a shared pointer to it
        //
        std::shared_ptr<AftObjType> _newAftObj = std::make_shared<AftObjType>(res);

        //
        // See if there's any subclass specific noodling
        //
        return _newAftObj->_create(_newAftObj);
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
        std::cout << "AftObjectTemplate: bind" << std::endl;
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
        std::cout << "AftObjectTemplate: destroy" << std::endl;
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
        std::cout << "AftObjectTemplate: unbind" << std::endl;
        return true;
    }

    ///
    /// Debugging
    ///
    virtual std::ostream& description (std::ostream &os) const override
    {
        os << "AftObjectTemplate: description \n";
        return os;
    }

};

}  // namespace AFTHALP

#endif // __AftObject__
