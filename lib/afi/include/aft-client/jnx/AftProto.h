//
/// @file   AftProto.h
/// @brief  AftProto class definition
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

#ifndef __AftProto__
#define __AftProto__

///
/// @addtogroup AftProto
/// @{
///

///
/// @class   AftProto
/// @brief   Simple class for describing packet (and other types) of Protos
///
class AftProto {
protected:
    AftProtoIndex _index; ///< Index of proto
    
public:
    //
    // Constructor and destructor
    //
    ///
    /// @brief                   Default constructor for protos
    /// @param [in] newIndex     Proto index
    ///
    AftProto(AftProtoIndex newIndex = 0): _index(newIndex) {};
        
    ~AftProto() {};
    
    //
    // Accessors
    //

    /// @returns Proto index
    uint32_t index() const { return _index; };
    
    ///
    /// @brief            Define << class operator to append description to an output stream
    /// @param [in] os    Reference to output stream to append to
    /// @param [in] proto Reference to proto to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftProto &proto) {
        os << " index:" << proto.index();
        return os;
    }
    
    ///
    /// @brief             Comparator for protos
    /// @param [in] proto  Reference to proto to compare against
    ///
    bool operator== (const AftProto &proto) const {
        return (_index == proto.index());
    }
};

///
/// @}
///

#endif
