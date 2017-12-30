//
// @file   AftTermMatchCondition.h
// @brief  Definitions for Aft Filter Term MatchConditions
// Created by Ramakant on July 2017
// Copyright (c) Juniper Networks, Inc., [2017].
//
#ifndef __AftTermMatchCondition__
#define __AftTermMatchCondition__

#include <sstream>
#include <string>
#include <vector>
#include <memory>

class AftDfwRange;
using AftDfwRangeUPtr = std::unique_ptr<AftDfwRange>;
using AftDfwRangeVec = std::vector<AftDfwRangeUPtr>;

class AftDfwRange {
    private:
        uint32_t _min;
        uint32_t _max;
    public:
        static AftDfwRangeUPtr create (uint32_t min_, uint32_t max_) {
            return std::make_unique<AftDfwRange>(min_, max_);
        }
        //@brief constructor
        AftDfwRange (uint32_t min_, uint32_t max_) :
            _min(min_), _max(max_){};

        //@brief get min of range
        uint32_t min() const { return _min; }

        //@brief get max of range
        uint32_t max() const { return _max; }
};

class AftDfwPrefix;
using AftDfwPrefixUPtr = std::unique_ptr<AftDfwPrefix>;
using AftDfwPrefixVec = std::vector<AftDfwPrefixUPtr>;

class AftDfwPrefix {
    private:
        uint32_t _prefixLen;
        bool _polarity;
        AftDataPrefix::Ptr _prefixData;
    public :
        static AftDfwPrefixUPtr create(uint16_t len,
                                       bool polarity,
                                       AftDataPrefix::Ptr pfxData) {
            return std::make_unique<AftDfwPrefix>(len, polarity, pfxData);
        }
        //@brief constructor
        AftDfwPrefix(uint16_t len,
                     bool polarity,
                     AftDataPrefix::Ptr pfxData) :
           _prefixLen(len), _polarity(polarity), _prefixData(pfxData) {
        };

        const uint8_t* getBytes() const {
            return _prefixData->dataArray();
        }

        const AftDataPrefix::Ptr getPfxData() const {
            return _prefixData;
        }

        uint32_t getLen() const { return _prefixLen; }

        bool getPolarity() const { return _polarity; }

        void show (std::ostream &out) const {
            std::string polarityStr = (_polarity == true)? "true" : "false";
            if (!out.good()) out.clear();
            out << "\t\t Prefix len(bits)=" << _prefixLen
                << " Polarity=" << polarityStr;
            out << " Prefix = ";
            const uint8_t *pfxBytes = getBytes();
            for (uint32_t i = 0; i < _prefixData->byteLength(); i++) {
                out << pfxBytes[i] << " " ;
            }
            out << std::endl;
        }
};

class AftTermMatch {
    public:
        enum MatchClass {
            RangeMatch,
            PrefixMatch,
            BitOpMatch,
            FlexMatch
        };

        enum MatchType {
            SrcAddr,
            DestAddr,
            Address,
            Protocol,
            SrcPort,
            DestPort,
            Port,
            ForwardingClass,
            PacketLossPriority,
            IcmpType,
            IcmpCode,
            TypeOfService,
            Dscp,
            Precedence,
            SrcMacAddr,
            DestMacAddr,
            EtherType,
            PacketLength,
            TTL,
            Ifl,
            Invalid // Should be last match
        };

    private:
        MatchClass _matchClass;
        MatchType _matchType;
        bool _polarity;
    public:
        MatchClass matchClass() const { return _matchClass; }
        MatchType matchType() const { return _matchType; }
        bool matchPolarity() const { return _polarity; }

        //@brief Constructor
        AftTermMatch(MatchClass mcClass, MatchType mcType,
                        bool polarity):
            _matchClass(mcClass), _matchType(mcType), _polarity(polarity) {};

        //@brief virtual destructor
        virtual ~AftTermMatch() {};

        virtual void show(std::ostream &out) const {
            if (!out.good()) out.clear();
            out << "\t\t  MC field type: " << toStringTermMatchType(matchType())
                << "  MC polarity: " << matchPolarity()
                << std::endl;
        }

        template<typename T>
        inline const T* typedDownCast() {
            return static_cast<T*>(this);
        };

        static std::string toStringTermMatchType(MatchType mcType) {
            switch (mcType) {
                case Protocol:
                    return "protocol";
                case SrcPort:
                    return "src-port";
                case DestPort:
                    return "dst-port";
                case Port:
                    return "port";
                case SrcAddr:
                    return "source-address";
                case DestAddr:
                    return "destination-address";
                case Address:
                    return "address";
                case Dscp:
                    return "dscp";
                case Precedence:
                    return "precedence";
                case TypeOfService:
                    return "TOS";
                case SrcMacAddr:
                    return "src-mac-addr";
                case DestMacAddr:
                    return "dst-mac-addr";
                case EtherType:
                    return "Ether-type";
                case ForwardingClass:
                    return "forwarding-class";
                case PacketLossPriority:
                    return "loss-priority";
                case PacketLength:
                    return "packet-length";
                case TTL:
                    return "ttl";
                case IcmpType:
                    return "icmp-type";
                case IcmpCode:
                    return "icmp-code";
                case Ifl:
                    return "ifl";
                default:
                    return "invalid";
            }
        }// end function toStringMatchType
};
using AftTermMatchUPtr = std::unique_ptr<AftTermMatch>;

