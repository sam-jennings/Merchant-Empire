#include "Card.h"

Card::Card(Rank rank, Suit suit) : rank_(rank), suit_(suit) {}

std::string Card::toString() const {
    return rankToString(rank_) + " of " + suitToString(suit_);
}

bool Card::operator==(const Card& other) const {
    return rank_ == other.rank_ && suit_ == other.suit_;
}

bool Card::operator<(const Card& other) const {
    if (suit_ != other.suit_) {
        return suit_ < other.suit_;
    }
    return rank_ < other.rank_;
}

std::string suitToString(Suit suit) {
    switch (suit) {
        case Suit::HEARTS: return "Hearts";
        case Suit::DIAMONDS: return "Diamonds";
        case Suit::CLUBS: return "Clubs";
        case Suit::SPADES: return "Spades";
    }
    return "Unknown";
}

std::string rankToString(Rank rank) {
    switch (rank) {
        case Rank::ACE: return "A";
        case Rank::TWO: return "2";
        case Rank::THREE: return "3";
        case Rank::FOUR: return "4";
        case Rank::FIVE: return "5";
        case Rank::SIX: return "6";
        case Rank::SEVEN: return "7";
        case Rank::EIGHT: return "8";
        case Rank::NINE: return "9";
        case Rank::TEN: return "10";
        case Rank::JACK: return "J";
        case Rank::QUEEN: return "Q";
        case Rank::KING: return "K";
    }
    return "?";
}