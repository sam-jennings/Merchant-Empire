#include "Game.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

Game::Game(int numPlayers, unsigned int seed) 
    : numPlayers_(numPlayers), currentRound_(0) {
    
    if (seed == 0) {
        std::random_device rd;
        rng_.seed(rd());
    } else {
        rng_.seed(seed);
    }
    
    // Create players
    for (int i = 0; i < numPlayers_; ++i) {
        players_.push_back(std::make_shared<Player>(i + 1));
    }
    
    initializeDeck();
    dealCards();
    setupBazaar();
}

void Game::initializeDeck() {
    // Create standard 52-card deck (no jokers)
    std::vector<Suit> suits = {Suit::HEARTS, Suit::DIAMONDS, Suit::CLUBS, Suit::SPADES};
    std::vector<Rank> ranks = {
        Rank::ACE, Rank::TWO, Rank::THREE, Rank::FOUR, Rank::FIVE, Rank::SIX,
        Rank::SEVEN, Rank::EIGHT, Rank::NINE, Rank::TEN, Rank::JACK, Rank::QUEEN, Rank::KING
    };
    
    supply_.clear();
    for (Suit suit : suits) {
        for (Rank rank : ranks) {
            supply_.push_back(Card(rank, suit));
        }
    }
    
    shuffleDeck(supply_);
}

void Game::shuffleDeck(std::vector<Card>& deck) {
    std::shuffle(deck.begin(), deck.end(), rng_);
}

void Game::dealCards() {
    int cardsPerPlayer = (numPlayers_ == 3) ? 7 : 6;
    
    for (int i = 0; i < cardsPerPlayer; ++i) {
        for (auto& player : players_) {
            if (!supply_.empty()) {
                player->addCard(drawFromSupply());
            }
        }
    }
}

void Game::setupBazaar() {
    for (int i = 0; i < 5; ++i) {
        if (!supply_.empty()) {
            bazaar_.push_back(drawFromSupply());
        }
    }
}

Card Game::drawFromSupply() {
    if (supply_.empty()) {
        throw std::runtime_error("Supply is empty!");
    }
    Card card = supply_.back();
    supply_.pop_back();
    return card;
}

Card Game::takeFromBazaar(int index) {
    if (index < 0 || index >= (int)bazaar_.size()) {
        throw std::out_of_range("Invalid bazaar index");
    }
    Card card = bazaar_[index];
    replaceInBazaar(index);
    return card;
}

void Game::replaceInBazaar(int index) {
    if (!supply_.empty()) {
        bazaar_[index] = drawFromSupply();
    } else {
        bazaar_.erase(bazaar_.begin() + index);
    }
}

void Game::play() {
    std::cout << "=== MERCHANT EMPIRE SIMULATION ===" << std::endl;
    std::cout << "Starting game with " << numPlayers_ << " players" << std::endl;
    std::cout << "Supply: " << supply_.size() << " cards remaining" << std::endl;
    std::cout << std::endl;
    
    // Main game loop
    while (!isGameOver()) {
        currentRound_++;
        
        for (auto& player : players_) {
            if (isGameOver()) break;
            playTurn(player);
        }
    }
    
    // Final round for remaining players
    std::cout << "\n=== FINAL ROUND ===" << std::endl;
    for (auto& player : players_) {
        std::cout << "\n" << player->getName() << "'s final turn:" << std::endl;
        // Players can still make deals with remaining cards
        dealPhase(player);
    }
    
    printResults();
}

void Game::playTurn(std::shared_ptr<Player> player) {
    supplyPhase(player);
    //barterPhase(player);
    dealPhase(player);
}

void Game::supplyPhase(std::shared_ptr<Player> player) {
    // Base acquisition
    int cardsDrawn = 0;
    if (supply_.size() >= 1) {
        player->addCard(drawFromSupply());
        cardsDrawn = 1;
    } 
    
    // Supply agreements from partnerships
    int supplyBonus = player->getTotalSupplyBonus();
    for (int i = 0; i < supplyBonus && !supply_.empty(); ++i) {
        player->addCard(drawFromSupply());
        cardsDrawn++;
    }
}

