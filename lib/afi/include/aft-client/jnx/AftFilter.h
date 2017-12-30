//
// @file   AftFilter.h
// @brief  Model definitions for general Aft filters
// Created by Shivayogi Ugaji on Feb 2017
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

#ifndef __AftFilter__
#define __AftFilter__

#include "jnx/AftNode.h"
#include "jnx/AftEntry.h"
#include "jnx/AftTermMatchCondition.h"
#include "jnx/AftTermAction.h"
///

using AftFilterIndex = AftFieldIndex;
using AftTermIndex = AftFieldIndex;
///
/// @class AftFilterTemplate
/// @brief Node used to define a filter template
///

class AftFilterTemplate : public AftNode {
  public:
    enum FilterFamily {
        Inet,
        Inet6,
        Mpls,
        CCC,
        Any,
        Invalid // Need to be last
    };

    enum FilterOperation {
        FilterStart,
        FilterEnd
    };
  protected:
    std::string         _filterName;  ///< Name of filter
    AftFilterIndex      _filterIndex; ///< Index of filter
    FilterFamily        _family = Invalid;
    uint64_t            _flags = 0;
    uint32_t            _priority = 0;
    uint32_t            _version = 0;
    FilterOperation     _oper = FilterStart;
    AftDataPrefix::Ptr  _filterTLV;    ///< TLV data for filter
  public:
    /// @typedef Pointer type for AftFilterTemplate
    ///
    typedef std::shared_ptr<AftFilterTemplate> Ptr;

    ///
    /// @brief Factory constructor for AftFilterTemplate
    ///
    static AftFilterTemplate::Ptr
    create(const std::string        name,
           const AftFilterIndex     index,
           const FilterFamily       family,
           const uint64_t           flags,
           const uint32_t           priority,
           const uint32_t           version,
           const FilterOperation    oper,
           const AftDataPrefix::Ptr tlvData) {
        return std::make_shared<AftFilterTemplate>(name,
                                                   index,
                                                   family,
                                                   flags,
                                                   priority,
                                                   version,
                                                   oper,
                                                   tlvData);
    }

    //
    // Constructor and destructor for AftFilterTemplate
    //
    AftFilterTemplate(const std::string        name,
                      const AftFilterIndex     index,
                      const FilterFamily       family,
                      const uint64_t           flags,
                      const uint32_t           priority,
                      const uint32_t           version,
                      const FilterOperation    oper,
                      const AftDataPrefix::Ptr tlvData)
        : AftNode(),
          _filterName(name),
          _filterIndex(index),
          _family(family),
          _flags(flags),
          _priority(priority),
          _version(version),
          _oper(oper),
          _filterTLV(tlvData) {};
    ~AftFilterTemplate() {}

    static AftFilterTemplate::Ptr
    create(const std::string newFilterName,
           const AftFilterIndex   newFilterIndex,
           const AftDataPrefix::Ptr newFilterTLV,
           const FilterOperation newOper) {
        return std::make_shared<AftFilterTemplate>(newFilterName,
                                                   newFilterIndex,
                                                   newFilterTLV,
                                                   newOper);
    }

    //
    // Constructor and destructor for AftFilterTemplate
    //
    AftFilterTemplate(const std::string newFilterName,
                      const AftFilterIndex     newFilterIndex,
                      const AftDataPrefix::Ptr newFilterTLV,
                      const FilterOperation    newOper)
                        : AftNode(),
                          _filterName(newFilterName),
                          _filterIndex(newFilterIndex),
                          _oper(newOper),
                          _filterTLV(newFilterTLV){};

    const std::string        filterName() const { return _filterName; };
    AftFilterIndex           filterIndex() const { return _filterIndex; };
    const AftDataPrefix::Ptr filterTLV() const { return _filterTLV; };
    FilterFamily             filterFamily() const { return _family; };
    uint64_t                 filterFlags() const { return _flags; };
    uint32_t                 filterPriority() const { return _priority; };
    uint32_t                 filterVersion() const { return _version; };
    FilterOperation          filterOperation() const { return _oper; };
    ///
    ///  @returns node type for AftFilterTemplate
    ///
    virtual const std::string nodeType() const {
        return "AftFilterTemplate";
    };

    std::string AftFilterOperString (AftFilterTemplate::FilterOperation newOper) const
    {
        switch(newOper) {
            case AftFilterTemplate::FilterStart:
                return "FilterStart";
                break;
            case AftFilterTemplate::FilterEnd:
                return "FilterEnd";
                break;
        }
        return "Unknown";
    }

