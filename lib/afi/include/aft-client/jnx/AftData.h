//
/// @file   AftData.h
/// @brief  AftData class definition and related dependent classes
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

#ifndef __AftData__
#define __AftData__

#include <arpa/inet.h>
#include <boost/variant.hpp>

//
// Handle differences for OS X vs linux build
//

#ifdef __APPLE__

//
// We need this for ether_aton()
//

#include <net/ethernet.h>

#else

//
// We need this for ether_aton()
//
#include <netinet/ether.h>

//
// Linux doesn't have htonll yet
//
#include <sys/param.h>

inline uint64_t htonll (uint64_t n) {
#if __BYTE_ORDER == __BIG_ENDIAN
    return n;
#else
    return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
#endif
}

#endif

///
/// @defgroup AftData   Data
/// @brief Classes of basic data encapsulations that allow abstraction and re-use across the Aft interface
///
/// @details AftData implements a wrapper class that allows data to be encapsulated with some state as to
/// what the underlying data actually *is* (whether it's an index, an IP address or an MPLS label).
/// This allows us to do two things
///
/// - Abstract data references in the Aft client (and server)
/// - Add context to allow additional error checking on the client and the server
///
/// Obviously, we trade off some memory footprint for the extra context but this allows us to provide a rich
/// interface for lookup key management, for example.
///
/// @{
///

#define AFT_BITS_IN_BYTE 8  ///< Basic definition to avoid playing whack-a-mole with includes @hideinitializer

///
/// @typedef  AftDataBytes
/// @brief    Vector of 8 bit values for data buffers
///
typedef std::vector<uint8_t> AftDataBytes;

class AftData;
class AftDataBool;
class AftDataInt;
class AftDataString;
typedef std::shared_ptr<AftData> AftDataPtr; ///< Pointer type for all AftData objects
typedef boost::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t, std::string, AftDataBytes> AftDataValue;

///
/// @class   AftData
/// @brief   Base class for all Aft data classes
///
class AftData {
protected:
    AftDataValue _value;     ///< Variant of all the possible types
    uint32_t     _bitLength; ///< Size of encapsulated data in bits
    
    ///
    /// @brief                   Base constructor for data objects
    /// @param [in] newBitLength Length in bits of data
    ///
    AftData(const bool         &newValue, uint32_t newBitLength): _value(newValue), _bitLength(newBitLength) {};
    AftData(const uint8_t      &newValue, uint32_t newBitLength): _value(newValue), _bitLength(newBitLength) {};
    AftData(const uint16_t     &newValue, uint32_t newBitLength): _value(newValue), _bitLength(newBitLength) {};
    AftData(const uint32_t     &newValue, uint32_t newBitLength): _value(newValue), _bitLength(newBitLength) {};
    AftData(const uint64_t     &newValue, uint32_t newBitLength): _value(newValue), _bitLength(newBitLength) {};
    AftData(const std::string  &newValue, uint32_t newBitLength): _value(newValue), _bitLength(newBitLength) {};
    AftData(const AftDataBytes &newValue, uint32_t newBitLength): _value(newValue), _bitLength(newBitLength) {};
    
public:
    ///
    /// @brief           Convenience downcast template. Use this instead of manual casting if required
    /// @param [in] data Data pointer to downcast
    ///
    template <class T>
    static std::shared_ptr<T> downcast(const AftDataPtr &data) {
        return std::static_pointer_cast<T>(data);
    }

    AftData(): _bitLength(0) {};
    
    ///
    /// @brief                  Factory generator for general 8 bit scalar value
    /// @param [in] newValue    Scalar value to represent as data
    ///
    static std::shared_ptr<AftDataBool> create(const bool &newValue) {
        return std::make_shared<AftDataBool>(newValue);
    }

    ///
    /// @brief                  Factory generator for general 8 bit scalar value
    /// @param [in] newValue    Scalar value to represent as data
    ///
    static std::shared_ptr<AftDataInt> create(const uint8_t &newValue) {
        return std::make_shared<AftDataInt>(newValue);
    }
    
    ///
    /// @brief                  Factory generator for general 16 bit scalar value
    /// @param [in] newValue    Scalar value to represent as data
    ///
    static std::shared_ptr<AftDataInt> create(const uint16_t &newValue) {
        return std::make_shared<AftDataInt>(newValue);
    }
    
    ///
    /// @brief                  Factory generator for general 32 bit scalar value
    /// @param [in] newValue    Scalar value to represent as data
    ///
    static std::shared_ptr<AftDataInt> create(const uint32_t &newValue) {
        return std::make_shared<AftDataInt>(newValue);
    }
    
    ///
    /// @brief                  Factory generator for general 64 bit scalar value
    /// @param [in] newValue    Scalar value to represent as data
    ///
    static std::shared_ptr<AftDataInt> create(const uint64_t &newValue) {
        return std::make_shared<AftDataInt>(newValue);
    }
    
    ///
    /// @brief                  Factory generator for general scalar value
    /// @param [in] newValue    Scalar value to represent as data
    ///
    static std::shared_ptr<AftDataInt> create(const uint64_t &newValue, uint32_t newBitLength) {
        return std::make_shared<AftDataInt>(newValue, newBitLength);
    }

