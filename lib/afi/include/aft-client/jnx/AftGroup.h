//
/// @file   AftGroup.h
/// @brief  AftGroup class definition
//
//  Copyright (c) Juniper Networks, Inc., [2017].
//
//  All rights reserved.
//
//
//  Third-Party Code: This code may depend on other components under separate
//  copyright notice and license terms.  Your use of the source code for those
//  components is subject to the terms and conditions of the respective license
//  as noted in the Third-Party source code file.
//
//

#ifndef __AftGroup__
#define __AftGroup__

///
/// @addtogroup AftGroup
/// @{
///

#define AFT_GROUP_INDEX_NONE 0 ///< If a node doesn't belong to a group, it has a zero index

///
/// @class   AftGroup
/// @brief   Simple class for describing packet (and other types) of Groups
///
class AftGroup {
protected:
    AftGroupIndex _index; ///< Index of Group
    
public:
    //
    // Constructor and destructor
    //
    ///
    /// @brief                   Default constructor for Groups
    /// @param [in] newIndex     Group index
    ///
    AftGroup(uint32_t newIndex = 0): _index(newIndex) {};
    
    ~AftGroup() {};
    
    //
    // Accessors
    //
    /// @returns Group index
    uint32_t index() const { return _index; };
    
    ///
    /// @brief            Define << class operator to append description to an output stream
    /// @param [in] os    Reference to output stream to append to
    /// @param [in] Group Reference to Group to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftGroup &Group) {
        os << "index:" << Group.index();
        return os;
    }
    
    ///
    /// @brief             Comparator for Groups
    /// @param [in] Group  Reference to Group to compare against
    ///
    bool operator== (const AftGroup &Group) const {
        return (_index == Group.index());
    }
};

///
/// @}
///

#endif
