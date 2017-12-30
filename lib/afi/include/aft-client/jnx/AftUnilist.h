//
/// @file  AftUnilist.h
/// @brief Unilist definitions
//
//  Created by Sreenivas Gadela on 6/25/17.
//  Copyright (c) 2017 Juniper Networks Inc. All rights reserved.
//

#ifndef __AftUnilist__
#define __AftUnilist__

#include "jnx/AftNode.h"

///
/// @class   AftOrderedList
/// @brief   OrderedList class for Unilist
///
class AftOrderedList : public AftNode {
protected:
    AftTokenVector         _oNext; ///< Array of next nodes

public:
    typedef std::shared_ptr<AftOrderedList> Ptr; ///< Pointer type for
                                                 ///  OrderedList nodes

    ///
    /// @brief Factory generate an OrderedList node
    /// @param [in] next;       ///< Array of next nodes
    ///
    static AftOrderedList::Ptr create(const AftTokenVector & next) {
        return std::make_shared<AftOrderedList>(next);
    }

    ///
    /// @brief Copy constructor for AftOrderedList
    /// @param [in] node    AftOrderedList node to copy
    ///
    AftOrderedList(const AftOrderedList &node): AftNode(node),
                                                _oNext(node.next()) { }

    ///
    /// @brief Construct AftOrderedList node
    /// @param [in] next;       ///< Array of next nodes
    ///
    AftOrderedList(const AftTokenVector & nextArg) : AftNode(),
                                                     _oNext(nextArg) { }

    ~AftOrderedList() {};

    virtual const std::string nodeType() const { return "AftOrderedList"; }

    const AftTokenVector & next() const { return _oNext; }
};

///
/// @class   AftUnilistElement
/// @brief   Unilist element
///
struct AftUnilistElement {
    AftNodeToken elemToken;
    bool         install;    ///< Install child element
};

typedef std::vector<AftUnilistElement> AftUnilistElementVector;

///
/// @class   AftUnilist
/// @brief   Unilist
///
class AftUnilist : public AftNode {

public:
    //
    // Used for Active/Backup which is unilist
    // else used for AE case
    //
    enum class AftUnilistMode {
        UNKNOWN,
        UNILIST,
        AE
    };

protected:

    uint16_t                    _cCount;      ///< Number of child elements
    AftUnilistElementVector     _cList;       ///< List of child elements
    AftNodeToken                _orderedList; ///< Ordered list
    AftUnilistMode              _mode;        /// << mode(unilist, aggregate)
    AftNodeToken                _selector;    ///< Selector

    bool                        _cpHash;
    bool                        _rotateHash;
    bool                        _adaptive;
    bool                        _inplaceUpdate;
    bool                        _randomMode;

public:

    using Ptr = std::shared_ptr<AftUnilist>; ///< Pointer type for AftUnilist
                                             ///  nodes

    ///
    /// @brief Factory generate a AftUnilist node
    /// @param [in] elemList         List of child elements
    /// @param [in] listToken        Token id of Child List Node
    /// @param [in] mode             UnilistMode or AggregateMode
    /// @param [in] selectorToken    Selector Token
    ///
    static AftUnilist::Ptr create(const AftUnilistElementVector &elemList,
                                  AftNodeToken                  listToken = AFT_NODE_TOKEN_NONE,
                                  AftUnilistMode                mode = AftUnilistMode::UNILIST,
                                  AftNodeToken                  selectorToken = AFT_NODE_TOKEN_NONE,
                                  bool                          rotateHash = true,
                                  bool                          cpHash = true,
                                  bool                          adaptive = false,
                                  bool                          randomMode = false,
                                  bool                          inplaceUpdate = true) {
        return std::make_shared<AftUnilist>(elemList, listToken, mode, selectorToken,
                                            rotateHash, cpHash, adaptive, randomMode,
                                            inplaceUpdate);
    }

    ///
    /// @brief Construct a unilist node
    /// @param [in] elemList       List of child elements
    /// @param [in] listToken      token of the List
    /// @param [in] mode           qualifies the type whether Unilist or AE
    /// @param [in] selector       selector node Token (optional) which indicated frr is not needed
    ///
    AftUnilist(const AftUnilistElementVector &elemList,
               const AftNodeToken            listToken = AFT_NODE_TOKEN_NONE,
               const AftUnilistMode          mode = AftUnilistMode::UNILIST,
               const AftNodeToken            selector = AFT_NODE_TOKEN_NONE,
               const bool                    rotateHash = true,
               const bool                    cpHash = true,
               const bool                    adaptive = false,
               const bool                    randomMode = false,
               const bool                    inplaceUpdate = true)
        : AftNode(), 
         _cCount(elemList.size()),
         _cList(elemList),
         _orderedList(listToken),
         _mode(mode),
         _selector(selector),
         _cpHash(cpHash),
         _rotateHash(rotateHash),
         _adaptive(adaptive),
         _inplaceUpdate(inplaceUpdate),
         _randomMode(randomMode)
    {
    }

    ~AftUnilist() { }

    ///
    /// @brief Get list of nodes needed by unilist
    /// @param [out] nextTokens List of tokens of child elements + selector
    ///
    virtual void nextNodes(AftTokenVector &nextTokens) {
        //
        // TBD: Validate tokens before pushing
        //
        /* Push ListNode Token instead of each child node tokens
        for (const auto &cElement : _cList) {
            nextTokens.push_back(cElement.elemToken);
        }
        */
        if (AftTokenIsSet(_orderedList)) {
            nextTokens.push_back(_orderedList);
        }
        //
        //                   ____ List Node
        //                  |
        // Unilist points to
        //                  |____ Selector Node
        //
        // Selector is another next node
        //
        if (AftTokenIsSet(_selector)) {
            nextTokens.push_back(_selector);
        }
    }

    ///
    /// @brief Fast Re-Route needeed?
    ///
    bool frrNeeded() { return true; }

    uint32_t count() const { return _cCount; };
    const AftUnilistElementVector &cList() const { return _cList; };

    AftNodeToken listNodeToken() const { return _orderedList;};
    AftNodeToken selectorToken() const { return _selector; };

    bool randomModeNeeded() const { return _randomMode; };
    bool rotateHashNeeded() const { return _rotateHash; };
    bool cpHashNeeded()     const { return _cpHash; };
    bool adaptiveNeeded()   const { return _adaptive; };
    bool inPlaceUpdateNeeded() const { return _inplaceUpdate; };

    AftUnilistMode getMode() { return _mode; };

    virtual const std::string nodeType() const { return "AftUnilist"; };

    virtual std::ostream &description(std::ostream &os) const {
        AftNode::description(os);
        os << "\nListNodeToken: " << _orderedList;
        os << "\nSelectorNodeToken: " << _selector;
        os << "\nNumber of Child: " << _cCount;
        os << "\ncList = ";
        for (const auto &cElement : _cList) {
            std::string install = cElement.install ? " install; " :
                                    " don't install; ";
            os << " " << cElement.elemToken << install;
        }
        os << "\n";
        return os;
    }
};

#endif