    ///
    /// @brief                   Factory generator for string data
    /// @param [in] newString    Reference to string to use for data
    ///
    static std::shared_ptr<AftDataString> create(const std::string &newString) {
        return std::make_shared<AftDataString>(newString);
    }

    //
    // Accessors
    //
    /// @returns Length in bits of data
    uint32_t bitLength()  const { return _bitLength; };
 
    /// @returns Length in bytes of data
    uint32_t byteLength() const { return (_bitLength + AFT_BITS_IN_BYTE - 1) / AFT_BITS_IN_BYTE; };
    
    // @returns value with templatized class
    template <class T>
    T value() {
        return boost::get<T>(_value);
    }

    ///
    /// @brief            Define << class operator to append description to an output stream
    /// @param [in] os    Reference to output stream to append to
    /// @param [in] aData Reference to data object to append description of to stream
    ///
    friend std::ostream &operator<< (std::ostream &os, const AftData &aData) {
        return aData.description(os);
    }
 
    ///
    /// @brief            Comparator for data
    /// @param [in] aData Reference to data to compare against
    ///
    bool operator== (const AftData &aData) const {
        return compare(aData) == 0;
    }
    
    /// @returns Reference to base of prefix as uint8_t pointer
    virtual const uint8_t *dataArray() const { return nullptr; };
    
    ///
    /// @brief              Append contents of data object to AftDataBytes vector
    /// @param [in] newData Reference to AftDataBytes vector to append to
    ///
    virtual uint32_t append(AftDataBytes &newData) { return 0; };

    /// @returns String description of data type
    virtual const std::string dataClass() const { return "AftData"; };

    ///
    /// @brief Append a description of the data to an output stream
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of data appended
    ///
    virtual std::ostream &description (std::ostream &os) const {
        os << "No Data";
        return os;
    }
    
    ///
    /// @brief Compare data
    /// @param [in] aData Reference to data to compare against
    /// @returns Zero if data is the same
    ///
    virtual int compare (const AftData &aData) const {
        if ((_bitLength == aData._bitLength) && (_value == aData._value)) return 0;
        return (_value < aData._value) ? -1 : 1;
    }
};

///
/// @class   AftDataBool
/// @brief   Data class for bools
///
class AftDataBool: public AftData {
public:
    ///
    /// @brief                   Constructor for general scalar value
    /// @param [in] newValue     Scalar value to represent as data
    ///
    AftDataBool(const bool     &newValue): AftData(newValue, 1) {};
    
    //
    // Virtual functions
    //
    virtual uint32_t append(AftDataBytes &aData) {
        bool boolValue = boost::get<bool>(_value);
        aData.push_back(boolValue);
        return sizeof(uint8_t);
    };
    virtual const std::string dataClass() const { return "AftDataBool"; };
    virtual std::ostream &description (std::ostream &os) const {
        bool boolValue = boost::get<bool>(_value);
        os << ((boolValue) ? "true" : "false");
        return os;
    }
};

///
/// @class   AftDataInt
/// @brief   Data class for all integers
///
class AftDataInt: public AftData {
public:
    ///
    /// @brief                   Constructor for general scalar value
    /// @param [in] newValue     Scalar value to represent as data
    ///
    AftDataInt(const uint8_t  &newValue): AftData(newValue, 8) {};
    AftDataInt(const uint16_t &newValue): AftData(newValue, 16) {};
    AftDataInt(const uint32_t &newValue): AftData(newValue, 32) {};
    AftDataInt(const uint64_t &newValue): AftData(newValue, 64) {};

    ///
    /// @brief                   Constructor for general scalar value
    /// @param [in] newValue     Scalar value to represent as data
    /// @param [in] newBitLength Bitlength of the data
    ///
    AftDataInt(const uint64_t &newValue, uint32_t newBitLength): AftData(newValue, newBitLength) {};
    
    //
    // Virtual functions
    //
    virtual uint32_t append(AftDataBytes &aData) {
        uint32_t count;
        uint32_t max = byteLength();
        uint64_t rawValue, intValue;
        
        //
        // The joy of byteorder. As we treat prefixes as the sequence of
        // bytes you'd see on the wire, they're network byte order. Make
        // sure that as we generate a prefix, we do the same.
        //
        switch (_bitLength) {
            default:
            case 8:  rawValue = boost::get<uint8_t>(_value);  intValue = rawValue; break;
            case 16: rawValue = boost::get<uint16_t>(_value); intValue = htons(rawValue);  break;
            case 32: rawValue = boost::get<uint32_t>(_value); intValue = htonl(rawValue);  break;
            case 64: rawValue = boost::get<uint64_t>(_value); intValue = htonll(rawValue); break;
        }
        
        for (count = 0; count < max; count++) {
            uint8_t byte = intValue & 0xFF;
            aData.push_back(byte);
            intValue >>= 8;
        }
        return _bitLength;
    };
    virtual const std::string dataClass() const { return "AftDataInt"; };
    virtual std::ostream &description (std::ostream &os) const {
        switch (_bitLength) {
            default:
            case 8:  os << boost::get<uint8_t>(_value); break;
            case 16: os << boost::get<uint16_t>(_value); break;
            case 32: os << boost::get<uint32_t>(_value); break;
            case 64: os << boost::get<uint64_t>(_value); break;
        }
        
        return os;
    }
};

