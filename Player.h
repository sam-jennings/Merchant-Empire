#ifndef PLAYER_H
#define PLAYER_H

#include "Card.h"
#include "Contract.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

class Player {
public:
    Player(int id);
    
    int getId() const { return id_; }
    std::string getName() const { return "Player " + std::to_string(id_); }
    
    // Hand management
    void addCard(const Card& card);
    void removeCard(const Card& card);
    const std::vector<Card>& getHand() const { return hand_; }
    int getHandSize() const { return hand_.size(); }
    
    // Contract management
    void addContract(std::shared_ptr<Contract> contract);
    const std::vector<std::shared_ptr<Contract>>& getContracts() const { return contracts_; }
    int getTotalPoints() const;
    
    // Benefits
    int getTotalSupplyBonus() const;
    int getTotalDeals() const;
    std::vector<std::shared_ptr<Contract>> getTradeRoutes() const;
    
    // AI Strategy
    struct PossibleContract {
        ContractType type;
        std::vector<Card> cards;
        int points;
        double efficiency; // points per card

        bool operator<(const PossibleContract& other) const {
            return efficiency > other.efficiency; // Higher efficiency first
        }
    };

    struct VoteBreakdown {
        std::map<Suit, int> guildStanding;
        int caravanCapacity = 0;
        int marketShare = 0;
        int silkRoadMarks = 0;
    };

    std::vector<PossibleContract> findPossibleContracts() const;
    PossibleContract selectBestContract() const;
    bool shouldExtendContract(std::shared_ptr<Contract> contract, const Card& card) const;
    std::vector<Card> selectCardsForTrade(int tradeCost, const std::vector<Card>& bazaar) const;

    VoteBreakdown calculateVoteBreakdown() const;
    
    std::string toString() const;
    
private:
    int id_;
    std::vector<Card> hand_;
    std::vector<std::shared_ptr<Contract>> contracts_;
    
    void findPartnerships(std::vector<PossibleContract>& contracts) const;
    void findTradeRoutes(std::vector<PossibleContract>& contracts) const;
    void findMonopolies(std::vector<PossibleContract>& contracts) const;
    void findSilkRoads(std::vector<PossibleContract>& contracts) const;
};

#endif