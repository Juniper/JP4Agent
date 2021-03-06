//
// Juniper P4 Agent
//
/// @file  Afi.h
/// @brief Afi
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

#ifndef SRC_AFI_INCLUDE_AFI_H_
#define SRC_AFI_INCLUDE_AFI_H_

#include "arpa/inet.h"
#include "google/rpc/code.pb.h"
#include "p4/tmp/p4config.pb.h"
#ifdef UBUNTU
#include "p4runtime.grpc.pb.h"
#include "p4runtime.pb.h"
#else // UBUNTU
#include "p4runtime_wrl.grpc.pb.h"
#include "p4runtime_wrl.pb.h"
#endif // UBUNTU
#include "P4Info.h"

#ifdef OPENTRACING
#include <jaegertracing/Tracer.h>
#endif // OPENTRACING
//#include <jsoncpp/json/json.h>
#include <json/json.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "AfiCreator.h"
#include "AfiDM.h"
#include "AfiDevice.h"
#include "AfiJsonResource.h"
#include "AfiNext.h"
#include "AfiObject.h"
#include "AfiTree.h"
#include "AfiTreeEntry.h"
#include "AfiCap.h"
#include "AfiCapMatch.h"
#include "AfiCapAction.h"
#include "AfiCapEntry.h"
#include "AfiCapEntryMatch.h"
#include "AfiCapEntryAction.h"
#include "AfiEncap.h"
#include "AfiEncapEntry.h"
#include "AfiTreeEncap.h"
#include "AfiTreeEncapEntry.h"
#include "AfiTypes.h"
#include "Log.h"

//
// This function to be implemented by each target
//
AFIHAL::AfiDeviceUPtr createDevice(const std::string &name);

namespace AFIHAL
{
class Afi;
using AfiUPtr          = std::unique_ptr<Afi>;
using AfiObjectNameMap = std::map<std::string, AfiObjectPtr>;
// using AfiObjectIdMap = std::map<uint32_t, AfiObjectPtr>;

template <class T> inline
void str2Uint(const std::string& s, T &v) {
    v = *((const T *) &s.c_str()[0]);
    if (typeid(T) == typeid(uint16_t)) {
        v = ntohs(v);
    } else if (typeid(T) == typeid(uint32_t)) {
        v = ntohl(v);
    }
}

class AfiTEntryMatchField {
public:
    enum MfType { EXACT, LPM, TERNARY };

    AfiTEntryMatchField(int id,
                        MfType t,
                        const std::string &value,
                        const uint16_t len,
                        const std::string &mask) : _id(id),
                                                   _type(t),
                                                   _value(value),
                                                   _len(len),
                                                   _mask(mask) { }
    ~AfiTEntryMatchField() { }

    uint32_t id() const { return _id; }
    MfType type() const { return _type; }
    const std::string& value() const { return _value; }
    uint16_t len() const { return _len; }
    const std::string& mask() const { return _mask; }

    std::ostream &description(std::ostream &os) const {
        os << "_________ AfiTEntryMatchField _______" << std::endl;
        os << "Match Field ID: " << _id << std::endl;

        if (_value.size() == 1) {
            uint8_t v;
            str2Uint(_value, v);
            os << "Match Field Value: " << v << std::endl;
            if (_type == MfType::TERNARY) {
                uint8_t m;
                str2Uint(_mask, m);
                os << "Match Field Mask: "  << m << std::endl;
            }
        } else if (_value.size() == 2) {
            uint16_t v;
            str2Uint(_value, v);
            os << "Match Field Value: " << v << std::endl;
            if (_type == MfType::TERNARY) {
                uint16_t m;
                str2Uint(_mask, m);
                os << "Match Field Mask: "  << m << std::endl;
            }
        } else if (_value.size() == 4) {
            uint32_t v;
            str2Uint(_value, v);
            os << "Match Field Value: " << v << std::endl;
            if (_type == MfType::TERNARY) {
                uint32_t m;
                str2Uint(_mask, m);
                os << "Match Field Mask: "  << m << std::endl;
            }
        } else if (_value.size() == 6) {
            os << "Match Field Value: ";
            for (unsigned int i = 0; i < _value.size(); i++)
                os << static_cast<unsigned>(_value[i]);
            os << std::endl;
            if (_type == MfType::TERNARY) {
                os << "Match Field Mask: ";
                for (unsigned int i = 0; i < _value.size(); i++)
                    os << static_cast<unsigned>(_mask[i]);
                os << std::endl;
            }
        }

        if (_type == MfType::LPM)
            os << "Match Field Len: " << _len << std::endl;

        return os;
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const AfiTEntryMatchField &mf)
    {
        return mf.description(os);
    }

private:
    uint32_t _id;
    MfType _type;
    const std::string _value;
    const uint16_t _len;
    const std::string _mask;
};

class AfiAEntry {
public:
    AfiAEntry(int id,
              const std::string &value) : _id(id),
                                          _value(value) { }
    ~AfiAEntry() { }