///
/// @class   AftDataString
/// @brief   Data class for strings
///
class AftDataString: public AftData {
public:
    static uint32_t getBitLength(const std::string &newString) { return (uint32_t)(newString.length() * AFT_BITS_IN_BYTE); };
    
    ///
    /// @brief                   Constructor for string data
    /// @param [in] newString    Reference to string to use for data
    ///
    AftDataString(const std::string &newString): AftData(newString, AftDataString::getBitLength(newString)) {};
    
    //
    // Accessors
    //
    
    /// @returns Reference to base of prefix as uint8_t pointer
    virtual const uint8_t *dataArray() const { return (const uint8_t *)(boost::get<std::string>(_value).c_str()); };

    //
    // Virtual functions
    //
    virtual uint32_t append(AftDataBytes &aData) {
        const std::string &data = boost::get<const std::string &>(_value);
        aData.insert(aData.end(), data.begin(), data.end() );
        return _bitLength;
    };
    virtual const std::string dataClass() const { return "AftDataString"; };
    virtual std::ostream &description (std::ostream &os) const {
        const std::string &data = boost::get<const std::string &>(_value);
        for (uint8_t aValue: data)
            os << aValue << ".";
        os << "/" << _bitLength;
        return os;
    }
};

///
/// @class   AftDataPrefix
/// @brief   Data class for byte-array prefixes
///
class AftDataPrefix: public AftData {
public:
    typedef std::shared_ptr<AftDataPrefix> Ptr; ///< Pointer type for all AftData ethernet addresses

    ///
    /// @brief Factory generator for zero length prefix
    ///
    static std::shared_ptr<AftDataPrefix> create() {
        return std::make_shared<AftDataPrefix>(AftDataBytes(), 0);
    }

    ///
    /// @brief                   Factory generator for generic byte buffer data
    /// @param [in] newData      Vector of bytes to repesent as data
    /// @param [in] newBitLength Length in bits of data
    ///
    static std::shared_ptr<AftDataPrefix> create(AftDataBytes newData, uint32_t newBitLength) {
        return std::make_shared<AftDataPrefix>(newData, newBitLength);
    }

    ///
    /// @brief                   Factory generator for generic byte buffer data
    /// @param [in] newData      Pointer to uint8_t data to represent as data
    /// @param [in] newBitLength Length in bits of data
    ///
    static std::shared_ptr<AftDataPrefix> create(uint8_t *newData, uint32_t newBitLength) {
        return std::make_shared<AftDataPrefix>(newData, newBitLength);
    }

    ///
    /// @brief                   Factory generator for generic byte buffer data
    /// @param [in] newData      Pointer to const uint8_t data to represent as data
    /// @param [in] newBitLength Length in bits of data
    ///
    static std::shared_ptr<AftDataPrefix> create(const uint8_t *newData, uint32_t newBitLength) {
        return std::make_shared<AftDataPrefix>(newData, newBitLength);
    }

    ///
    /// @brief                   Constructor for generic byte buffer data
    /// @param [in] newData      Vector of bytes to repesent as data
    /// @param [in] newBitLength Length in bits of data
    ///
    AftDataPrefix(AftDataBytes newData, uint32_t newBitLength): AftData(newData, newBitLength) {};

    ///
    /// @brief                   Constructor for generic byte buffer data
    /// @param [in] newData      Pointer to uint8_t data to represent as data
    /// @param [in] newBitLength Length in bits of data
    ///
    AftDataPrefix(uint8_t *newData, uint32_t newBitLength): AftData() {
        //
        // Convenience constructor for arbitrary byte buffers
        //
        AftDataBytes data;
        _bitLength = newBitLength;
        data.assign(newData, newData + byteLength());
        _value     = data;
    }

    ///
    /// @brief                   Constructor for generic byte buffer data
    /// @param [in] newData      Pointer to const uint8_t data to represent as data
    /// @param [in] newBitLength Length in bits of data
    ///
    AftDataPrefix(const uint8_t *newData, uint32_t newBitLength): AftData() {
        //
        // Convenience constructor for arbitrary byte buffers
        //
        AftDataBytes data;
        _bitLength = newBitLength;
        data.assign(newData, newData + byteLength());
        _value     = data;
    }
    
    //
    // Virtual functions
    //

    /// @returns Reference to base of prefix as uint8_t pointer
    virtual const uint8_t *dataArray() const { return boost::get<const AftDataBytes &>(_value).data(); };
    
