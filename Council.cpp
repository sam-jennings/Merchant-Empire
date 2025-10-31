#include "Council.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <tuple>

namespace council {

namespace {

bool isPolyHonor(HonorType type) {
    return type == HonorType::POLY_SUIT || type == HonorType::POLY_TRACK;
}

struct PolyTrackResource {
    enum class Kind { SUIT, ROUTE, MARKET } kind;
    Suit suit = Suit::HEARTS;
    int available = 0;
    int allocated = 0;
};

std::optional<HonorCommitment> allocateMonoSuit(int votesNeeded, TrackResources& available) {
    votesNeeded = std::max(votesNeeded, 1);
    Suit bestSuit = Suit::HEARTS;
    int bestVotes = -1;
    for (const auto& [suit, value] : available.suits) {
        if (value > bestVotes) {
            bestVotes = value;
            bestSuit = suit;
        }
    }
    if (bestVotes < votesNeeded) {
        return std::nullopt;
    }
    HonorCommitment commitment;
    commitment.legal = true;
    commitment.suitVotes[bestSuit] = votesNeeded;
    available.suits[bestSuit] -= votesNeeded;
    return commitment;
}

std::optional<HonorCommitment> allocateMonoTrack(int votesNeeded, TrackResources& available) {
    votesNeeded = std::max(votesNeeded, 1);
    enum class TrackChoice { SUIT, ROUTE, MARKET };
    TrackChoice choice = TrackChoice::SUIT;
    Suit chosenSuit = Suit::HEARTS;
    int bestVotes = -1;

    for (const auto& [suit, value] : available.suits) {
        if (value > bestVotes) {
            bestVotes = value;
            chosenSuit = suit;
            choice = TrackChoice::SUIT;
        }
    }
    if (available.route > bestVotes) {
        bestVotes = available.route;
        choice = TrackChoice::ROUTE;
    }
    if (available.market > bestVotes) {
        bestVotes = available.market;
        choice = TrackChoice::MARKET;
    }

    if (bestVotes < votesNeeded) {
        return std::nullopt;
    }

    HonorCommitment commitment;
    commitment.legal = true;
    switch (choice) {
        case TrackChoice::SUIT:
            commitment.suitVotes[chosenSuit] = votesNeeded;
            available.suits[chosenSuit] -= votesNeeded;
            break;
        case TrackChoice::ROUTE:
            commitment.routeVotes = votesNeeded;
            available.route -= votesNeeded;
            break;
        case TrackChoice::MARKET:
            commitment.marketVotes = votesNeeded;
            available.market -= votesNeeded;
            break;
    }
    return commitment;
}

std::optional<HonorCommitment> allocatePolySuit(int votesNeeded, TrackResources& available) {
    votesNeeded = std::max(votesNeeded, 2);
    std::vector<PolyTrackResource> resources;
    for (const auto& [suit, value] : available.suits) {
        if (value > 0) {
            resources.push_back({PolyTrackResource::Kind::SUIT, suit, value, 0});
        }
    }
    if (resources.size() < 2) {
        return std::nullopt;
    }
    int totalAvailable = 0;
    for (const auto& res : resources) {
        totalAvailable += res.available;
    }
    if (totalAvailable < votesNeeded) {
        return std::nullopt;
    }

    std::sort(resources.begin(), resources.end(), [](const auto& a, const auto& b) {
        return a.available > b.available;
    });

    for (size_t i = 0; i < 2; ++i) {
        resources[i].allocated += 1;
        resources[i].available -= 1;
        votesNeeded -= 1;
    }

    while (votesNeeded > 0) {
        std::sort(resources.begin(), resources.end(), [](const auto& a, const auto& b) {
            return a.available > b.available;
        });
        if (resources.front().available <= 0) {
            return std::nullopt;
        }
        resources.front().allocated += 1;
        resources.front().available -= 1;
        votesNeeded -= 1;
    }

    HonorCommitment commitment;
    commitment.legal = true;
    for (const auto& res : resources) {
        if (res.allocated > 0) {
            commitment.suitVotes[res.suit] = res.allocated;
            available.suits[res.suit] -= res.allocated;
        }
    }
    return commitment;
}

std::optional<HonorCommitment> allocatePolyTrack(int votesNeeded, TrackResources& available) {
    votesNeeded = std::max(votesNeeded, 2);
    std::vector<PolyTrackResource> resources;
    for (const auto& [suit, value] : available.suits) {
        if (value > 0) {
            resources.push_back({PolyTrackResource::Kind::SUIT, suit, value, 0});
        }
    }
    if (available.route > 0) {
        resources.push_back({PolyTrackResource::Kind::ROUTE, Suit::HEARTS, available.route, 0});
    }
    if (available.market > 0) {
        resources.push_back({PolyTrackResource::Kind::MARKET, Suit::HEARTS, available.market, 0});
    }
    if (resources.size() < 2) {
        return std::nullopt;
    }

    int totalAvailable = 0;
    for (const auto& res : resources) {
        totalAvailable += res.available;
    }
    if (totalAvailable < votesNeeded) {
        return std::nullopt;
    }

    std::sort(resources.begin(), resources.end(), [](const auto& a, const auto& b) {
        return a.available > b.available;
    });

    for (size_t i = 0; i < 2; ++i) {
        resources[i].allocated += 1;
        resources[i].available -= 1;
        votesNeeded -= 1;
    }

    while (votesNeeded > 0) {
        std::sort(resources.begin(), resources.end(), [](const auto& a, const auto& b) {
            return a.available > b.available;
        });
        if (resources.front().available <= 0) {
            return std::nullopt;
        }
        resources.front().allocated += 1;
        resources.front().available -= 1;
        votesNeeded -= 1;
    }

    HonorCommitment commitment;
    commitment.legal = true;
    for (const auto& res : resources) {
        if (res.allocated > 0) {
            switch (res.kind) {
                case PolyTrackResource::Kind::SUIT:
                    commitment.suitVotes[res.suit] = res.allocated;
                    available.suits[res.suit] -= res.allocated;
                    break;
                case PolyTrackResource::Kind::ROUTE:
                    commitment.routeVotes += res.allocated;
                    available.route -= res.allocated;
                    break;
                case PolyTrackResource::Kind::MARKET:
                    commitment.marketVotes += res.allocated;
                    available.market -= res.allocated;
                    break;
            }
        }
    }
    return commitment;
}

std::optional<HonorCommitment> allocateHonor(HonorType type, int votesNeeded, TrackResources& available) {
    switch (type) {
        case HonorType::MONO_SUIT:
            return allocateMonoSuit(votesNeeded, available);
        case HonorType::POLY_SUIT:
            return allocatePolySuit(votesNeeded, available);
        case HonorType::ROUTE_ONLY: {
            votesNeeded = std::max(votesNeeded, 1);
            if (available.route < votesNeeded) {
                return std::nullopt;
            }
            HonorCommitment commitment;
            commitment.legal = true;
            commitment.routeVotes = votesNeeded;
            available.route -= votesNeeded;
            return commitment;
        }
        case HonorType::MARKET_ONLY: {
            votesNeeded = std::max(votesNeeded, 1);
            if (available.market < votesNeeded) {
                return std::nullopt;
            }
            HonorCommitment commitment;
            commitment.legal = true;
            commitment.marketVotes = votesNeeded;
            available.market -= votesNeeded;
            return commitment;
        }
        case HonorType::MONO_TRACK:
            return allocateMonoTrack(votesNeeded, available);
        case HonorType::POLY_TRACK:
            return allocatePolyTrack(votesNeeded, available);
    }
    return std::nullopt;
}

int maxSuitVotes(const TrackResources& resources) {
    int best = 0;
    for (const auto& [suit, value] : resources.suits) {
        best = std::max(best, value);
    }
    return best;
}

int getMaxContribution(const TrackResources& resources, HonorType type) {
    switch (type) {
        case HonorType::MONO_SUIT:
            return maxSuitVotes(resources);
        case HonorType::POLY_SUIT: {
            int suitsWithVotes = 0;
            int total = 0;
            for (const auto& [suit, value] : resources.suits) {
                if (value > 0) {
                    suitsWithVotes++;
                    total += value;
                }
            }
            return suitsWithVotes >= 2 ? total : 0;
        }
        case HonorType::ROUTE_ONLY:
            return resources.route;
        case HonorType::MARKET_ONLY:
            return resources.market;
        case HonorType::MONO_TRACK: {
            int best = std::max(resources.route, resources.market);
            for (const auto& [suit, value] : resources.suits) {
                best = std::max(best, value);
            }
            return best;
        }
        case HonorType::POLY_TRACK: {
            int tracksWithVotes = 0;
            int total = 0;
            for (const auto& [suit, value] : resources.suits) {
                if (value > 0) {
                    tracksWithVotes++;
                    total += value;
                }
            }
            if (resources.route > 0) {
                tracksWithVotes++;
                total += resources.route;
            }
            if (resources.market > 0) {
                tracksWithVotes++;
                total += resources.market;
            }
            return tracksWithVotes >= 2 ? total : 0;
        }
    }
    return 0;
}

std::map<HonorType, int> computeRequiredVotes(const std::vector<PlayerCouncilState>& states,
                                               const PlayerCouncilState& current,
                                               const std::vector<CouncilHonor>& honors) {
    std::map<HonorType, int> votesNeeded;
    for (const auto& honor : honors) {
        int maxOpposition = 0;
        for (const auto& opponent : states) {
            if (opponent.player->getId() == current.player->getId()) {
                continue;
            }
            maxOpposition = std::max(maxOpposition, getMaxContribution(opponent.original, honor.type));
        }
        int required = maxOpposition + 1;
        if (isPolyHonor(honor.type)) {
            required = std::max(required, 2);
        } else {
            required = std::max(required, 1);
        }
        votesNeeded[honor.type] = required;
    }
    return votesNeeded;
}

class VotingStrategy {
public:
    virtual ~VotingStrategy() = default;
    virtual void allocate(PlayerCouncilState& state,
                          const std::vector<PlayerCouncilState>& roster,
                          const std::vector<CouncilHonor>& honors,
                          int numPlayers) = 0;
};

class MaximizerStrategy : public VotingStrategy {
public:
    void allocate(PlayerCouncilState& state,
                  const std::vector<PlayerCouncilState>& roster,
                  const std::vector<CouncilHonor>& honors,
                  int numPlayers) override {
        auto votesNeeded = computeRequiredVotes(roster, state, honors);

        std::vector<const CouncilHonor*> honorOrder;
        honorOrder.reserve(honors.size());
        for (const auto& honor : honors) {
            honorOrder.push_back(&honor);
        }

        std::sort(honorOrder.begin(), honorOrder.end(), [&](const CouncilHonor* a, const CouncilHonor* b) {
            int neededA = votesNeeded[a->type];
            int neededB = votesNeeded[b->type];

            int vpA = a->getVictoryPoints(numPlayers);
            int vpB = b->getVictoryPoints(numPlayers);

            double efficiencyA = neededA > 0 ? static_cast<double>(vpA) / neededA : 0.0;
            double efficiencyB = neededB > 0 ? static_cast<double>(vpB) / neededB : 0.0;

            if (std::fabs(efficiencyA - efficiencyB) > 1e-6) {
                return efficiencyA > efficiencyB;
            }
            if (vpA != vpB) {
                return vpA > vpB;
            }
            return static_cast<int>(a->type) < static_cast<int>(b->type);
        });

        TrackResources available = state.remaining;
        for (const auto* honor : honorOrder) {
            int requiredVotes = votesNeeded[honor->type];
            if (requiredVotes <= 0) {
                continue;
            }
            auto commitment = allocateHonor(honor->type, requiredVotes, available);
            if (commitment) {
                state.commitments[honor->type] = *commitment;
            }
        }
        state.remaining = available;
    }
};

class SniperStrategy : public VotingStrategy {
public:
    void allocate(PlayerCouncilState& state,
                  const std::vector<PlayerCouncilState>& roster,
                  const std::vector<CouncilHonor>& honors,
                  int numPlayers) override {
        auto votesNeeded = computeRequiredVotes(roster, state, honors);
        TrackResources available = state.remaining;

        std::vector<const CouncilHonor*> viable;
        for (const auto& honor : honors) {
            int maxBid = getMaxContribution(available, honor.type);
            if (maxBid >= votesNeeded[honor.type]) {
                viable.push_back(&honor);
            }
        }

        std::sort(viable.begin(), viable.end(), [&](const CouncilHonor* a, const CouncilHonor* b) {
            return a->getVictoryPoints(numPlayers) > b->getVictoryPoints(numPlayers);
        });

        if (!viable.empty()) {
            const CouncilHonor* primary = viable[0];
            int maxBid = getMaxContribution(available, primary->type);
            int spend = std::max(votesNeeded[primary->type], maxBid);
            spend = std::clamp(spend, votesNeeded[primary->type], maxBid);
            auto commitment = allocateHonor(primary->type, spend, available);
            if (commitment) {
                state.commitments[primary->type] = *commitment;
            }
        }

        if (viable.size() > 1) {
            const CouncilHonor* secondary = viable[1];
            int maxBid = getMaxContribution(available, secondary->type);
            int spend = std::max(votesNeeded[secondary->type], maxBid);
            spend = std::clamp(spend, votesNeeded[secondary->type], maxBid);
            auto commitment = allocateHonor(secondary->type, spend, available);
            if (commitment) {
                state.commitments[secondary->type] = *commitment;
            }
        }

        state.remaining = available;
    }
};

class SpreaderStrategy : public VotingStrategy {
public:
    void allocate(PlayerCouncilState& state,
                  const std::vector<PlayerCouncilState>& roster,
                  const std::vector<CouncilHonor>& honors,
                  int numPlayers) override {
        auto votesNeeded = computeRequiredVotes(roster, state, honors);
        TrackResources available = state.remaining;

        std::vector<const CouncilHonor*> order;
        for (const auto& honor : honors) {
            order.push_back(&honor);
        }
        std::sort(order.begin(), order.end(), [&](const CouncilHonor* a, const CouncilHonor* b) {
            return a->getVictoryPoints(numPlayers) > b->getVictoryPoints(numPlayers);
        });

        for (const auto* honor : order) {
            int required = votesNeeded[honor->type];
            if (required <= 0) {
                continue;
            }
            auto commitment = allocateHonor(honor->type, required, available);
            if (commitment) {
                state.commitments[honor->type] = *commitment;
            }
        }

        state.remaining = available;
    }
};

class ChaosStrategy : public VotingStrategy {
public:
    explicit ChaosStrategy(unsigned int seed) : rng_(seed) {}

