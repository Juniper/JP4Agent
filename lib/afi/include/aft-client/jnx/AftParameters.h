//
/// @file   AftParameters.h
/// @brief
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

#ifndef __AftParameters__
#define __AftParameters__

#include "AftData.h"
#include "boost/variant.hpp"

///
/// @addtogroup AftParameters
/// @brief Classes used to define optional node and entry parameters
/// @{
///

typedef std::map<std::string, AftDataPtr> AftParametersMap; ///< Map of AftParameter names and data values
typedef std::vector<std::string> AftParameterKeys;          ///< Vector of keys

///
/// @class AftParameters
///
class AftParameters {
protected:
    AftParametersMap _params; ///< Map of parameter names and values
    
public:
    typedef std::unique_ptr<AftParameters> UPtr;  ///< Pointer type for parameters
    
    ///
    /// @brief Factory generate parameters
    ///
    /// @param [in] newParams Reference to parameter map to use
    ///
    static AftParameters::UPtr create(const AftParametersMap &newParams = AftParametersMap()) {
        return std::make_unique<AftParameters>(newParams);
    }
    
    ///
    /// @brief Construct a parameter map
    ///
    /// @param [in] newParams Reference to parameter map to use
    ///
    AftParameters(const AftParametersMap &newParams): _params(newParams) {};
    ~AftParameters() {};
    
    /// @returns Reference to parameter map
    AftParametersMap &params() { return _params; };

    ///
    /// @brief           Convenience parameter access template. Use this to access node parameters
    /// @param [in] name Parameter name to look for
    /// @returns         True if parameter is found
    ///
    template <typename T>
    bool parameter(const std::string &name, T &resultValue) {
        AftDataPtr data = _params[name];
        if (data == nullptr) return false;
        resultValue = data->value<T>();
        return true;
    }
     ///
    /// @brief           Convenience parameter access template. Use this to access node parameters
    /// @param [in] name Parameter name to look for
    /// @returns         Pointer to AftData if parameter is found
    ///
    template <class T>
    std::shared_ptr<T> parameterData(const std::string &name) {
        AftDataPtr data = _params[name];
        return (data != nullptr) ? std::static_pointer_cast<T>(data) : nullptr;
    }
    
    ///
    /// @brief               Convenience parameter setter. Use this to set node parameters
    /// @param [in] name     Parameter name to set
    /// @param [in] newValue Reference to data to associate with name
    ///
    template <typename T>
    void setParameter(const std::string &name, const T &newValue) {
        _params[name] = AftData::create(newValue);
    }

    ///
    /// @brief           Convenience parameter setter. Use this to set node parameters
    /// @param [in] name Parameter name to set
    /// @param [in] data Reference to data to associate with name
    ///
    void setParameterData(const std::string &name, const AftDataPtr &data) {
        _params[name] = data;
    }

    ///
    /// @brief                 Convenience parameter set template. Use this to set parameters
    /// @param [in] name       Parameter name to look for
    /// @param [in] paramValue Parameter value to set
    ///
    template <class T>
    void setParameterData(const std::string &name, T paramValue) {
        AftDataPtr data = AftData::create(paramValue);
        if (data != nullptr) _params[name] = data;
    }
    
    ///
    ///
    ///
    void parameterKeys (AftParameterKeys &paramKeys) {
        for (auto it: _params) {
            paramKeys.push_back(it.first);
        }
    }

    AftParametersMap::iterator begin() { return _params.begin(); }
    AftParametersMap::iterator end() { return   _params.end(); }
};

///
/// @}
///

#endif