    virtual uint32_t append(AftDataBytes &aData) {
        const AftDataBytes &data = boost::get<const AftDataBytes &>(_value);
        aData.insert(aData.end(), data.begin(), data.end() );
        return _bitLength;
    };
    virtual const std::string dataClass() const { return "AftDataPrefix"; };
    virtual std::ostream &description (std::ostream &os) const {
        const AftDataBytes &data = boost::get<const AftDataBytes &>(_value);
        for (uint8_t aValue: data)
            os << (int)aValue << ".";
        os << "/" << _bitLength;
        return os;
    }
};

///
/// @class   AftDataIP4Addr
/// @brief   Data class for IPv4 addresses
///
class AftDataIP4Addr: public AftData {
public:
    ///
    /// @brief                   Factory generator for IPv4 address
    /// @param [in] newAddress   IPv4 address as a C-style byte array to represent in data
    /// @param [in] newBitLength Length of IPv4 address in bits (i.e. subnet length)
    ///
    static std::shared_ptr<AftDataIP4Addr> create(uint8_t *newAddress, uint32_t newBitLength) {
        if (newBitLength > (sizeof(uint32_t) * AFT_BITS_IN_BYTE)) {
            return nullptr;
        }
        return std::make_shared<AftDataIP4Addr>(newAddress, newBitLength);
    }

    ///
    /// @brief                   Factory generator for IPv4 address
    /// @param [in] newAddress   IPv4 address to represent in data
    /// @param [in] newBitLength Length of IPv4 address in bits (i.e. subnet length)
    ///
    static std::shared_ptr<AftDataIP4Addr> create(const uint32_t &newAddress, uint32_t newBitLength) {
        return std::make_shared<AftDataIP4Addr>(newAddress, newBitLength);
    }
    
    ///
    /// @brief                   Factory generator for IPv4 address
    /// @param [in] prefixString IPv4 address as a dot-notation string to represent in data
    ///
    static std::shared_ptr<AftDataIP4Addr> create(std::string prefixString) {
        return std::make_shared<AftDataIP4Addr>(prefixString);
    }
    
    //
    // Constructors and destructor
    //
    AftDataIP4Addr(): AftData() {};

    ///
    /// @brief                   Constructor for IPv4 address
    /// @param [in] newAddress   IPv4 address as a C-style byte array to represent in data
    /// @param [in] newBitLength Length of IPv4 address in bits (i.e. subnet length)
    ///
    AftDataIP4Addr(uint8_t *newAddress, uint32_t newBitLength): AftData() {
        //
        // Convenience constructor for arbitrary byte buffers
        //
        AftDataBytes data;
        _bitLength = newBitLength;
        data.assign(newAddress, newAddress + byteLength());
        _value     = data;
    }

    ///
    /// @brief                   Constructor for IPv4 address
    /// @param [in] newAddress   IPv4 address to represent in data
    /// @param [in] newBitLength Length of IPv4 address in bits (i.e. subnet length)
    ///
    AftDataIP4Addr(const uint32_t &newAddress, uint32_t newBitLength): AftData() {
        AftDataBytes aData;
        uint32_t addr = newAddress;

        _bitLength = newBitLength;
        for (uint32_t count = 0; count < byteLength(); count++) {
            uint8_t byte = addr & 0xFF;
            aData.push_back(byte);
            addr >>= 8;
        }
        _value = aData;
    }
    
    ///
    /// @brief                   Constructor for IPv4 address
    /// @param [in] prefixString IPv4 address as a dot-notation string to represent in data
    ///
    AftDataIP4Addr(std::string prefixString): AftData() {
        AftDataBytes aData(4);
        
        //
        // Cheap'n'nasty hack to build key prefixes from text strings for debug purposes
        //
        inet_pton(AF_INET, prefixString.c_str(), &aData[0]);
        _bitLength = sizeof(uint32_t) * AFT_BITS_IN_BYTE;
        _value     = aData;
    };
    
    /// @returns Reference to base of prefix as uint8_t pointer
    virtual const uint8_t *dataArray() const { return boost::get<AftDataBytes>(_value).data(); };

    //
    // Virtual functions
    //
    virtual uint32_t append(AftDataBytes &aData) {
        const AftDataBytes &data = boost::get<const AftDataBytes &>(_value);
        aData.insert(aData.end(), data.begin(), data.end() );
        return _bitLength;
    };

    virtual const std::string dataClass() const { return "AftDataIP4Addr"; };
    virtual std::ostream &description (std::ostream &os) const {
        char addressString[INET_ADDRSTRLEN];
        const AftDataBytes &data = boost::get<const AftDataBytes &>(_value);
        inet_ntop(AF_INET, &data[0], addressString, INET_ADDRSTRLEN);
        os << addressString;
        return os;
    }
};

#define AFT_IP6_ADDR_BITS 128

///
/// @class   AftDataIP6Addr
/// @brief   Data class for IPv6 addresses
///
class AftDataIP6Addr: public AftData {
public:
    ///
    /// @brief                   Factory generator for IPv6 address
    /// @param [in] newAddress   IPv6 address as a byte vector to represent in data
    ///
    static std::shared_ptr<AftDataIP6Addr> create(AftDataBytes newAddress) {
        return std::make_shared<AftDataIP6Addr>(newAddress);
    }

