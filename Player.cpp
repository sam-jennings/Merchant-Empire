#include "Player.h"
#include <algorithm>
#include <map>
#include <sstream>

Player::Player(int id) : id_(id) {}

void Player::addCard(const Card& card) {
    hand_.push_back(card);
}

void Player::removeCard(const Card& card) {
    auto it = std::find(hand_.begin(), hand_.end(), card);
    if (it != hand_.end()) {
        hand_.erase(it);
    }
}

void Player::addContract(std::shared_ptr<Contract> contract) {
    contracts_.push_back(contract);
}

int Player::getTotalPoints() const {
    int total = 0;
    for (const auto& contract : contracts_) {
        total += contract->getPoints();
    }
    return total;
}

int Player::getTotalSupplyBonus() const {
    int total = 0;
    for (const auto& contract : contracts_) {
        total += contract->getSupplyBonus();
    }
    return total;
}

int Player::getTotalDeals() const {
    int total = 1; // Base deal
    for (const auto& contract : contracts_) {
        int bonus = contract->getBonusDeals();
        if (bonus >= 999) return 999; // Unlimited
        total += bonus;
    }
    return total;
}

std::vector<std::shared_ptr<Contract>> Player::getTradeRoutes() const {
    std::vector<std::shared_ptr<Contract>> routes;
    for (const auto& contract : contracts_) {
        if (contract->hasTradeRights()) {
            routes.push_back(contract);
        }
    }
    return routes;
}

std::vector<Player::PossibleContract> Player::findPossibleContracts() const {
    std::vector<PossibleContract> possible;
    
    findSilkRoads(possible);     // Check Silk Roads first (highest value)
    findPartnerships(possible);
    findTradeRoutes(possible);
    findMonopolies(possible);
    
    std::sort(possible.begin(), possible.end());
    return possible;
}

void Player::findSilkRoads(std::vector<PossibleContract>& contracts) const {
    if (hand_.size() < 3) return;
    
    // Group cards by suit
    std::map<Suit, std::vector<Card>> bySuit;
    for (const auto& card : hand_) {
        bySuit[card.getSuit()].push_back(card);
    }
    
    // For each suit, find sequences
    for (const auto& [suit, cards] : bySuit) {
        if (cards.size() < 3) continue;
        
        std::vector<Card> sortedCards = cards;
        std::sort(sortedCards.begin(), sortedCards.end(), 
            [](const Card& a, const Card& b) { return a.getRankValue() < b.getRankValue(); });
        
        // Find all sequences of length 3-7
        for (size_t start = 0; start < sortedCards.size(); ++start) {
            for (size_t len = 3; len <= std::min(size_t(7), sortedCards.size() - start); ++len) {
                std::vector<Card> sequence;
                for (size_t i = start; i < start + len; ++i) {
                    sequence.push_back(sortedCards[i]);
                }
                
                if (Contract::isValidContract(ContractType::SILK_ROAD, sequence)) {
                    int points = Contract::calculatePoints(ContractType::SILK_ROAD, len);
                    contracts.push_back({ContractType::SILK_ROAD, sequence, points, 
                                        static_cast<double>(points) / len});
                }
            }
        }
    }
}

void Player::findPartnerships(std::vector<PossibleContract>& contracts) const {
    if (hand_.size() < 3) return;
    
    std::map<Suit, std::vector<Card>> bySuit;
    for (const auto& card : hand_) {
        bySuit[card.getSuit()].push_back(card);
    }
    
    for (const auto& [suit, cards] : bySuit) {
        if (cards.size() >= 3) {
            for (size_t len = 3; len <= std::min(size_t(7), cards.size()); ++len) {
                std::vector<Card> partnership(cards.begin(), cards.begin() + len);
                int points = Contract::calculatePoints(ContractType::PARTNERSHIP, len);
                contracts.push_back({ContractType::PARTNERSHIP, partnership, points,
                                    static_cast<double>(points) / len});
            }
        }
    }
}