    std::ostream &description (std::ostream &os) const
    {
        AftNode::description(os);
        os << "{FilterName:" << _filterName;
        os << " Index:" << _filterIndex;
        os << " FilterState:" << AftFilterOperString(_oper) << "}";
        return os;
    }

    ///
    /// @class Info
    /// @brief Information return class for AftFilterTemplate
    ///
    class Info: public AftInfo {
    public:
        struct CntrData {
            #define DFW_COUNTER_TYPE_BYTE_PKT   0
            #define DFW_TYPE_MAX_STATS          5
            #define DFW_INFO_TYPE_CNTR_READ_RE       0
            #define DFW_INFO_TYPE_CNTR_READ_CLI      1
            #define DFW_INFO_TYPE_CNTR_CLEAR         2
            #define MAX_BUFFER_SIZE 1400
            uint8_t _buffer[MAX_BUFFER_SIZE];
            uint32_t _len;
        };
        using CntrDataVec = std::vector <std::unique_ptr<CntrData>>;
    protected:
        CntrDataVec _cntrData;
    public:
        Info(AftNodeToken newNodeToken, CntrDataVec inCntrData): AftInfo(newNodeToken)
        {
            _cntrData = std::move(inCntrData);
        };
        ~Info() {};

        CntrDataVec& getCntrData() {return _cntrData;};
        virtual const std::string infoType() const { return "AftFilterTemplate"; };
    };

    virtual Info::UPtr nodeInfo(AftFilterTemplate::Info::CntrDataVec cntrData) const {
         return std::make_unique<AftFilterTemplate::Info>(_nodeToken, std::move(cntrData));
    };
};

using AftTermMatchVec = std::vector<AftTermMatchUPtr>;
using AftTermActionVec = std::vector<AftTermActionUPtr>;

///
/// @class AftFilterTerm
/// @brief Base entry for all Aft filter terms
///

class AftFilterTerm : public AftEntry {
  protected:
    AftNodeToken                          _termFilter; ///< Token of parent filter
    std::string                           _termName;   ///< Name of filter term
    AftFilterIndex                        _filterIndex;  ///< Index of filter term
    AftTermIndex                          _termIndex;
    AftTermIndex                          _nextTermIndex;
    AftTermMatchVec                       _matches;
    AftTermActionVec                      _actions;
    AftDataPrefix::Ptr                    _termTLV;    ///< TLV data for filter term
  public:
    ///
    /// @typedef Pointer type for AftFilterTerm
    ///
    typedef std::shared_ptr<AftFilterTerm> Ptr;

    ///
    /// @brief Factory constructor for AftFilterTerm
    ///
    static AftFilterTerm::Ptr create(const AftNodeToken           newTermFilter,
                                     const std::string            newTermName,
                                     const AftFilterIndex         newFilterIndex,
                                     const AftTermIndex           newTermIndex,
                                     const AftTermIndex           newTermNextIndex,
                                     AftTermMatchVec              matches,
                                     AftTermActionVec             actions,
                                     const AftDataPrefix::Ptr     newTermTLV) {
        return std::make_shared<AftFilterTerm>(
            newTermFilter, newTermName, newFilterIndex,
            newTermIndex, newTermNextIndex, std::move(matches),
            std::move(actions), newTermTLV);
    }

    //
    // Constructor and destructor for AftFilterTerm
    //
    AftFilterTerm(const AftNodeToken            newTermFilter,
                  const std::string             newTermName,
                  const AftFilterIndex          newFilterIndex,
                  const AftTermIndex            newTermIndex,
                  const AftTermIndex            newTermNextIndex,
                  AftTermMatchVec               matches,
                  AftTermActionVec              actions,
                  const AftDataPrefix::Ptr      newTermTLV)
        : AftEntry(newTermFilter),
          _termFilter(newTermFilter),
          _termName(newTermName),
          _filterIndex(newFilterIndex),
          _termIndex(newTermIndex),
          _nextTermIndex(newTermNextIndex),
          _matches(std::move(matches)),
          _actions(std::move(actions)),
          _termTLV(newTermTLV) {};