    ///
    /// @brief                   Factory generator for IPv6 address
    /// @param [in] newAddress   IPv6 address as a C-style byte array to represent in data
    /// @param [in] newBitLength Length of IPv6 address in bits (i.e. subnet length)
    ///
    static std::shared_ptr<AftDataIP6Addr> create(uint8_t *newAddress, uint32_t newBitLength) {
        return std::make_shared<AftDataIP6Addr>(newAddress, newBitLength);
    }

    ///
    /// @brief                   Factory generator for IPv6 address
    /// @param [in] newAddress   IPv6 address as a C-style const byte array to represent in data
    /// @param [in] newBitLength Length of IPv6 address in bits (i.e. subnet length)
    ///
    static std::shared_ptr<AftDataIP6Addr> create(const uint8_t *newAddress, uint32_t newBitLength) {
        return std::make_shared<AftDataIP6Addr>(newAddress, newBitLength);
    }

    ///
    /// @brief                   Factory generator for IPv4 address
    /// @param [in] prefixString IPv4 address as a dot-notation string to represent in data
    ///
    static std::shared_ptr<AftDataIP6Addr> create(std::string prefixString) {
        return std::make_shared<AftDataIP6Addr>(prefixString);
    }

    ///
    /// @brief                   Constructor for IPv6 address
    /// @param [in] newAddress   IPv6 address as a byte vector to represent in data
    ///
    AftDataIP6Addr(AftDataBytes newAddress): AftData(newAddress, AFT_IP6_ADDR_BITS) {};

    ///
    /// @brief                   Constructor for IPv6 address
    /// @param [in] newAddress   IPv6 address as a C-style byte array to represent in data
    /// @param [in] newBitLength Length of IPv6 address in bits (i.e. subnet length)
    ///
    AftDataIP6Addr(uint8_t *newAddress, uint32_t newBitLength): AftData() {
        //
        // Convenience constructor for arbitrary byte buffers
        //
        AftDataBytes data;
        _bitLength = newBitLength;
        data.assign(newAddress, newAddress + byteLength());
        _value     = data;
    }

    ///
    /// @brief                   Constructor for IPv6 address
    /// @param [in] newAddress   IPv6 address as a C-style const byte array to represent in data
    /// @param [in] newBitLength Length of IPv6 address in bits (i.e. subnet length)
    ///
    AftDataIP6Addr(const uint8_t *newAddress, uint32_t newBitLength): AftData() {
        //
        // Convenience constructor for arbitrary byte buffers
        //
        AftDataBytes data;
        _bitLength = newBitLength;
        data.assign(newAddress, newAddress + byteLength());
        _value     = data;
    }

    ///
    /// @brief                   Constructor for IPv6 address
    /// @param [in] newAddress   IPv6 address as a C-style byte array to represent in data
    ///
    AftDataIP6Addr(uint8_t *newAddress): AftData() {
        //
        // Convenience constructor for arbitrary byte buffers
        //
        AftDataBytes data;
        _bitLength = AFT_IP6_ADDR_BITS;
        data.assign(newAddress, newAddress + byteLength());
        _value     = data;
    }

    ///
    /// @brief                   Constructor for IPv6 address
    /// @param [in] prefixString IPv6 address as a standard-notation string to represent in data
    ///
    AftDataIP6Addr(std::string prefixString): AftData() {
        AftDataBytes aData(16);
        
        //
        // Cheap'n'nasty hack to build key prefixes from text strings for debug purposes
        //
        inet_pton(AF_INET6, prefixString.c_str(), &aData[0]);
        _value     = aData;
        _bitLength = AFT_IP6_ADDR_BITS;
    };

    
    //
    // Virtual functions
    //

    /// @returns Reference to base of prefix as uint8_t pointer
    virtual const uint8_t *dataArray() const { return boost::get<AftDataBytes>(_value).data(); };
    
    virtual uint32_t append(AftDataBytes &aData) {
        const AftDataBytes &data = boost::get<const AftDataBytes &>(_value);
        aData.insert(aData.end(), data.begin(), data.end() );
        return _bitLength;
    };
    virtual const std::string dataClass() const { return "AftDataIP6Addr"; };
    virtual std::ostream &description (std::ostream &os) const {
        char addressString[INET6_ADDRSTRLEN];
        const AftDataBytes &data = boost::get<const AftDataBytes &>(_value);
        inet_ntop(AF_INET6, &data[0], addressString, INET6_ADDRSTRLEN);
        os << addressString;
        return os;
    }
};

#define AFT_ETHER_ADDR 6                                        ///< Size of ethernet MAC address in bytes
#define AFT_ETHER_ADDR_BITS (AFT_ETHER_ADDR * AFT_BITS_IN_BYTE) ///< Size of ethernet MAC address in bits

///
/// @class   AftDataEtherAddr
/// @brief   Data class for Ethernet MAC addresses
///
class AftDataEtherAddr: public AftData {
public:
    typedef std::shared_ptr<AftDataEtherAddr> Ptr; ///< Pointer type for all AftData ethernet addresses
    
