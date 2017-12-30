//
// Juniper P4 compiler 
//
/// @file  backend.h
/// @brief Juniper P4 compiler: AFI backend
//
// Created by Sandesh Kumar Sodhi, December, 2017
// Copyright (c) [2017] Juniper Networks, Inc. All rights reserved.
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

#ifndef _BACKENDS_AFI_BACKEND_H_
#define _BACKENDS_AFI_BACKEND_H_

#include "analyzer.h"
#include "expression.h"
#include "frontends/common/model.h"
#include "frontends/p4/coreLibrary.h"
#include "helpers.h"
#include "ir/ir.h"
#include "lib/error.h"
#include "lib/exceptions.h"
#include "lib/gc.h"
#include "lib/json.h"
#include "lib/log.h"
#include "lib/nullstream.h"
#include "JsonObjects.h"
#include "metermap.h"
#include "midend/convertEnums.h"
#include "options.h"
//#include "portableSwitch.h"
#include "afiSwitch.h"

namespace AFI {

class ExpressionConverter;

class Backend : public PassManager {
    using DirectCounterMap = std::map<cstring, const IR::P4Table*>;

    // Current implementation uses refMap and typeMap from midend.
    // Once all midend passes are refactored to avoid patching refMap, typeMap,
    // We can regenerated the refMap and typeMap in backend.
    P4::ReferenceMap*                refMap;
    P4::TypeMap*                     typeMap;
    P4::ConvertEnums::EnumMapping*   enumMap;
    const IR::ToplevelBlock*         toplevel;
    ExpressionConverter*             conv;
    P4::P4CoreLibrary&               corelib;
    ProgramParts                     structure;
    Util::JsonObject                 jsonTop;
    //P4::PortableModel&               model;  // remove
    DirectCounterMap                 directCounterMap;
    DirectMeterMap                   meterMap;
    ErrorCodesMap                    errorCodesMap;

    // backend supports multiple target architectures, we create different
    // json generators for each architecture to handle the differences in json
    // format for each architecture.
    P4V1::SimpleSwitch*              simpleSwitch;
    // PortableSwitchJsonConverter*  portableSwitch;

 public:
    AFI::JsonObjects*               json;
    Target                           target;
    Util::JsonArray*                 counters;
    Util::JsonArray*                 externs;
    Util::JsonArray*                 field_lists;
    Util::JsonArray*                 learn_lists;
    Util::JsonArray*                 meter_arrays;
    Util::JsonArray*                 register_arrays;
    Util::JsonArray*                 force_arith;
    Util::JsonArray*                 field_aliases;

    // We place scalar user metadata fields (i.e., bit<>, bool)
    // in the scalarsName metadata object, so we may need to rename
    // these fields.  This map holds the new names.
    std::map<const IR::StructField*, cstring> scalarMetadataFields;

    std::set<cstring>                pipeline_controls;
    std::set<cstring>                non_pipeline_controls;
    std::set<cstring>                update_checksum_controls;
    std::set<cstring>                verify_checksum_controls;
    std::set<cstring>                deparser_controls;

    // expects 'ingress' and 'egress' pipeline to have fixed name.
    // provide an map from user program block name to hard-coded names.
    std::map<cstring, cstring>       pipeline_namemap;

 protected:
    ErrorValue retrieveErrorValue(const IR::Member* mem) const;
    void createFieldAliases(const char *remapFile);
    void genExternMethod(Util::JsonArray* result, P4::ExternMethod *em);

 public:
    Backend(bool isV1, P4::ReferenceMap* refMap, P4::TypeMap* typeMap,
            P4::ConvertEnums::EnumMapping* enumMap) :
        refMap(refMap), typeMap(typeMap), enumMap(enumMap),
        corelib(P4::P4CoreLibrary::instance),
        //model(P4::PortableModel::instance),
        simpleSwitch(new P4V1::SimpleSwitch(this)),
        json(new AFI::JsonObjects()),
        target(Target::SIMPLE) { refMap->setIsV1(isV1); setName("BackEnd"); }
    void process(const IR::ToplevelBlock* block, AFIOptions& options);
    void convert(AFIOptions& options);
    void serialize(std::ostream& out) const
    { jsonTop.serialize(out); }
    P4::P4CoreLibrary &   getCoreLibrary() const   { return corelib; }
    ErrorCodesMap &       getErrorCodesMap()       { return errorCodesMap; }
    ExpressionConverter * getExpressionConverter() { return conv; }
    DirectCounterMap &    getDirectCounterMap()    { return directCounterMap; }
    DirectMeterMap &      getMeterMap()  { return meterMap; }
    //P4::PortableModel &   getModel()     { return model; }
    ProgramParts &        getStructure() { return structure; }
    P4::ReferenceMap*     getRefMap()    { return refMap; }
    P4::TypeMap*          getTypeMap()   { return typeMap; }
    P4V1::SimpleSwitch*   getSimpleSwitch()        { return simpleSwitch; }
    const IR::ToplevelBlock* getToplevelBlock() { CHECK_NULL(toplevel); return toplevel; }
    /// True if this parameter represents the standard_metadata input.
    bool isStandardMetadataParameter(const IR::Parameter* param);
};

}  // namespace AFI

#endif /* _BACKENDS_AFI_BACKEND_H_ */
