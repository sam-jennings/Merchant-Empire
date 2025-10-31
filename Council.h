#pragma once

#include "Player.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace council {

enum class HonorType {
    MONO_SUIT,
    POLY_SUIT,
    ROUTE_ONLY,
    MARKET_ONLY,
    MONO_TRACK,
    POLY_TRACK
};

struct CouncilHonor {
    HonorType type;
    std::string name;
    std::map<int, int> victoryPointsByPlayerCount;

    int getVictoryPoints(int numPlayers) const;
};

struct TrackResources {
    std::map<Suit, int> suits;
    int route = 0;
    int market = 0;

    int totalVotes() const;
};

struct HonorCommitment {
    std::map<Suit, int> suitVotes;
    int routeVotes = 0;
    int marketVotes = 0;
    int wildVotes = 0;
    bool legal = false;

    int totalFromTracks() const;
    int totalWithWild() const;
    int sourcesUsed() const;
};

struct PlayerCouncilState {
    std::shared_ptr<Player> player;
    TrackResources original;
    TrackResources remaining;
    int silkRoadMarks = 0;
    std::map<HonorType, HonorCommitment> commitments;
};

struct HonorOutcome {
    CouncilHonor honor;
    std::vector<std::shared_ptr<Player>> winners;
    double vpPerWinner = 0.0;
    std::string resolutionNote;
    std::map<int, HonorCommitment> commitmentsByPlayer;
};

struct CouncilResults {
    std::vector<PlayerCouncilState> states;
    std::map<int, double> honorPoints;
    std::vector<HonorOutcome> outcomes;
};

enum class VotingProfile {
    None,
    Maximizer,
    Sniper,
    Spreader,
    Chaos
};

struct StrategyConfig {
    VotingProfile profile = VotingProfile::None;
};

using StrategyAssignments = std::map<int, StrategyConfig>;

const std::vector<CouncilHonor>& getCouncilHonors();

CouncilResults calculateBasicCouncilResults(const std::vector<std::shared_ptr<Player>>& players,
                                            int numPlayers);

CouncilResults resolveCouncil(const std::vector<std::shared_ptr<Player>>& players,
                              int numPlayers,
                              const StrategyAssignments& assignments = {});

void printCouncilHonorResults(const CouncilResults& results, int numPlayers);

std::string formatScore(double value);
std::string formatDecimal(double value, int precision = 2);
std::string commitmentSummary(const HonorCommitment& commitment);

double computeUtilization(const TrackResources& original, const HonorCommitment& commitment);
double computeFocus(const TrackResources& original, const HonorCommitment& commitment);

} // namespace council