    ///
    /// @brief                   Factory generator for Ethernet address
    /// @param [in] newAddress   MAC address as a byte vector to represent in data
    ///
    static AftDataEtherAddr::Ptr create(AftDataBytes newAddress) {
        return std::make_shared<AftDataEtherAddr>(newAddress);
    }

    ///
    /// @brief                   Factory generator for Ethernet address
    /// @param [in] newAddress   Ethernet address as a C-style byte array to represent in data
    ///
    static AftDataEtherAddr::Ptr create(uint8_t *newAddress) {
        return std::make_shared<AftDataEtherAddr>(newAddress);
    }

    ///
    /// @brief                   Factory generator for Ethernet address
    /// @param [in] newAddress   Ethernet address as a C-style const byte array to represent in data
    ///
    static AftDataEtherAddr::Ptr create(const uint8_t *newAddress) {
        return std::make_shared<AftDataEtherAddr>(newAddress);
    }

    ///
    /// @brief                   Factory generator for Ethernet address
    /// @param [in] prefixString Ethernet address as a dot-notation string to represent in data
    ///
    static AftDataEtherAddr::Ptr create(std::string prefixString) {
        return std::make_shared<AftDataEtherAddr>(prefixString);
    }

    ///
    /// @brief                   Constructor for Ethernet address
    /// @param [in] newAddress   MAC address as a byte vector to represent in data
    ///
    AftDataEtherAddr(AftDataBytes newAddress): AftData(newAddress, AFT_ETHER_ADDR_BITS) {};

    ///
    /// @brief                   Constructor for Ethernet address
    /// @param [in] newAddress   MAC address as a C-style byte array to represent in data
    ///
    AftDataEtherAddr(uint8_t *newAddress): AftData() {
        //
        // Convenience constructor for arbitrary byte buffers
        //
        AftDataBytes data;
        _bitLength = AFT_ETHER_ADDR_BITS;
        data.assign(newAddress, newAddress + byteLength());
        _value     = data;
    }

    ///
    /// @brief                   Constructor for Ethernet address
    /// @param [in] newAddress   MAC address as a C-style const byte array to represent in data
    ///
    AftDataEtherAddr(const uint8_t *newAddress): AftData() {
        //
        // Convenience constructor for arbitrary byte buffers
        //
        AftDataBytes data;
        _bitLength = AFT_ETHER_ADDR_BITS;
        data.assign(newAddress, newAddress + byteLength());
        _value     = data;
    }

    ///
    /// @brief                   Constructor for Ethernet address
    /// @param [in] prefixString Ethernet address as a dot-notation string to represent in data
    ///
    AftDataEtherAddr(std::string prefixString): AftData() {
        AftDataBytes       data;
        struct ether_addr *eaddr;
        uint8_t           *edata;
        
        //
        // Cheap'n'nasty hack to build key prefixes from text strings for debug purposes
        //
        eaddr = ether_aton(prefixString.c_str());
        edata = (uint8_t *)eaddr;
        data.assign(edata, edata + AFT_ETHER_ADDR);

        _value     = data;
        _bitLength = AFT_ETHER_ADDR_BITS;
    };

    
    //
    // Virtual functions
    //
    
    /// @returns Reference to base of prefix as uint8_t pointer
    virtual const uint8_t *dataArray() const { return boost::get<AftDataBytes>(_value).data(); };

    virtual uint32_t append(AftDataBytes &aData) {
        const AftDataBytes &data = boost::get<const AftDataBytes &>(_value);
        aData.insert(aData.end(), data.begin(), data.end() );
        return _bitLength;
    };
    virtual const std::string dataClass() const { return "AftDataEtherAddr"; };
    virtual std::ostream &description (std::ostream &os) const {
        const AftDataBytes &data = boost::get<const AftDataBytes &>(_value);
        char oldFill = os.fill('0');
        std::ios::fmtflags oldFlags(os.flags());
        
        os << std::setw(2) << std::hex << static_cast<uint32_t>(data.data()[0]);
        for (uint i = 1; i < AFT_ETHER_ADDR; ++i) {
            os << ':' << std::setw(2) << std::hex << static_cast<uint32_t>(data.data()[i]);
        }
        os << std::dec;
        os.fill(oldFill);
        os.flags(oldFlags);

        return os;
    }
};

///
/// @class   AftDataLabel
/// @brief   Data class for MPLS labels fields, which will be used for label encapsulation
///
class AftDataLabel: public AftData {
protected:
    uint8_t  _labelType;             ///< Swap or push label
    bool     _expValid;              ///< Set to true, if valid
    uint8_t  _exp;                   ///< EXP value
    bool     _ttlPropagate;          ///< True if we're propogating TTLs
    bool     _insertControlWord;     ///< True if we have to insert a control word
    uint8_t  _controlWord;           ///< Control word value
    bool     _insertFlowLabel;       ///< True if flow label needs to be inserted
    bool     _insertEntropyLabel;    ///< True if entropy label needs to be inserted
    bool     _popEntropyLabelOnSwap; ///< True if entropy label needs to be popped
    
public:
    typedef std::shared_ptr<AftDataLabel> Ptr; ///< Pointer type for all AftData MPLS labels
    