class AftDfwRangeMatch;
using AftDfwRangeMatchUPtr = std::unique_ptr<AftDfwRangeMatch>;

class AftDfwRangeMatch:
    public AftTermMatch {
        private:
            AftDfwRangeVec  _rangeVec;
        public:
            static AftDfwRangeMatchUPtr create (MatchType mcType,
                                                bool mcPolar,
                                                AftDfwRangeVec rangeVec) {
                 return std::make_unique<AftDfwRangeMatch>(mcType,
                                                           mcPolar,
                                                           std::move(rangeVec));
            }
            //@brief constructor
            AftDfwRangeMatch(MatchType mcType,
                             bool mcPolar,
                             AftDfwRangeVec rangeVec):
                AftTermMatch(RangeMatch, mcType, mcPolar),  // NOLINT (JuniperAFT/Identifier)
                    _rangeVec(std::move(rangeVec)) {};

            uint32_t min(uint32_t i) const {
                 return getRanges()[i]->min();
            }

            uint32_t max(uint32_t i) const { return getRanges()[i]->max(); }

            size_t size() const { return getRanges().size(); }

            ~AftDfwRangeMatch() {};

            const AftDfwRangeVec& getRanges() const {
                return _rangeVec;
            }

            void insertRange(AftDfwRangeUPtr pRange) {
                _rangeVec.emplace_back(std::move(pRange));
            }

            void insertRange(uint32_t minVal, uint32_t maxVal) {
                _rangeVec.emplace_back(AftDfwRange::create(minVal, maxVal));
            }

            void show (std::ostream &out) const {
                if (!out.good()) out.clear();
                out << "\t\t MC Range: MC-Type =" << toStringTermMatchType(matchType())
                    << std::endl;
                AftTermMatch::show(out);
                for (auto &rangeVal : getRanges()) {
                    out << "\t\t max = " << rangeVal->max();
                    out << "\t\t min = " << rangeVal->min() << std::endl;
                }
            }
};

class AftDfwPrefixMatch;
using AftDfwPrefixMatchUPtr = std::unique_ptr<AftDfwPrefixMatch>;

class AftDfwPrefixMatch:
    public AftTermMatch {
        private:
            AftDfwPrefixVec     _prefixVec;
        public :
            static AftDfwPrefixMatchUPtr create (MatchType mcType,
                                                 bool mcPolarity,
                                                 AftDfwPrefixVec prefixVec) {
                return std::make_unique<AftDfwPrefixMatch>(mcType,
                                                           mcPolarity,
                                                           std::move(prefixVec));
            }
            AftDfwPrefixMatch(MatchType mcType,
                              bool mcPolarity,
                              AftDfwPrefixVec prefixVec):
                AftTermMatch(PrefixMatch, mcType,  // NOLINT (JuniperAFT/Identifier)
                            mcPolarity),
                            _prefixVec(std::move(prefixVec)) {};

            const uint8_t* prefixData(const int i) const {
                return getPrefixes()[i]->getBytes();
            }

            const AftDfwPrefixUPtr& getPrefix(const int i) const {
                return getPrefixes()[i];
            }

            int getPolarity(const int i) const {
                return getPrefixes()[i]->getPolarity();
            }

            uint32_t getPrefixLen(int i) const { return getPrefixes()[i]->getLen(); }

            void insertPrefix(AftDfwPrefixUPtr pPrefix) {
                _prefixVec.emplace_back(std::move(pPrefix));
            }

            void insertPrefix(uint16_t len, AftDataPrefix::Ptr pfxData,
                             bool polarity) {
                _prefixVec.emplace_back(AftDfwPrefix::create(len, polarity, pfxData));
            }

            size_t getSize() const {
                return getPrefixes().size();
            }

            const AftDfwPrefixVec& getPrefixes() const {
                return _prefixVec;
            }

            ~AftDfwPrefixMatch() {};

            void show (std::ostream &out) const {
               if (!out.good()) out.clear();
               out << "\t\t MC Prefix: MC-Type=" << toStringTermMatchType(matchType())
                   << std::endl;
               AftTermMatch::show(out);
               for (auto &pfx : getPrefixes()) {
                   out << " \t\t ------- prefix element "
                       << " ------ " << std::endl;
                   pfx->show(out);
               }
               out << std::endl;
           }
};

//
// TBD Bitfields match types
//
#endif