void Player::findTradeRoutes(std::vector<PossibleContract>& contracts) const {
    if (hand_.size() < 3) return;
    
    std::vector<Card> sortedHand = hand_;
    std::sort(sortedHand.begin(), sortedHand.end(),
        [](const Card& a, const Card& b) { return a.getRankValue() < b.getRankValue(); });
    
    // Find sequences
    for (size_t start = 0; start < sortedHand.size(); ++start) {
        for (size_t len = 3; len <= std::min(size_t(7), sortedHand.size() - start); ++len) {
            std::vector<Card> sequence;
            for (size_t i = start; i < start + len; ++i) {
                sequence.push_back(sortedHand[i]);
            }
            
            if (Contract::isValidContract(ContractType::TRADE_ROUTE, sequence)) {
                int points = Contract::calculatePoints(ContractType::TRADE_ROUTE, len);
                contracts.push_back({ContractType::TRADE_ROUTE, sequence, points,
                                    static_cast<double>(points) / len});
            }
        }
    }
}

void Player::findMonopolies(std::vector<PossibleContract>& contracts) const {
    if (hand_.size() < 3) return;
    
    std::map<Rank, std::vector<Card>> byRank;
    for (const auto& card : hand_) {
        byRank[card.getRank()].push_back(card);
    }
    
    for (const auto& [rank, cards] : byRank) {
        if (cards.size() >= 3) {
            for (size_t len = 3; len <= std::min(size_t(4), cards.size()); ++len) {
                std::vector<Card> monopoly(cards.begin(), cards.begin() + len);
                int points = Contract::calculatePoints(ContractType::MONOPOLY, len);
                contracts.push_back({ContractType::MONOPOLY, monopoly, points,
                                    static_cast<double>(points) / len});
            }
        }
    }
}

Player::PossibleContract Player::selectBestContract() const {
    auto possible = findPossibleContracts();
    if (possible.empty()) {
        return {ContractType::PARTNERSHIP, {}, 0, 0.0};
    }
    return possible[0];
}

bool Player::shouldExtendContract(std::shared_ptr<Contract> contract, const Card& card) const {
    auto cards = contract->getCards();
    cards.push_back(card);
    
    if (Contract::isValidContract(contract->getType(), cards)) {
        int newPoints = Contract::calculatePoints(contract->getType(), cards.size());
        int currentPoints = contract->getPoints();
        return newPoints > currentPoints; // Only extend if we gain points
    }
    return false;
}

std::vector<Card> Player::selectCardsForTrade(int tradeCost, const std::vector<Card>& bazaar) const {
    // Select lowest value cards to trade away
    std::vector<Card> sorted = hand_;
    std::sort(sorted.begin(), sorted.end(),
        [](const Card& a, const Card& b) { return a.getRankValue() < b.getRankValue(); });

    std::vector<Card> toTrade;
    for (int i = 0; i < tradeCost && i < (int)sorted.size(); ++i) {
        toTrade.push_back(sorted[i]);
    }
    return toTrade;
}

Player::VoteBreakdown Player::calculateVoteBreakdown() const {
    VoteBreakdown breakdown;
    breakdown.guildStanding = {
        {Suit::HEARTS, 0},
        {Suit::DIAMONDS, 0},
        {Suit::CLUBS, 0},
        {Suit::SPADES, 0}
    };

    for (const auto& contract : contracts_) {
        if (!contract) {
            continue;
        }

        auto type = contract->getType();
        int size = contract->getSize();
        const auto& cards = contract->getCards();

        switch (type) {
            case ContractType::PARTNERSHIP: {
                if (!cards.empty()) {
                    Suit suit = cards.front().getSuit();
                    breakdown.guildStanding[suit] += size;

                    if (Contract::isValidContract(ContractType::SILK_ROAD, cards)) {
                        breakdown.silkRoadMarks += 1;
                    }
                }
                break;
            }
            case ContractType::SILK_ROAD: {
                if (!cards.empty()) {
                    Suit suit = cards.front().getSuit();
                    breakdown.guildStanding[suit] += size;
                }
                breakdown.caravanCapacity += size;
                breakdown.silkRoadMarks += 1;
                break;
            }
            case ContractType::TRADE_ROUTE: {
                breakdown.caravanCapacity += size;
                break;
            }
            case ContractType::MONOPOLY: {
                breakdown.marketShare += size;
                break;
            }
        }
    }

    return breakdown;
}

std::string Player::toString() const {
    std::ostringstream oss;
    oss << getName() << " - Hand: " << hand_.size() << " cards, "
        << contracts_.size() << " contracts, "
        << getTotalPoints() << " points";
    return oss.str();
}