    ///
    /// @brief Factory generator for MPLS data labels
    ///
    /// @param [in] newLabel                  Value of the 20-bit MPLS label
    /// @param [in] newLabelType              Label type 0: Swap and 1: Push
    /// @param [in] newExpValid               False: Exp as code point; True: use exp value
    /// @param [in] newExp                    Set to the value of Exp to use
    /// @param [in] newTtlPropagate           True if we're propogating TTLs
    /// @param [in] newInsertControlWord      True if we're inserting control word
    /// @param [in] newControlWord            Control word value
    /// @param [in] newInsertFlowLabel        True if we're instering flow label
    /// @param [in] newInsertEntropyLabel     True if we're inserting entropy label
    /// @param [in] newPopEntropyLabelOnSwap  True if we're popping entropy label on swap
    ///
    static AftDataLabel::Ptr create(uint32_t  newLabel,
                                    uint8_t   newLabelType,
                                    bool      newExpValid,
                                    uint8_t   newExp,
                                    bool      newTtlPropagate,
                                    bool      newInsertControlWord,
                                    uint8_t   newControlWord,
                                    bool      newInsertFlowLabel,
                                    bool      newInsertEntropyLabel,
                                    bool      newPopEntropyLabelOnSwap) {
        return std::make_shared<AftDataLabel>(newLabel,
                                              newLabelType,
                                              newExpValid,
                                              newExp,
                                              newTtlPropagate,
                                              newInsertControlWord,
                                              newControlWord,
                                              newInsertFlowLabel,
                                              newInsertEntropyLabel,
                                              newPopEntropyLabelOnSwap);
    }
    
    ///
    /// @brief Factory generator for MPLS data labels
    ///
    /// @param [in] newLabel                  Value of the 20-bit MPLS label
    /// @param [in] newLabelType              Label type 0: Swap and 1: Push
    /// @param [in] newExpValid               False: Exp as code point; True: use exp value
    /// @param [in] newExp                    Set to the value of Exp to use
    /// @param [in] newTtlPropagate           True if we're propogating TTLs
    /// @param [in] newInsertControlWord      True if we're inserting control word
    /// @param [in] newInsertFlowLabel        True if we're instering flow label
    /// @param [in] newInsertEntropyLabel     True if we're inserting entropy label
    ///
    static AftDataLabel::Ptr create(uint32_t  newLabel,
                                    uint8_t   newLabelType,
                                    bool      newExpValid,
                                    uint8_t   newExp,
                                    bool      newTtlPropagate,
                                    bool      newInsertControlWord,
                                    bool      newInsertFlowLabel,
                                    bool      newInsertEntropyLabel) {
        return std::make_shared<AftDataLabel>(newLabel,
                                              newLabelType,
                                              newExpValid,
                                              newExp,
                                              newTtlPropagate,
                                              newInsertControlWord,
                                              0,
                                              newInsertFlowLabel,
                                              newInsertEntropyLabel,
                                              false);
    }

    ///
    /// @brief Factory generator for MPLS data labels
    ///
    /// @param [in] newLabel                  Value of the 20-bit MPLS label
    /// @param [in] newExp                    Set to the value of Exp to use
    /// @param [in] newTtlPropagate           True if we're propogating TTLs
    /// @param [in] newInsertControlWord      True if we're inserting control word
    /// @param [in] newControlWord            Control word value
    /// @param [in] newInsertFlowLabel        True if we're instering flow label
    /// @param [in] newInsertEntropyLabel     True if we're inserting entropy label
    /// @param [in] newPopEntropyLabelOnSwap  True if we're popping entropy label on swap
    ///
    static AftDataLabel::Ptr create(uint32_t  newLabel,
                                    uint8_t   newExp,
                                    bool      newTtlPropagate,
                                    bool      newInsertControlWord,
                                    uint8_t   newControlWord,
                                    bool      newInsertFlowLabel,
                                    bool      newInsertEntropyLabel,
                                    bool      newPopEntropyLabelOnSwap) {
        return std::make_shared<AftDataLabel>(newLabel,
                                              0,
                                              false,
                                              newExp,
                                              newTtlPropagate,
                                              newInsertControlWord,
                                              newControlWord,
                                              newInsertFlowLabel,
                                              newInsertEntropyLabel,
                                              newPopEntropyLabelOnSwap);
    }
    
