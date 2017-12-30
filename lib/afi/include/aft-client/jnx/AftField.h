//
/// @file   AftField.h
/// @brief  AftField class definition
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

#ifndef __AftField__
#define __AftField__

///
/// @addtogroup AftFieldKey
/// @{
///

///
/// @class   AftField
/// @brief   Simple class for describing packet (and other types) of fields
///
class AftField {
protected:
    std::string  _name; ///< Name of field
    
public:
    //
    // Constructor
    //
    ///
    /// @brief                   Default constructor for fields
    /// @param [in] newName      Field name
    ///
    AftField(std::string newName): _name(newName) {};
    
    //
    // Accessors
    //
    /// @returns Field name
    const std::string name() const { return _name; };
    
    ///
    /// @brief            Define << class operator to append description to an output stream
    /// @param [in] os    Reference to output stream to append to
    /// @param [in] field Reference to field to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftField &field) {
        os << field.name();
        return os;
    }
    
    ///
    /// @brief             Comparator for fields
    /// @param [in] field  Reference to field to compare against
    ///
    bool operator== (const AftField &field) const {
        return (_name.compare(field.name()) == 0);
    }
};

typedef std::vector<AftField> AftFieldVector; ///< Typedef of std::vector used for all lists of fields

///
/// @}
///

#endif
