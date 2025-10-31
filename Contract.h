#ifndef CONTRACT_H
#define CONTRACT_H

#include "Card.h"
#include <vector>
#include <string>

enum class ContractType {
    PARTNERSHIP,  // Same suit
    TRADE_ROUTE,  // Sequential ranks
    MONOPOLY,     // Matching ranks
    SILK_ROAD     // Sequential same suit
};

class Contract {
public:
    Contract(ContractType type, const std::vector<Card>& cards, int roundCreated);
    
    ContractType getType() const { return type_; }
    const std::vector<Card>& getCards() const { return cards_; }
    int getPoints() const { return points_; }
    int getRoundCreated() const { return roundCreated_; }
    int getSize() const { return cards_.size(); }
    
    // Benefits
    int getSupplyBonus() const;  // For Partnerships and Silk Roads
    bool hasTradeRights() const; // For Trade Routes and Silk Roads
    int getTradeCost() const;    // Cards to give for Bazaar exchange
    int getBonusDeals() const;   // For Monopolies
    
    void addCards(const std::vector<Card>& newCards);
    
    std::string toString() const;
    std::string getTypeString() const;
    
    static int calculatePoints(ContractType type, int cardCount);
    static bool isValidContract(ContractType type, const std::vector<Card>& cards);
    
private:
    ContractType type_;
    std::vector<Card> cards_;
    int points_;
    int roundCreated_;
    
    void calculatePoints();
};

std::string contractTypeToString(ContractType type);

#endif