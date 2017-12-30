//
/// @file   AftPortTable.h
/// @brief  AftPortTable and AftPortEntry class definitions
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

#ifndef __AftPortTable__
#define __AftPortTable__

#include "jnx/AftNode.h"

///
/// @defgroup  AftPort Ports
/// @brief Classes used to define and manage input and output ports for Aft
/// @{
///

typedef std::map<AftIndex, AftPortPtr>  AftPortIndexMap; ///< Underlying map type used for token table

///
/// @class   AftPortTable
/// @brief   Master class for managing either input or output ports
///
class AftPortTable {
protected:
    AftPortIndexMap _ports;    ///< Underlying map used to hold port entries
    AftIndex        _maxIndex; ///< Maximum index allowed in port table
    
public:
    ///
    /// @brief  Factory convenience creator for shared_ptr to port table.
    ///
    static std::shared_ptr<AftPortTable> create();
    
    //
    // Constructor and destructor
    //
    AftPortTable(): _maxIndex(0) {};
    ~AftPortTable() {};
    
    //
    // Port management
    //
    void insert(const AftPortPtr      &port);
    AftIndex size();
    AftIndex max();
    
    //
    // Index range
    //
    ///
    /// @brief Set the maximum index value of the port table
    /// @param [in] newMaxIndex Maximum index the port table will take
    ///
    void setMaxIndex(const AftIndex newMaxIndex) { _maxIndex = newMaxIndex; };
    
    /// @return Maximum index allowed in port table
    AftIndex maxIndex() const { return _maxIndex; };
    
    //
    // Table access
    //
    bool portForIndex(AftIndex portIndex, AftPortPtr &port);
    bool portForName(std::string name, AftPortPtr &port);
    bool tokenForIndex(AftIndex portIndex, AftNodeToken &token);
    bool tokenForName(std::string name, AftNodeToken &token);
    
    //
    // Debug
    //
    std::ostream &description (std::ostream &os) const;
};

typedef std::shared_ptr<AftPortTable> AftPortTablePtr; ///< Pointer type used to refer to instances of port tables

///
/// @}
///

#endif