    ///
    /// @brief Factory constructor for AftFilterTerm
    ///
    static AftFilterTerm::Ptr create(const AftNodeToken           newTermFilter,
                                     const std::string            newTermName,
                                     const AftFilterIndex         newFilterIndex,
                                     const AftDataPrefix::Ptr     newTermTLV) {
        return std::make_shared<AftFilterTerm>(newTermFilter, newTermName, newFilterIndex,
                                               newTermTLV);
    }

    //
    // Constructor for AftFilterTerm
    //
    AftFilterTerm(const AftNodeToken            newTermFilter,
                  const std::string             newTermName,
                  const AftFilterIndex          newFilterIndex,
                  const AftDataPrefix::Ptr      newTermTLV)
        : AftEntry(newTermFilter),
    _termFilter(newTermFilter),
    _termName(newTermName),
    _filterIndex(newFilterIndex),
    _termTLV(newTermTLV) {};

    ~AftFilterTerm() {}

    AftNodeToken             termFilter() const { return _termFilter; };
    const std::string        termName() const { return _termName; };
    AftFilterIndex           filterIndex() const { return _filterIndex; };
    const AftDataPrefix::Ptr termTLV() const { return _termTLV; };
    AftTermIndex             termIndex() const { return _termIndex; };
    AftTermIndex             nextTermIndex() const { return _nextTermIndex; };

    const AftTermMatchVec&   getMatches() const {
        return (_matches);
    }

    const AftTermActionVec& getActions() const {
        return (_actions);
    }
    ///
    /// @returns entry type for AftFilterTerm
    ///
    virtual const std::string entryType() const {
        return "AftFilterTerm";
    };


    bool entryIsValid(const AftValidatorPtr &validator, std::ostream &os) {
        return true;
    }

    std::ostream &description (std::ostream &os) const
    {
        AftEntry::description(os);
        os << "{EntryType: " << entryType();
        os <<  " FilterTemplateToken: " << _termFilter;
        os << " Term Name: " << _termName << std::endl;
        os << " FilterIndex:" << _filterIndex << std::endl;
        if (termTLV()) {
            //
            // If TLV format we're done
            //
            return os;
        }
        os << " Term Matches: " << std::endl;
        for (auto& match: getMatches()) {
            match->show(os);
            os << std::endl;
        }
        os << " Term Actions: " << std::endl;
        for (auto& action: getActions()) {
            action->show(os);
            os << std::endl;
        }
        return os;
    }
};

///
/// @class AftFilterTermDelete
/// @brief Filter Term Delete Entry Class
///
class AftFilterTermDelete : public AftDeleteEntryTemplate<AftTermIndex> {
protected:
    std::string                           _termName;   ///< Name of filter term
public:
    using Ptr = std::shared_ptr<AftFilterTermDelete>; ///< Pointer type AftFilterTermDelete

    static AftFilterTermDelete::Ptr create(const AftNodeToken   delParentNode,
                                           AftTermIndex         delEntryKey,
                                           std::string          delEntryName) {
        return std::make_shared<AftFilterTermDelete>(delParentNode, delEntryKey, delEntryName);
    }

    ///
    /// @brief Construct a deletion entry for a filter term
    ///
    /// @param [in] delParentNode Token for parent node for entry
    /// @param [in] delEntryKey   Unique key for filter term
    /// @param [in] delEntryName Term Name
    ///
    AftFilterTermDelete(const AftNodeToken   delParentNode,
                        AftTermIndex         delEntryKey,
                        std::string          delEntryName) :
                                AftDeleteEntryTemplate(delParentNode,
                                                       delEntryKey),
                                                        _termName(delEntryName) {};


    /// @returns Class of entry as a string
    const std::string entryType() const override { return "AftDeleteFilterTerm"; };

    //
    // Getters
    //
    AftTermIndex index() const { return key(); };

    std::string name() const { return _termName; };

    ///
    /// @brief Print class description
    /// @param [in] os Reference to output stream to append description to
    /// @returns Reference to output stream with text description of entry appended
    ///
    virtual std::ostream &description (std::ostream &os) const override {
        os << " Term index:" << index() << std::endl;
        os << " Term name:" << name() << std::endl;
        return os;
    }
};


///
/// @class AftFilter
/// @brief Base node for all Aft filter instances
///