    void allocate(PlayerCouncilState& state,
                  const std::vector<PlayerCouncilState>& roster,
                  const std::vector<CouncilHonor>& honors,
                  int numPlayers) override {
        (void)numPlayers;
        auto votesNeeded = computeRequiredVotes(roster, state, honors);
        TrackResources available = state.remaining;

        std::vector<const CouncilHonor*> shuffled;
        for (const auto& honor : honors) {
            shuffled.push_back(&honor);
        }
        std::shuffle(shuffled.begin(), shuffled.end(), rng_);

        for (const auto* honor : shuffled) {
            int maxBid = getMaxContribution(available, honor->type);
            if (maxBid <= 0) {
                continue;
            }
            int minBid = std::min(maxBid, std::max(votesNeeded[honor->type], 1));
            std::uniform_int_distribution<int> dist(minBid, maxBid);
            int spend = dist(rng_);
            auto commitment = allocateHonor(honor->type, spend, available);
            if (commitment) {
                state.commitments[honor->type] = *commitment;
            }
        }

        state.remaining = available;
    }

private:
    std::mt19937 rng_;
};

std::unique_ptr<VotingStrategy> makeStrategy(VotingProfile profile, int playerId) {
    switch (profile) {
        case VotingProfile::Maximizer:
            return std::make_unique<MaximizerStrategy>();
        case VotingProfile::Sniper:
            return std::make_unique<SniperStrategy>();
        case VotingProfile::Spreader:
            return std::make_unique<SpreaderStrategy>();
        case VotingProfile::Chaos:
            return std::make_unique<ChaosStrategy>(static_cast<unsigned int>(playerId * 7919));
        case VotingProfile::None:
        default:
            return nullptr;
    }
}

void assignWildVotes(PlayerCouncilState& state,
                     const std::vector<CouncilHonor>& honors,
                     int numPlayers) {
    if (state.silkRoadMarks <= 0) {
        return;
    }
    const CouncilHonor* bestHonor = nullptr;
    for (const auto& honor : honors) {
        auto it = state.commitments.find(honor.type);
        if (it == state.commitments.end()) {
            continue;
        }
        if (!it->second.legal || it->second.totalFromTracks() <= 0) {
            continue;
        }
        if (!bestHonor || honor.getVictoryPoints(numPlayers) > bestHonor->getVictoryPoints(numPlayers)) {
            bestHonor = &honor;
        }
    }
    if (bestHonor) {
        state.commitments[bestHonor->type].wildVotes += state.silkRoadMarks;
    }
}

void resolveHonors(CouncilResults& results, int numPlayers) {
    const auto& honors = getCouncilHonors();

    for (const auto& honor : honors) {
        HonorOutcome outcome;
        outcome.honor = honor;
        outcome.vpPerWinner = 0.0;

        double vpValue = static_cast<double>(honor.getVictoryPoints(numPlayers));
        double maxVotes = 0.0;
        std::vector<const PlayerCouncilState*> contenders;

        for (const auto& state : results.states) {
            HonorCommitment commitment;
            auto it = state.commitments.find(honor.type);
            if (it != state.commitments.end()) {
                commitment = it->second;
            }
            outcome.commitmentsByPlayer[state.player->getId()] = commitment;

            if (!commitment.legal || commitment.totalFromTracks() <= 0) {
                continue;
            }

            double totalVotes = static_cast<double>(commitment.totalWithWild());
            if (totalVotes > maxVotes + 1e-6) {
                maxVotes = totalVotes;
                contenders.clear();
                contenders.push_back(&state);
            } else if (std::fabs(totalVotes - maxVotes) < 1e-6) {
                contenders.push_back(&state);
            }
        }

        std::vector<const PlayerCouncilState*> winners = contenders;
        std::string resolution;

        if (winners.size() > 1) {
            auto applyTieBreaker = [&](auto metricFunc, const std::string& label) {
                if (winners.size() <= 1) return;
                double bestMetric = -1.0;
                std::vector<const PlayerCouncilState*> filtered;
                for (const auto* candidate : winners) {
                    const auto& commitment = outcome.commitmentsByPlayer.at(candidate->player->getId());
                    double metric = metricFunc(*candidate, commitment);
                    if (metric > bestMetric + 1e-6) {
                        bestMetric = metric;
                        filtered = {candidate};
                    } else if (std::fabs(metric - bestMetric) < 1e-6) {
                        filtered.push_back(candidate);
                    }
                }
                if (filtered.size() < winners.size() && !filtered.empty()) {
                    winners = filtered;
                    resolution = label;
                }
            };

            applyTieBreaker(
                [](const PlayerCouncilState& state, const HonorCommitment& commitment) {
                    return computeUtilization(state.original, commitment);
                },
                "Resolved by Utilisation (U)"
            );

            applyTieBreaker(
                [](const PlayerCouncilState& state, const HonorCommitment& commitment) {
                    return computeFocus(state.original, commitment);
                },
                "Resolved by Focus (F)"
            );

            if (winners.size() > 1 && isPolyHonor(honor.type)) {
                auto polyBreaker = [&](const PlayerCouncilState&, const HonorCommitment& commitment) {
                    return static_cast<double>(commitment.sourcesUsed());
                };
                applyTieBreaker(polyBreaker, "Resolved by Poly breadth bonus");
            }
        }

        if (winners.empty() || maxVotes <= 0.0) {
            outcome.resolutionNote = "No eligible contestants.";
            results.outcomes.push_back(outcome);
            continue;
        }

        if (winners.size() > 1) {
            if (resolution.empty()) {
                resolution = "VP shared after tie-breakers.";
            } else {
                resolution += "; VP shared.";
            }
        }

        outcome.resolutionNote = resolution;
        double share = vpValue / static_cast<double>(winners.size());
        outcome.vpPerWinner = share;

        for (const auto* winnerState : winners) {
            auto id = winnerState->player->getId();
            results.honorPoints[id] += share;
            outcome.winners.push_back(winnerState->player);
        }

        results.outcomes.push_back(outcome);
    }
}

CouncilResults buildInitialStates(const std::vector<std::shared_ptr<Player>>& players) {
    CouncilResults results;
    results.states.reserve(players.size());

    for (const auto& player : players) {
        PlayerCouncilState state;
        state.player = player;
        auto breakdown = player->calculateVoteBreakdown();

        state.original.suits[Suit::HEARTS] = breakdown.guildStanding[Suit::HEARTS];
        state.original.suits[Suit::DIAMONDS] = breakdown.guildStanding[Suit::DIAMONDS];
        state.original.suits[Suit::CLUBS] = breakdown.guildStanding[Suit::CLUBS];
        state.original.suits[Suit::SPADES] = breakdown.guildStanding[Suit::SPADES];
        state.original.route = breakdown.caravanCapacity;
        state.original.market = breakdown.marketShare;
        state.remaining = state.original;
        state.silkRoadMarks = breakdown.silkRoadMarks;

        results.honorPoints[player->getId()] = 0.0;
        results.states.push_back(state);
    }

    return results;
}

CouncilResults calculateBasic(const std::vector<std::shared_ptr<Player>>& players, int numPlayers) {
    auto results = buildInitialStates(players);
    const auto& honors = getCouncilHonors();

    for (auto& state : results.states) {
        for (const auto& honor : honors) {
            int maxContribution = getMaxContribution(state.original, honor.type);
            if (maxContribution <= 0) {
                continue;
            }
            TrackResources scratch = state.original;
            auto commitment = allocateHonor(honor.type, maxContribution, scratch);
            if (commitment) {
                state.commitments[honor.type] = *commitment;
            }
        }
    }

    for (auto& state : results.states) {
        assignWildVotes(state, honors, numPlayers);
    }

    resolveHonors(results, numPlayers);
    return results;
}

CouncilResults calculateWithStrategies(const std::vector<std::shared_ptr<Player>>& players,
                                       int numPlayers,
                                       const StrategyAssignments& assignments) {
    auto results = buildInitialStates(players);
    const auto& honors = getCouncilHonors();

    for (auto& state : results.states) {
        VotingProfile profile = VotingProfile::Maximizer;
        auto it = assignments.find(state.player->getId());
        if (it != assignments.end() && it->second.profile != VotingProfile::None) {
            profile = it->second.profile;
        }

        auto strategy = makeStrategy(profile, state.player->getId());
        if (!strategy) {
            strategy = makeStrategy(VotingProfile::Maximizer, state.player->getId());
        }

        strategy->allocate(state, results.states, honors, numPlayers);
    }

    for (auto& state : results.states) {
        assignWildVotes(state, honors, numPlayers);
    }

    resolveHonors(results, numPlayers);
    return results;
}

} // namespace

int CouncilHonor::getVictoryPoints(int numPlayers) const {
    auto it = victoryPointsByPlayerCount.find(numPlayers);
    if (it != victoryPointsByPlayerCount.end()) {
        return it->second;
    }
    int bestPlayers = 0;
    int vp = 0;
    for (const auto& entry : victoryPointsByPlayerCount) {
        if (entry.first <= numPlayers && entry.first > bestPlayers) {
            bestPlayers = entry.first;
            vp = entry.second;
        }
    }
    return vp;
}

int TrackResources::totalVotes() const {
    int total = route + market;
    for (const auto& [suit, value] : suits) {
        total += value;
    }
    return total;
}

int HonorCommitment::totalFromTracks() const {
    int total = routeVotes + marketVotes;
    for (const auto& [suit, value] : suitVotes) {
        total += value;
    }
    return total;
}

int HonorCommitment::totalWithWild() const {
    return totalFromTracks() + wildVotes;
}

int HonorCommitment::sourcesUsed() const {
    int sources = 0;
    for (const auto& [suit, value] : suitVotes) {
        if (value > 0) {
            sources++;
        }
    }
    if (routeVotes > 0) sources++;
    if (marketVotes > 0) sources++;
    return sources;
}

const std::vector<CouncilHonor>& getCouncilHonors() {
    static const std::vector<CouncilHonor> HONORS = {
        {HonorType::MONO_SUIT, "Single-Guild Honour (Mono-Suit)", {{2, 5}, {3, 7}, {4, 8}}},
        {HonorType::POLY_SUIT, "Cross-Guild Honour (Poly-Suit)", {{2, 4}, {3, 5}, {4, 6}}},
        {HonorType::ROUTE_ONLY, "Route Honour (Route-only)", {{2, 6}, {3, 7}, {4, 8}}},
        {HonorType::MARKET_ONLY, "Market Honour (Market-only)", {{2, 4}, {3, 5}, {4, 6}}},
        {HonorType::MONO_TRACK, "Single-Track Honour (Mono-Ledger)", {{2, 5}, {3, 6}, {4, 7}}},
        {HonorType::POLY_TRACK, "Combined-Track Honour (Poly-Ledger)", {{2, 3}, {3, 4}, {4, 5}}}
    };
    return HONORS;
}

CouncilResults calculateBasicCouncilResults(const std::vector<std::shared_ptr<Player>>& players,
                                            int numPlayers) {
    return calculateBasic(players, numPlayers);
}

CouncilResults resolveCouncil(const std::vector<std::shared_ptr<Player>>& players,
                              int numPlayers,
                              const StrategyAssignments& assignments) {
    bool useStrategies = false;
    for (const auto& [_, config] : assignments) {
        if (config.profile != VotingProfile::None) {
            useStrategies = true;
            break;
        }
    }
    if (!useStrategies) {
        return calculateBasic(players, numPlayers);
    }
    return calculateWithStrategies(players, numPlayers, assignments);
}

double computeUtilization(const TrackResources& original, const HonorCommitment& commitment) {
    double used = static_cast<double>(commitment.totalFromTracks());
    if (used <= 0.0) {
        return 0.0;
    }
    double capacity = 0.0;
    for (const auto& [suit, votes] : commitment.suitVotes) {
        auto it = original.suits.find(suit);
        if (it != original.suits.end()) {
            capacity += it->second;
        }
    }
    if (commitment.routeVotes > 0) {
        capacity += original.route;
    }
    if (commitment.marketVotes > 0) {
        capacity += original.market;
    }
    if (capacity <= 0.0) {
        return 0.0;
    }
    return used / capacity;
}

double computeFocus(const TrackResources& original, const HonorCommitment& commitment) {
    double used = static_cast<double>(commitment.totalFromTracks());
    if (used <= 0.0) {
        return 0.0;
    }
    double totalCapacity = static_cast<double>(original.totalVotes());
    if (totalCapacity <= 0.0) {
        return 0.0;
    }
    return used / totalCapacity;
}

std::string formatScore(double value) {
    if (std::fabs(value - std::round(value)) < 1e-6) {
        return std::to_string(static_cast<int>(std::round(value)));
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value;
    return oss.str();
}

std::string formatDecimal(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

std::string commitmentSummary(const HonorCommitment& commitment) {
    std::vector<std::string> parts;
    for (const auto& [suit, votes] : commitment.suitVotes) {
        if (votes > 0) {
            parts.push_back(suitToString(suit) + " " + std::to_string(votes));
        }
    }
    if (commitment.routeVotes > 0) {
        parts.push_back("Routes " + std::to_string(commitment.routeVotes));
    }
    if (commitment.marketVotes > 0) {
        parts.push_back("Market " + std::to_string(commitment.marketVotes));
    }
    if (parts.empty()) {
        return "No votes";
    }
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << parts[i];
    }
    return oss.str();
}

void printCouncilHonorResults(const CouncilResults& results, int numPlayers) {
    std::cout << "\n=== AUDIENCE WITH THE HIGH COUNCIL ===" << std::endl;

    for (const auto& outcome : results.outcomes) {
        int vpValue = outcome.honor.getVictoryPoints(numPlayers);
        std::cout << "\n" << outcome.honor.name << " (" << vpValue << " VP):" << std::endl;

        if (outcome.winners.empty()) {
            std::cout << "  No honour awarded." << std::endl;
            continue;
        }

        if (outcome.winners.size() == 1) {
            std::cout << "  Winner: " << outcome.winners.front()->getName()
                      << " (" << formatScore(outcome.vpPerWinner) << " VP)" << std::endl;
        } else {
            std::cout << "  Winners: ";
            for (size_t i = 0; i < outcome.winners.size(); ++i) {
                if (i > 0) {
                    std::cout << ", ";
                }
                std::cout << outcome.winners[i]->getName();
            }
            std::cout << " (each receives " << formatScore(outcome.vpPerWinner) << " VP)" << std::endl;
        }

        if (!outcome.resolutionNote.empty()) {
            std::cout << "  " << outcome.resolutionNote << std::endl;
        }

        std::vector<std::tuple<double, std::shared_ptr<Player>, HonorCommitment, double, double>> displayEntries;
        for (const auto& state : results.states) {
            auto it = outcome.commitmentsByPlayer.find(state.player->getId());
            if (it == outcome.commitmentsByPlayer.end()) {
                continue;
            }
            const auto& commitment = it->second;
            if (!commitment.legal || commitment.totalFromTracks() <= 0) {
                continue;
            }
            double totalVotes = static_cast<double>(commitment.totalWithWild());
            double utilization = computeUtilization(state.original, commitment);
            double focus = computeFocus(state.original, commitment);
            displayEntries.emplace_back(totalVotes, state.player, commitment, utilization, focus);
        }

        std::sort(displayEntries.begin(), displayEntries.end(), [](const auto& a, const auto& b) {
            return std::get<0>(a) > std::get<0>(b);
        });

        for (const auto& entry : displayEntries) {
            double totalVotes = std::get<0>(entry);
            auto player = std::get<1>(entry);
            const auto& commitment = std::get<2>(entry);
            double utilization = std::get<3>(entry);
            double focus = std::get<4>(entry);

            std::cout << "  - " << player->getName() << ": "
                      << formatScore(totalVotes) << " votes";
            std::cout << " (" << commitmentSummary(commitment);
            if (commitment.wildVotes > 0) {
                std::cout << ", Wild " << commitment.wildVotes;
            }
            std::cout << "; U=" << formatDecimal(utilization)
                      << ", F=" << formatDecimal(focus) << ")" << std::endl;
        }
    }
}

} // namespace council

