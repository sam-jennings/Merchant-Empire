#ifndef GAME_H
#define GAME_H

#include "Card.h"
#include "Contract.h"
#include "Council.h"
#include "Player.h"
#include <vector>
#include <memory>
#include <random>

class Game {
public:
    Game(int numPlayers = 4, unsigned int seed = 0);

    void play();
    void printResults() const;
    void setCouncilStrategy(int playerId, council::VotingProfile profile);

private:
    int numPlayers_;
    int currentRound_;
    std::vector<std::shared_ptr<Player>> players_;
    std::vector<Card> supply_;
    std::vector<Card> bazaar_;
    std::mt19937 rng_;
    mutable council::StrategyAssignments councilStrategies_;
    
    // Setup
    void initializeDeck();
    void dealCards();
    void setupBazaar();
    void shuffleDeck(std::vector<Card>& deck);
    
    // Turn phases
    void playTurn(std::shared_ptr<Player> player);
    void supplyPhase(std::shared_ptr<Player> player);
    void barterPhase(std::shared_ptr<Player> player);
    void dealPhase(std::shared_ptr<Player> player);
    
    // Helper methods
    Card drawFromSupply();
    Card takeFromBazaar(int index);
    void replaceInBazaar(int index);
    bool isGameOver() const { return supply_.empty(); }
    
    std::shared_ptr<Player> getWinner() const;

    void printGameState() const;
    void printPlayerState(const std::shared_ptr<Player>& player) const;
    void printVoteBreakdown(const std::vector<std::shared_ptr<Player>>& sortedPlayers) const;
};

#endif