void Game::barterPhase(std::shared_ptr<Player> player) {
    auto tradeRoutes = player->getTradeRoutes();
    
    for (auto& route : tradeRoutes) {
        if (bazaar_.empty() || player->getHandSize() < route->getTradeCost()) {
            continue;
        }
        
        // Simple strategy: take the highest rank card from bazaar
        int bestIndex = 0;
        int bestRank = bazaar_[0].getRankValue();
        for (size_t i = 1; i < bazaar_.size(); ++i) {
            if (bazaar_[i].getRankValue() > bestRank) {
                bestRank = bazaar_[i].getRankValue();
                bestIndex = i;
            }
        }
        
        // Trade away low-value cards
        auto cardsToTrade = player->selectCardsForTrade(route->getTradeCost(), bazaar_);
        for (const auto& card : cardsToTrade) {
            player->removeCard(card);
        }
        
        // Take from bazaar
        Card takenCard = takeFromBazaar(bestIndex);
        player->addCard(takenCard);
    }
}

void Game::dealPhase(std::shared_ptr<Player> player) {
    int availableDeals = player->getTotalDeals();
    
    for (int dealNum = 0; dealNum < availableDeals; ++dealNum) {
        auto bestContract = player->selectBestContract();
        
        if (bestContract.points == 0 || bestContract.cards.empty()) {
            break; // No valid contracts to make
        }
        
        // Check if we should extend an existing contract instead
        bool extended = false;
        for (auto& existingContract : player->getContracts()) {
            for (const auto& card : bestContract.cards) {
                if (player->shouldExtendContract(existingContract, card)) {
                    existingContract->addCards({card});
                    player->removeCard(card);
                    extended = true;
                    
                    std::cout << "  Round " << currentRound_ << ": " 
                              << player->getName() << " extended " 
                              << existingContract->getTypeString() 
                              << " (now " << existingContract->getSize() << " cards, "
                              << existingContract->getPoints() << " pts)" << std::endl;
                    break;
                }
            }
            if (extended) break;
        }
        
        if (!extended) {
            // Create new contract
            auto newContract = std::make_shared<Contract>(
                bestContract.type, bestContract.cards, currentRound_
            );
            player->addContract(newContract);
            
            // Remove cards from hand
            for (const auto& card : bestContract.cards) {
                player->removeCard(card);
            }
            
            std::cout << "  Round " << currentRound_ << ": " 
                      << player->getName() << " signed " 
                      << newContract->getTypeString() 
                      << " (" << newContract->getSize() << " cards, "
                      << newContract->getPoints() << " pts)" << std::endl;
        }
    }
}

std::shared_ptr<Player> Game::getWinner() const {
    auto winner = players_[0];
    int maxPoints = winner->getTotalPoints();
    
    for (size_t i = 1; i < players_.size(); ++i) {
        int points = players_[i]->getTotalPoints();
        if (points > maxPoints) {
            maxPoints = points;
            winner = players_[i];
        } else if (points == maxPoints) {
            // Tiebreaker: most contracts
            if (players_[i]->getContracts().size() > winner->getContracts().size()) {
                winner = players_[i];
            }
        }
    }
    
    return winner;
}

void Game::printResults() const {
    std::cout << "\n\n=== GAME OVER ===" << std::endl;
    std::cout << "Total Rounds: " << currentRound_ << std::endl;
    std::cout << "\n=== FINAL STANDINGS ===" << std::endl;
    
    // Sort players by points
    std::vector<std::shared_ptr<Player>> sortedPlayers = players_;
    std::sort(sortedPlayers.begin(), sortedPlayers.end(),
        [](const std::shared_ptr<Player>& a, const std::shared_ptr<Player>& b) {
            int pointsA = a->getTotalPoints();
            int pointsB = b->getTotalPoints();
            if (pointsA != pointsB) return pointsA > pointsB;
            return a->getContracts().size() > b->getContracts().size();
        });
    
    for (size_t i = 0; i < sortedPlayers.size(); ++i) {
        auto player = sortedPlayers[i];
        std::cout << "\n" << (i + 1) << ". " << player->getName() 
                  << " - " << player->getTotalPoints() << " points"
                  << " (" << player->getContracts().size() << " contracts)" << std::endl;
        
        std::cout << "   Contracts:" << std::endl;
        for (const auto& contract : player->getContracts()) {
            std::cout << "   - " << contract->toString() << std::endl;
        }
    }
    
    auto winner = sortedPlayers[0];
    std::cout << "\n*** WINNER: " << winner->getName() 
              << " with " << winner->getTotalPoints() << " points! ***" << std::endl;
}