class AftFilter : public AftNode {
  protected:
    AftNodeToken _filterTemplate; ///< Token of filter template
    AftNodeToken _filterAccept;   ///< Token of filter accept
    AftNodeToken _filterReject;   ///< Token of filter reject
    AftNodeToken _filterDiscard;  ///< Token of filter discard
    uint32_t     _instanceKey;    ///< Key of the instance
    AftFilterIndex  _filterIndex;    ///< Index of filter Template
  public:
    ///
    /// @typedef Pointer type for AftFilter
    ///
    typedef std::shared_ptr<AftFilter> Ptr;

    ///
    /// @brief Factory constructor for AftFilter
    ///
    static AftFilter::Ptr
    create(const AftNodeToken newFilterTemplate,
           const AftNodeToken newFilterAccept,
           const AftNodeToken newFilterReject,
           const AftNodeToken newFilterDiscard,
           const uint32_t     newInstanceKey,
           const AftFilterIndex  newFilterIndex) {
        return std::make_shared<AftFilter>(
            newFilterTemplate, newFilterAccept, newFilterReject,
            newFilterDiscard, newInstanceKey, newFilterIndex);
    }

    //
    // Constructor and destructor for AftFilter
    //
    AftFilter(const AftNodeToken newFilterTemplate,
              const AftNodeToken newFilterAccept,
              const AftNodeToken newFilterReject,
              const AftNodeToken newFilterDiscard,
              const uint32_t     newInstanceKey,
              const AftFilterIndex newFilterIndex)
        : AftNode(),
          _filterTemplate(newFilterTemplate),
          _filterAccept(newFilterAccept),
          _filterReject(newFilterReject),
          _filterDiscard(newFilterDiscard),
          _instanceKey(newInstanceKey),
          _filterIndex(newFilterIndex){};

    ~AftFilter() {}

    AftNodeToken filterTemplate() const { return _filterTemplate; };
    AftNodeToken filterAccept() const { return _filterAccept; };
    AftNodeToken filterReject() const { return _filterReject; };
    AftNodeToken filterDiscard() const { return _filterDiscard; };
    AftFilterIndex filterTemplateIndex() const { return _filterIndex; };
    uint32_t filterInstanceKey() const { return _instanceKey; };

    /// @returns node type for AftFilter
    virtual const std::string nodeType() const {
        return "AftFilter";
    };
    virtual void nextNodes(AftTokenVector &nextTokens) {
        AftNode::nextNodes(nextTokens);
        nextTokens.push_back(_filterTemplate);
        nextTokens.push_back(_filterAccept);
        nextTokens.push_back(_filterReject);
        nextTokens.push_back(_filterDiscard);
    };

    std::ostream &description (std::ostream &os) const
    {
        AftNode::description(os);
        os << "{NodeType:" << nodeType();
        os << " Filter Template Token:" << _filterTemplate;
        os << " Index:" << _filterIndex;
        os << " Node mask: " << nodeMask();
        os << " Instance Key:" << _instanceKey << "}" << std::endl;
        return os;
    }

};


///
/// @class AftFilterBind
/// @brief Base node for all Aft filter bind
///

class AftFilterBindPoint : public AftNode {

protected:

    AftNodeToken      _bindPointValue;                     ///< Value of the bind point.
    u_int32_t         _bindPointType;                      ///< Type of the bind point.

    AftNodeToken      _cliFilterInputInstToken;         ///< Token of the cli ingress filter
    AftNodeToken      _cliPolicerInputInstToken;        ///< Token of the cli ingress policer-filter
    AftNodeToken      _implInputInstToken;              ///< Token of the implicit ingress filter

    AftNodeToken      _cliFilterOutputInstToken;        ///< Token of the cli egress filter
    AftNodeToken      _cliPolicerOutputInstToken;      ///< Token of the cli egress policer-filter
    AftNodeToken      _implOutputInstToken;             ///< Token of the implicit egress filter


public:
    enum BindPointType {
        BP_IFF,
        BP_LO0,
        BP_FTF,
        BP_IFL,
        BP_NH,
        BP_MAX
    };
    /// @typedef Pointer type for AftFilterBindPoint
    typedef std::shared_ptr<AftFilterBindPoint> Ptr;

