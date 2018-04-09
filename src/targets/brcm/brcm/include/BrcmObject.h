//
// Juniper P4 Agent
//
/// @file  BrcmObject.h
/// @brief Brcm Object Interface
//
// Created by Sudheendra Gopinath, March 2018
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


#ifndef __BrcmObject__
#define __BrcmObject__

#include "Afi.h"
#include <fstream>

using namespace std;

namespace BRCMHALP {

class BrcmObject;

using BrcmObjectPtr = std::shared_ptr<BrcmObject>;
using BrcmObjectWeakPtr = std::weak_ptr<BrcmObject>;

using BrcmObjectNameMap = std::map<std::string, BrcmObjectPtr>;
using BrcmObjectIdMap = std::map<uint32_t, BrcmObjectPtr>;

class BrcmObject
{
    
public:
    BrcmObject () {};
    virtual ~BrcmObject () {};
};

///
/// Forward decalaration
///
template<typename AfiObjType, typename BrcmObjType>
class BrcmObjectTemplate;

///
/// @class BrcmObjectTemplate
/// @brief Template base class for all Brcm Objects
///
template <typename AfiObjType, typename BrcmObjType>
class BrcmObjectTemplate: public BrcmObject, public AfiObjType {
public:
    ///
    /// Template share pointer alias
    ///
    using Ptr = std::shared_ptr<BrcmObjectTemplate<AfiObjType, BrcmObjType>>;

    ///
    /// Construction and destruction
    ///
    BrcmObjectTemplate<AfiObjType, BrcmObjType>(const AFIHAL::AfiJsonResource &res): AfiObjType(res) {}
    virtual ~BrcmObjectTemplate<AfiObjType, BrcmObjType>() {}

    ///
    /// @brief Utility creation function. This can be overriden by subclasses if required
    ///        to implement any node specific features
    ///
    /// @param [in] newBrcmObj   Brcm Object pointer
    ///
    /// @return Shared pointer to Brcm object
    ///
    std::shared_ptr<BrcmObjType> _create(const std::shared_ptr<BrcmObjType> &newBrcmObj)
    {
        return newBrcmObj;
    };

    ///
    /// @breif  Factory builder Brcm objects
    ///
    /// @param [in] res Json resource
    ///
    /// @return Shared pointer to Brcm object
    ///
    static std::shared_ptr<BrcmObjType> create(const AFIHAL::AfiJsonResource &res)
    {
        //
        // Create our object and return a shared pointer to it
        //
        std::shared_ptr<BrcmObjType> _newBrcmObj = std::make_shared<BrcmObjType>(res);

        //
        // See if there's any subclass specific noodling
        //
        return _newBrcmObj->_create(_newBrcmObj);
    }


    ///
    /// @brief  Default bind function for Brcm objects.
    ///         Derived classes can have specific implementation.
    ///
    virtual void _bind()
    {
         //return Brcmptr;
    }


    ///
    /// @brief  Bind function which creates hardware state
    ///
    virtual bool bind() override
    {
        //
        // Let the caller know whether it worked or not
        //
        std::cout << "BrcmObjectTemplate: bind" << std::endl;
        _bind();
        return true;
    }

    ///
    /// @berief  Destroy routine to destroy Brcm object
    ///
    void destroy ()
    {
        //
        // Release any hardware memory
        //
        std::cout << "BrcmObjectTemplate: destroy" << std::endl;
        this->unbind();
    }

    ///
    /// @brief  Unbind routine to uninstall hardware state
    ///
    virtual bool unbind() override
    {
        //
        // Free the state in the hardware
        //
        std::cout << "BrcmObjectTemplate: unbind" << std::endl;
        return true;
    }

    ///
    /// @brief  Debugging
    ///
    virtual std::ostream& description (std::ostream &os) const override
    {
        os << "BrcmObjectTemplate: description \n";
        return os;
    }

    ofstream gtestFile;
};

}  // namespace BRCMHALP

#endif // __BrcmObject__
