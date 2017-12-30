//
/// @file   AftValidator.h
/// @brief  Fundamental type definitions used by Aft
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

#ifndef __AftValidator__
#define __AftValidator__

#include "jnx/AftTypes.h"

///
/// @class   AftValidator
/// @brief   Base virtual class used for all data validators
///

class AftValidator {
public:
    virtual bool validate(AftNodeToken nodeToken, std::ostream &os) = 0;
    virtual bool validate(const AftTokenVector &nodeTokens, std::ostream &os) = 0;
    virtual bool validate(const AftField &field, std::ostream &os) = 0;
    virtual bool validate(const AftFieldVector &fields, std::ostream &os) = 0;
    virtual bool validate(const AftKey &key, std::ostream &os) = 0;
    virtual bool validate(const AftKeyVector &keys, std::ostream &os) = 0;
};

typedef std::shared_ptr<AftValidator> AftValidatorPtr; ///< Pointer type for all AftValidator objects

#endif