    ///
    /// @brief Factory constructor for AftFilterBind
    ///
    static AftFilterBindPoint::Ptr
    create(const AftNodeToken newBindPointValue,
           const u_int32_t    newBindPointType,
           const AftNodeToken newCliFilterInputInstToken,
           const AftNodeToken newCliPolicerInputInstToken,
           const AftNodeToken newImplInputInstToken,
           const AftNodeToken newCliFilterOutputInstToken,
           const AftNodeToken newCliPolicerOutputInstToken,
           const AftNodeToken newImplOutputInstToken) {
        return std::make_shared<AftFilterBindPoint>(
                newBindPointValue, newBindPointType, newCliFilterInputInstToken,
                newCliPolicerInputInstToken, newImplInputInstToken, newCliFilterOutputInstToken,
                newCliPolicerOutputInstToken, newImplOutputInstToken);
    }


    //
    // Constructor and destructor for AftFilterBindPoint
    //
    AftFilterBindPoint(const AftNodeToken newBindPointValue,
                       const u_int32_t    newBindPointType,
                       const AftNodeToken newCliFilterInputInstToken,
                       const AftNodeToken newCliPolicerInputInstToken,
                       const AftNodeToken newImplInputInstToken,
                       const AftNodeToken newCliFilterOutputInstToken,
                       const AftNodeToken newCliPolicerOutputInstToken,
                       const AftNodeToken newImplOutputInstToken)
            : AftNode(),
              _bindPointValue(newBindPointValue),
              _bindPointType(newBindPointType),
              _cliFilterInputInstToken(newCliFilterInputInstToken),
              _cliPolicerInputInstToken(newCliPolicerInputInstToken),
              _implInputInstToken(newImplInputInstToken),
              _cliFilterOutputInstToken(newCliFilterOutputInstToken),
              _cliPolicerOutputInstToken(newCliPolicerOutputInstToken),
              _implOutputInstToken(newImplOutputInstToken) {}

    ~AftFilterBindPoint() {}

    AftNodeToken  bindPointValue() const {
        return _bindPointValue;
    }
    u_int32_t     bindPointType() const { return _bindPointType; }
    AftNodeToken  cliFilterInputInstToken() const { return _cliFilterInputInstToken; }
    AftNodeToken  cliPolicerInputInstToken() const { return _cliPolicerInputInstToken; }
    AftNodeToken  implInputInstToken() const { return _implInputInstToken; }
    AftNodeToken  cliFilterOutputInstToken() const { return _cliFilterOutputInstToken; }
    AftNodeToken  cliPolicerOutputInstToken() const { return _cliPolicerOutputInstToken; }
    AftNodeToken  implOutputInstToken() const { return _implOutputInstToken; }


    ///
    /// @returns node type for AftFilterBindPoint
    ///
    virtual const std::string nodeType() const {
        return "AftFilterBindPoint";
    };
    virtual void nextNodes(AftTokenVector &nextTokens) {
        AftNode::nextNodes(nextTokens);
        if (_cliFilterInputInstToken != AFT_NODE_TOKEN_NONE) {
            nextTokens.push_back(_cliFilterInputInstToken);
        }
        if (_cliPolicerInputInstToken != AFT_NODE_TOKEN_NONE) {
            nextTokens.push_back(_cliPolicerInputInstToken);
        }
        if (_implInputInstToken != AFT_NODE_TOKEN_NONE) {
            nextTokens.push_back(_implInputInstToken);
        }
        if (_cliFilterOutputInstToken != AFT_NODE_TOKEN_NONE) {
            nextTokens.push_back(_cliFilterOutputInstToken);
        }
        if (_cliPolicerOutputInstToken != AFT_NODE_TOKEN_NONE) {
            nextTokens.push_back(_cliPolicerOutputInstToken);
        }
        if (_implOutputInstToken != AFT_NODE_TOKEN_NONE) {
            nextTokens.push_back(_implOutputInstToken);
        }
    };

    std::ostream &description (std::ostream &os) const
    {
        AftNode::description(os);
        os << "{NodeType:" << nodeType();
        os << " Bind Object Token:" << _bindPointValue;
        os << " Bind Point Type:" << _bindPointType;
        os << " CLI Filter input inst token" <<  _cliFilterInputInstToken;
        os << " CLI Policer Filter input inst token" <<  _cliPolicerInputInstToken;
        os << " Impl Filter input inst token" <<  _implInputInstToken;
        os << " CLI Filter output inst token" <<  _cliFilterOutputInstToken;
        os << " CLI Policer Filter output inst token" <<  _cliPolicerOutputInstToken;
        os << " Impl Filter output inst token" <<  _implOutputInstToken;
        os << " Node mask: " << nodeMask() << "}" << std::endl;
        return os;
    }
};

///
/// @}
///

#endif
