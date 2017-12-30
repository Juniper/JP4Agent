//
/// @file   AftKey.h
/// @brief  AftKey class definition
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

#ifndef __AftKey__
#define __AftKey__

///
/// @addtogroup AftFieldKey Fields and Keys
/// @brief Classes used to define packet fields and associated field/data tuples expressed as keys
/// @{
///

///
/// @class   AftKey
/// @brief   Tuple class for associating AftField and AftData values
///
class AftKey {
protected:
    AftField    _field; ///< Field value used for key
    AftDataPtr  _data;  ///< Associated field data
    
    ///
    /// @brief                   Append data in key to the end of the supplied data vector
    /// @param [in] aData         Reference to data vector to append to
    /// @returns                 Length in bits of data appended
    ///
    uint32_t append (AftDataBytes &aData) {
        return _data->append(aData);
    }

public:
    //
    // Constructor
    //
    ///
    /// @brief                   Empty constructor for keys
    ///
    AftKey(): _field(AftField("unknown")), _data(nullptr) {};

    /// @brief                   Default constructor for keys
    /// @param [in] newField     Field value for key
    /// @param [in] newData      Reference to data value for key
    ///
    AftKey(const AftField &newField, const AftDataPtr &newData): _field(newField), _data(newData) {};

        
    //
    // Accessors
    //
    /// @returns Key field
    const AftField   field() const { return _field; };
    
    /// @returns Key data
    const AftDataPtr &data()  const { return _data; };
    
    ///
    /// @brief            Define << class operator to append description to an output stream
    /// @param [in] os    Reference to output stream to append to
    /// @param [in] aKey  Reference to key to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftKey &aKey) {
        os << "{ field:" << aKey.field();
        os << ", data:" << *aKey.data() << " }";
        return os;
    }
    
    ///
    /// @brief           Comparator for keys
    /// @param [in] key  Reference to key to compare against
    ///
    bool operator== (const AftKey &key) const {
        return ((_field == key.field()) &&
                (_data != nullptr) && (key.data() != nullptr) &&
                (*_data == *key.data()));
    }

    ///
    /// @brief             Concatanates key vector data into an AftDataPrefix
    /// @param [in] keys   Reference to key vector to concatanate
    /// Returns            Shared pointer to AftDataPrefix that contains concatenated result
    ///
    static AftDataPtr createDataPrefix (const AftKeyVector &keys) {
        AftDataBytes dataBytes;
        uint32_t     bitLength = 0;
        
        for (AftKey key: keys) {
            bitLength += key.append(dataBytes);
        }
        return AftDataPrefix::create(dataBytes, bitLength);
    }
    
    ///
    /// @brief                 Returns a pointer to the data for a key field (if it exists)
    /// @param [in] keys       Reference to key vector to search
    /// @param [in] fieldName  Reference to field name to search for
    /// Returns                Shared pointer to AftData class that matches field
    ///
    static AftDataPtr dataForField (const AftKeyVector &keys, const std::string &fieldName) {
        for (AftKey key: keys) {
            if (key._field.name() == fieldName) {
                return key._data;
            }
        }
        return nullptr;
    }

    ///
    /// @brief                 Returns a pointer to the data for a key field (if it exists)
    /// @param [in] keys       Reference to key vector to search
    /// @param [in] fieldName  Reference to field name to search for
    /// Returns                Shared pointer to AftData class that matches field
    ///
    template <class T>
    static std::shared_ptr<T> dataForField (const AftKeyVector &keys, const std::string &fieldName) {
        return std::static_pointer_cast<T>(AftKey::dataForField(keys, fieldName));
    }

    ///
    /// @brief                   Returns the nartual value for a key field (if it exists)
    /// @param [in]  keys        Reference to key vector to search
    /// @param [in]  fieldName   Reference to field name to search for
    /// @param [out] returnValue Reference to value to write
    /// Returns                  True if the field key is present
    ///
    template <typename T>
    static bool valueForField (const AftKeyVector &keys, const std::string &fieldName, T &returnValue) {
        AftDataPtr data = AftKey::dataForField(keys, fieldName);
        if (data == nullptr) return false;
        returnValue = data->value<T>();
        return true;
    }
};

//
// @}
//


#endif