    uint32_t id() const { return _id; }
    const std::string& value() const { return _value; }

    std::ostream &description(std::ostream &os) const {
        os << "_________ AfiAEntry _______" << std::endl;
        os << "Action ID: " << _id << std::endl;

        if (_value.size() == 1) {
            uint8_t v;
            str2Uint(_value, v);
            os << "Action Value: " << _value << "," << v << std::endl;
        } else if (_value.size() == 2) {
            uint16_t v;
            str2Uint(_value, v);
            os << "Action Value: " << v << std::endl;
        } else if (_value.size() == 4) {
            uint32_t v;
            str2Uint(_value, v);
            os << "Action Value: " << v << std::endl;
        } else if (_value.size() == 6) {
            os << "Action Value: ";
            for (unsigned int i = 0; i < _value.size(); i++)
                os << static_cast<unsigned>(_value[i]);
            os << std::endl;
        }

        return os;
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const AfiAEntry &ae)
    {
        return ae.description(os);
    }

private:
    uint32_t _id;
    const std::string _value;
    const std::string _mask;
};

class Afi
{
 public:
    static Afi &instance()
    {
        static Afi afi;
        return afi;
    }

    Afi(Afi const &) = delete;
    Afi(Afi &&)      = delete;
    Afi &operator=(Afi const &) = delete;
    Afi &operator=(Afi &&) = delete;

    // AfiDevice& getDevice() {
    //    return _afiDevice;
    //}

    void init(const std::string &name)
    {
        // AfiDeviceUPtr createDevice(const std::string &name);

        Log(DEBUG) << "___ Afi::init : creating device ___";
        (name == "") ?
            _afiDevice = createDevice("_device_") :
            _afiDevice = createDevice(name);
    }

    bool handleAfiJsonObject(const Json::Value &cfg_obj,
                             const bool &pipeline_stage);
    bool handlePipelineConfig(const Json::Value &cfg_root);
    bool addAfiTree(const std::string &aftTreeName, const std::string &keyField,
                    const int protocol, const std::string &defaultNextObject,
                    const unsigned int treeSize);

    bool addEntry(const std::string &keystr, int pLen);

    bool afiAddCapEntry(P4InfoTablePtr table,
                        P4InfoActionPtr action,
                        const std::vector<AfiTEntryMatchField> &mfs,
                        const std::vector<AfiAEntry> &aes,
                        Json::Value& result);

    bool afiAddObjEntry(const uint32_t tId,
                        const uint32_t aId,
                        const std::vector<AfiTEntryMatchField> &mfs,
                        const std::vector<AfiAEntry> &afiActions);

    const AfiObjectPtr getAfiObject(const std::string &name)
    {
        Log(DEBUG) << "getAfiObject name:" << name;
        return _afiDevice->getAfiObject(name);
    }

    const std::vector<AfiObjectPtr> getAfiObjects() const
    {
        return _afiDevice->getAfiObjects();
    }

 protected:
    Afi() {}
    ~Afi() {}

 private:
    AfiDeviceUPtr _afiDevice;
};

}  // namespace AFIHAL

#endif  // SRC_AFI_INCLUDE_AFI_H_