    ///
    /// @brief Constructor for MPLS data labels
    ///
    /// @param [in] newLabel                  Value of the 20-bit MPLS label
    /// @param [in] newLabelType              Label type 0: Swap and 1: Push
    /// @param [in] newExpValid               False: Exp as code point; True: use exp value
    /// @param [in] newExp                    Set to the value of Exp to use
    /// @param [in] newTtlPropagate           True if we're propogating TTLs
    /// @param [in] newInsertControlWord      True if we're inserting control word
    /// @param [in] newControlWord            Control word value
    /// @param [in] newInsertFlowLabel        True if we're instering flow label
    /// @param [in] newInsertEntropyLabel     True if we're inserting entropy label
    /// @param [in] newPopEntropyLabelOnSwap  True if we're popping entropy label on swap
    ///
    AftDataLabel(uint32_t  newLabel,
                 uint8_t   newLabelType,
                 bool      newExpValid,
                 uint8_t   newExp,
                 bool      newTtlPropagate,
                 bool      newInsertControlWord,
                 uint8_t   newControlWord,
                 bool      newInsertFlowLabel,
                 bool      newInsertEntropyLabel,
                 bool      newPopEntropyLabelOnSwap) : AftData(newLabel, 20),
                                                      _labelType(newLabelType),
                                                      _expValid(newExpValid),
                                                      _exp(newExp),
                                                      _ttlPropagate(newTtlPropagate),
                                                      _insertControlWord(newInsertControlWord),
                                                      _controlWord(newControlWord),
                                                      _insertFlowLabel(newInsertFlowLabel),
                                                      _insertEntropyLabel(newInsertEntropyLabel),
                                                      _popEntropyLabelOnSwap(newPopEntropyLabelOnSwap) {};

    //
    // Accessors
    //

    /// @returns Returns the label used by the data instance
    uint32_t label()               const  { return boost::get<uint32_t>(_value); }

    /// @returns Returns the label type used by the data instance
    uint8_t  labelType()           const  { return _labelType; }

    /// @returns Returns true of the exp is valid
    bool     expValid()            const  { return _expValid; }

    /// @returns Returns the Exp value
    uint8_t  exp()                 const  { return _exp; }

    /// @returns Returns true if we're propogating TTLs
    bool     ttlPropagate()         const { return _ttlPropagate; };

    /// @returns Returns true if we're inserting a control word
    bool     insertControlWord()   const { return _insertControlWord; }

    /// @returns Returns the value of the control word for the label
    uint8_t  controlWord()          const { return _controlWord; }

    /// @returns Returns if the flow label needs to be inserted
    bool     insertFlowLabel()      const { return _insertFlowLabel; }

    /// @returns Returns if the entropy label needs to be inserted
    bool     insertEntropyLabel()   const { return _insertEntropyLabel; }

    /// @returns Returns if the entropy label needs to be popped on swap
    bool     popEntropyLabelOnSwap() const { return _popEntropyLabelOnSwap; };

    //
    // Virtual functions
    //
    virtual uint32_t append(AftDataBytes &aData) {
        uint32_t count;
        uint32_t max = byteLength();
        AftIndex labelValue = this->label();

        for (count = 0; count < max; count++) {
            uint8_t byte = labelValue & 0xFF;
            aData.push_back(byte);
            labelValue >>= 8;
        }
        return _bitLength;
    };
    virtual const std::string dataClass() const { return "AftDataLabel"; };
    virtual std::ostream &description (std::ostream &os) const {
        os << "Value " << this->label();
        os << ", Type " << static_cast<uint32_t>(_labelType);
        os << ", ExpValid " << _expValid;
        os << ", Exp " << static_cast<uint32_t>(_exp);
        os << ", TtlPropogate " << _ttlPropagate;
        os << ", InsertControlWord " << _insertControlWord;
        os << ", ControlWord " << static_cast<uint32_t>(_controlWord);
        os << ", InsertFlowLabel " << _insertFlowLabel;
        os << ", InsertEntropyLabel " << _insertEntropyLabel;
        os << ", popEntropyLabelOnSwap " << _popEntropyLabelOnSwap;
        return os;
    }
};

///
/// @class   AftDataLabelOnly
/// @brief   Data class for MPLS labels, which will be used for installing the label routes
///
class AftDataLabelOnly: public AftData {
public:
    ///
    /// @brief Factory constructor for label only MPLS data
    ///
    /// @param [in] newLabel           Value of the MPLS label
    /// @param [in] newLabelLenInBits  MPLS label length
    ///
    static std::shared_ptr<AftDataLabelOnly> create(uint32_t  newLabel,
                                                    uint16_t  newLabelLenInBits)
    {
        return std::make_shared<AftDataLabelOnly>(newLabel, newLabelLenInBits);
    }

    ///
    /// @brief Constructor for label only MPLS data
    ///
    /// @param [in] newLabel           Value of the MPLS label
    /// @param [in] newLabelLenInBits  MPLS label length
    ///
    AftDataLabelOnly(uint32_t  newLabel,
                     uint16_t  newLabelLenInBits)  : AftData(newLabel, newLabelLenInBits) {}

    //
    // Virtual functions
    //
    virtual uint32_t append(AftDataBytes &aData) {
        uint32_t count;
        uint32_t max = byteLength();
        AftIndex labelValue = boost::get<uint32_t>(_value);

        for (count = 0; count < max; count++) {
            uint8_t byte = labelValue & 0xFF;
            aData.push_back(byte);
            labelValue >>= 8;
        }
        return _bitLength;
    };
    virtual const std::string dataClass() const { return "AftDataLabelOnly"; };
    virtual std::ostream &description (std::ostream &os) const {
        os << "Label" << boost::get<uint32_t>(_value);
        return os;
    }
};
//
// @}
//

#endif
