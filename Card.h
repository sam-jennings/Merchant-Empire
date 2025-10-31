#ifndef CARD_H
#define CARD_H

#include <string>
#include <vector>

enum class Suit {
    HEARTS,    // Red Lotus Trading Company
    DIAMONDS,  // Golden Caravan Guild
    CLUBS,     // Obsidian Merchants
    SPADES     // Silver Wind Consortium
};

enum class Rank {
    ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, 
    EIGHT, NINE, TEN, JACK, QUEEN, KING
};

class Card {
public:
    Card(Rank rank, Suit suit);
    
    Rank getRank() const { return rank_; }
    Suit getSuit() const { return suit_; }
    int getRankValue() const { return static_cast<int>(rank_); }
    
    std::string toString() const;
    
    bool operator==(const Card& other) const;
    bool operator<(const Card& other) const;
    
private:
    Rank rank_;
    Suit suit_;
};

std::string suitToString(Suit suit);
std::string rankToString(Rank rank);

#endif