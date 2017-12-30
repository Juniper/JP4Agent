//
// @file   AftTermAction.h
// @brief  Definitions for Aft Filter Term MatchConditions
// Created by Ramakant on July 2017
// Copyright (c) Juniper Networks, Inc., [2017].
//
#ifndef __AftTermAction__
#define __AftTermAction__

#include <sstream>
#include <string>
#include <vector>

class AftTermAction;
using AftTermActionUPtr = std::unique_ptr<AftTermAction>;

class AftTermAction {
    public:
        enum ActionType {
            Accept,
            Discard,
            Sample,
            Log,
            Syslog,
            NextTerm,
            Policer,
            Counter,
            TcCounter,
            Srtcm,
            Trtcm,
            Reject,
            Nexthop,
            RtTable,
            PlpRemark,
            FcRemark,
            DscpRemark,
            Invalid // Should be last action type
        };

    private:
        ActionType _type;
    public:
        static AftTermActionUPtr create (ActionType tp) {
            return std::make_unique<AftTermAction>(tp);
        }
        // @brief constructor
        AftTermAction(ActionType tp) : _type(tp) {};

        ActionType type() const { return _type; }

        virtual void show(std::ostream &out) {
            out << "\t\t Action Type: "
                << toStringTermActionType(type())
                << std::endl;
        };

        //
        // virtual destructor
        //
        virtual ~AftTermAction() {};

        template<typename T>
        inline const T* typedDownCast() {
            return static_cast<T*>(this);
        };

        static std::string toStringTermActionType(ActionType actionType) {
           switch (actionType) {
                case Accept:
                    return "accept";
                case Discard:
                    return "discard";
                case Sample:
                    return "sample";
                case Log:
                    return "log";
                case Syslog:
                    return "syslog";
                case NextTerm:
                    return "next-term";
                case Policer:
                    return "policer";
                case Counter:
                    return "counter";
                case TcCounter:
                    return "tc-counter";
                case Srtcm:
                    return "srtcm";
                case Trtcm:
                    return "trtcm";
                case Reject:
                    return "reject";
                case Nexthop:
                    return "nexthop";
                case RtTable:
                    return "rt-table";
                case PlpRemark:
                    return "plp-remark";
                case FcRemark:
                    return "fc-remark";
                case DscpRemark:
                    return "dscp-remark";
                default:
                    return "unknown";
           }
       }
};

class AftTermActionPlct;
using AftTermActionPlctUPtr = std::unique_ptr<AftTermActionPlct>;

//
// Common class type for all variants of counters and policers
//
class AftTermActionPlct : public AftTermAction {
    private:
        std::string _name; // TBD name can be removed in future
        uint32_t    _index;
    public:
        static AftTermActionPlctUPtr create (ActionType actionType,
                                             std::string plctName,
                                             uint32_t plctIndex) {
             return std::make_unique<AftTermActionPlct>(actionType,
                                                        plctName,
                                                        plctIndex);
        }

        AftTermActionPlct(ActionType actionType,
                          std::string plctName,
                          uint32_t plctIndex):
            AftTermAction(actionType), // NOLINT (JuniperAFT/Identifier)
            _name(plctName),
            _index(plctIndex) {}

        ~AftTermActionPlct(){};

        uint32_t index() const { return _index;}

        const std::string name() const { return _name;}

        void show (std::ostream &out) {
            if (!out.good()) out.clear();
            AftTermAction::show(out);
            out << "\t\t name = " << name()
                << " index = " << index() << std::endl;
        }
};

class AftTermActionRemark;
using AftTermActionRemarkUPtr = std::unique_ptr<AftTermActionRemark>;

//
// Common class type for all variants of term actions which remark packet values
//
class AftTermActionRemark : public AftTermAction {
    private:
        uint32_t _remarkVal;
    public:
        static AftTermActionRemarkUPtr create (ActionType actionType,
                                               uint32_t remarkVal) {
             return std::make_unique<AftTermActionRemark>(actionType,
                                                          remarkVal);
        }

        AftTermActionRemark (ActionType actionType, uint8_t remarkVal) :
            AftTermAction(actionType), // NOLINT (JuniperAFT/Identifier)
                _remarkVal(remarkVal) {};

        ~AftTermActionRemark(){};

        uint32_t remarkValue() const { return _remarkVal; }

        void show (std::ostream &out) {
            if (!out.good()) out.clear();
            AftTermAction::show(out);
            out << "\t\t remark-val = "
                << remarkValue() << std::endl;
        }
};

class AftTermActionReject;
using AftTermActionRejectUPtr = std::unique_ptr<AftTermActionReject>;

class AftTermActionReject : public AftTermAction {
    private:
        uint32_t _integerVal;
    public:
        static AftTermActionRejectUPtr create (uint32_t integerVal) {
            return std::make_unique<AftTermActionReject>(integerVal);
        }

        AftTermActionReject (uint32_t integer) :
            AftTermAction(Reject), //NOLINT (JuniperAFT/Identifier)
               _integerVal(integer) {};

        ~AftTermActionReject(){};

        uint32_t integerValue() const { return _integerVal; }

        void show (std::ostream &out) {
            if (!out.good()) out.clear();
            AftTermAction::show(out);
            out << "\t\t integer-val="
                << integerValue() << std::endl;
        }
};

class AftTermActionNexthop;
using AftTermActionNexthopUPtr = std::unique_ptr<AftTermActionNexthop>;

class AftTermActionNexthop : public AftTermAction {
    private:
        uint32_t           _nhIndex;
    public:
        //
        // TBD There would mostly be token for nh-action
        //
        static AftTermActionNexthopUPtr create (uint16_t index) {
            return std::make_unique<AftTermActionNexthop>(index);
        }

        AftTermActionNexthop (uint16_t index) :
            AftTermAction(Nexthop), //NOLINT (JuniperAFT/Identifier)
                _nhIndex(index) {};

        ~AftTermActionNexthop(){};

        uint32_t nhIndex() const { return _nhIndex; }

        void show (std::ostream &out) {
            if (!out.good()) out.clear();
            AftTermAction::show(out);
            out << "\t\t nh-index " << nhIndex() << std::endl;
        }
};

class AftTermActionRtTable;
using AftTermActionRtTableUPtr = std::unique_ptr<AftTermActionRtTable>;

class AftTermActionRtTable : public AftTermAction {
    private:
        uint32_t           _rttIndex;
    public:
        //
        // TBD There would mostly be token for rtt-action
        //
        static AftTermActionRtTableUPtr create(uint32_t index) {
            return std::make_unique<AftTermActionRtTable>(index);
        }

        AftTermActionRtTable (uint16_t index) :
            AftTermAction(RtTable), //NOLINT (JuniperAFT/Identifier)
                _rttIndex(index)  {};

        ~AftTermActionRtTable(){};

        uint32_t rttIndex() const { return _rttIndex; }

        void show (std::ostream &out) {
            if (!out.good()) out.clear();
            AftTermAction::show(out);
            out << "\t\t rtt-index " << rttIndex() << std::endl;
        }
};
using AftTermActionRtTableUPtr = std::unique_ptr<AftTermActionRtTable>;

#endif
