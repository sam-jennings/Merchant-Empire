#include "Contract.h"
#include <algorithm>
#include <sstream>
#include <set>

Contract::Contract(ContractType type, const std::vector<Card>& cards, int roundCreated)
    : type_(type), cards_(cards), roundCreated_(roundCreated) {
    calculatePoints();
}

void Contract::calculatePoints() {
    points_ = calculatePoints(type_, cards_.size());
}

int Contract::calculatePoints(ContractType type, int cardCount) {
    switch (type) {
        case ContractType::PARTNERSHIP:
            switch (cardCount) {
                case 3: return 3;
                case 4: return 5;
                case 5: return 8;
                case 6: return 12;
                case 7: return 18;
                case 8: return 22;
                case 9: return 27;
                default: return 0;
            }
        case ContractType::TRADE_ROUTE:
            switch (cardCount) {
                case 3: return 4;
                case 4: return 6;
                case 5: return 10;
                case 6: return 15;
                case 7: return 22;
                default: return 0;
            }
        case ContractType::MONOPOLY:
            switch (cardCount) {
                case 3: return 5;
                case 4: return 12;
                default: return 0;
            }
        case ContractType::SILK_ROAD:
            switch (cardCount) {
                case 3: return 7;
                case 4: return 11;
                case 5: return 18;
                case 6: return 27;
                case 7: return 40;
                default: return 0;
            }
    }
    return 0;
}

int Contract::getSupplyBonus() const {
    if (type_ == ContractType::PARTNERSHIP || type_ == ContractType::SILK_ROAD) {
        int size = cards_.size();
        if (size >= 3 && size <= 5) return 1;
        if (size >= 6 && size <= 7) return 2;
        if (size >= 8) return 3;
    }
    return 0;
}

bool Contract::hasTradeRights() const {
    return type_ == ContractType::TRADE_ROUTE || type_ == ContractType::SILK_ROAD;
}

int Contract::getTradeCost() const {
    if (!hasTradeRights()) return 0;
    return (cards_.size() == 3) ? 2 : 1;
}

int Contract::getBonusDeals() const {
    if (type_ == ContractType::MONOPOLY) {
        if (cards_.size() == 3) return 1;
        if (cards_.size() == 4) return 999; // Unlimited
    }
    return 0;
}

void Contract::addCards(const std::vector<Card>& newCards) {
    cards_.insert(cards_.end(), newCards.begin(), newCards.end());
    calculatePoints();
}

std::string Contract::toString() const {
    std::ostringstream oss;
    oss << getTypeString() << " (" << cards_.size() << " cards, " 
        << points_ << " pts, Round " << roundCreated_ << "): ";
    for (size_t i = 0; i < cards_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << cards_[i].toString();
    }
    return oss.str();
}

std::string Contract::getTypeString() const {
    return contractTypeToString(type_);
}

bool Contract::isValidContract(ContractType type, const std::vector<Card>& cards) {
    if (cards.empty()) return false;
    
    switch (type) {
        case ContractType::PARTNERSHIP: {
            if (cards.size() < 3 || cards.size() > 7) return false;
            Suit suit = cards[0].getSuit();
            for (const auto& card : cards) {
                if (card.getSuit() != suit) return false;
            }
            return true;
        }
        
        case ContractType::TRADE_ROUTE: {
            if (cards.size() < 3 || cards.size() > 7) return false;
            std::vector<int> ranks;
            for (const auto& card : cards) {
                ranks.push_back(card.getRankValue());
            }
            std::sort(ranks.begin(), ranks.end());
            
            // Check for duplicates
            for (size_t i = 1; i < ranks.size(); ++i) {
                if (ranks[i] == ranks[i-1]) return false;
            }
            
            // Check for sequential
            bool isSequential = true;
            for (size_t i = 1; i < ranks.size(); ++i) {
                if (ranks[i] != ranks[i-1] + 1) {
                    isSequential = false;
                    break;
                }
            }
            
            if (isSequential) return true;
            
            // Check for wrap-around (Q-K-A) - must be exactly these at the end
            if (ranks.size() >= 3) {
                if (ranks[0] == 1) { // Has an Ace
                    // Check if the rest form Q-K at the end
                    bool hasQK = true;
                    for (size_t i = 1; i < ranks.size(); ++i) {
                        if (ranks[i] != 11 + i) { // Should be 12 (Q), 13 (K)
                            hasQK = false;
                            break;
                        }
                    }
                    if (hasQK && ranks[ranks.size()-1] == 13) return true;
                }
            }
            
            return false;
        }
        
        case ContractType::MONOPOLY: {
            if (cards.size() < 3 || cards.size() > 4) return false;
            Rank rank = cards[0].getRank();
            for (const auto& card : cards) {
                if (card.getRank() != rank) return false;
            }
            return true;
        }
        
        case ContractType::SILK_ROAD: {
            if (cards.size() < 3 || cards.size() > 7) return false;
            Suit suit = cards[0].getSuit();
            std::vector<int> ranks;
            for (const auto& card : cards) {
                if (card.getSuit() != suit) return false;
                ranks.push_back(card.getRankValue());
            }
            std::sort(ranks.begin(), ranks.end());
            
            // Check for duplicates
            for (size_t i = 1; i < ranks.size(); ++i) {
                if (ranks[i] == ranks[i-1]) return false;
            }
            
            // Check for sequential
            bool isSequential = true;
            for (size_t i = 1; i < ranks.size(); ++i) {
                if (ranks[i] != ranks[i-1] + 1) {
                    isSequential = false;
                    break;
                }
            }
            
            if (isSequential) return true;
            
            // Check for wrap-around (Q-K-A)
            if (ranks.size() >= 3) {
                if (ranks[0] == 1) { // Has an Ace
                    bool hasQK = true;
                    for (size_t i = 1; i < ranks.size(); ++i) {
                        if (ranks[i] != 11 + i) {
                            hasQK = false;
                            break;
                        }
                    }
                    if (hasQK && ranks[ranks.size()-1] == 13) return true;
                }
            }
            
            return false;
        }
    }
    return false;
}

std::string contractTypeToString(ContractType type) {
    switch (type) {
        case ContractType::PARTNERSHIP: return "Partnership";
        case ContractType::TRADE_ROUTE: return "Trade Route";
        case ContractType::MONOPOLY: return "Monopoly";
        case ContractType::SILK_ROAD: return "Silk Road";
    }
    return "Unknown";
}