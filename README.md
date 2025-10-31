# Merchant Empire - C++ Simulation

A C++ implementation that simulates the Merchant Empire card game with 4 AI players.

## Overview

This simulation implements the complete Merchant Empire game rules including:
- **Contract Types**: Partnership, Trade Route, Monopoly, and Silk Road
- **Turn Phases**: Supply Phase, Barter Phase, and Deal Phase
- **AI Strategy**: Players maximize points by identifying and creating the most valuable contracts
- **Game Flow**: Full game simulation from setup to final scoring

## Files

- `Card.h/cpp` - Card representation with suits and ranks
- `Contract.h/cpp` - Contract types, validation, and scoring logic
- `Player.h/cpp` - Player state management and AI strategy
- `Game.h/cpp` - Game state management and turn simulation
- `main.cpp` - Entry point for running the simulation
- `Makefile` - Build configuration

## Building

To compile the project:

```bash
make
```

To clean and rebuild:

```bash
make clean
make
```

## Running

To run the simulation:

```bash
./merchant_empire
```

Or use the Makefile shortcut:

```bash
make run
```

## Output

The simulation outputs:
1. **Turn-by-turn contract creation** - Shows which contracts were signed or extended each round
2. **Final standings** - Displays all players ranked by points
3. **Contract details** - Lists all contracts for each player with:
   - Contract type
   - Number of cards
   - Points scored
   - Round created
   - Specific cards in the contract

## Example Output

```
=== MERCHANT EMPIRE SIMULATION ===
Starting game with 4 players
Supply: 23 cards remaining

  Round 1: Player 1 signed Trade Route (4 cards, 6 pts)
  Round 1: Player 2 signed Trade Route (3 cards, 4 pts)
  ...

=== FINAL STANDINGS ===

1. Player 1 - 10 points (1 contracts)
   Contracts:
   - Trade Route (5 cards, 10 pts, Round 1): 6 of Clubs, 7 of Spades, 8 of Spades, 9 of Clubs, 10 of Hearts
...
```

## AI Strategy

The AI players use a point-maximizing strategy:

1. **Contract Selection**: Evaluates all possible contracts and selects based on efficiency (points per card)
2. **Contract Priority**: Prefers Silk Roads > Monopolies > Trade Routes > Partnerships
3. **Extension Logic**: Extends existing contracts when it adds more points
4. **Barter Strategy**: Uses Trade Routes to acquire high-value cards from the Bazaar
5. **Card Trading**: Trades away low-value cards when using Trade Routes

## Contract Scoring

The scoring follows the official game rules:

| Contract Type | 3 Cards | 4 Cards | 5 Cards | 6 Cards | 7 Cards |
|--------------|---------|---------|---------|---------|---------|
| Partnership  | 3       | 5       | 8       | 12      | 18      |
| Trade Route  | 4       | 6       | 10      | 15      | 22      |
| Monopoly     | 5       | 12      | -       | -       | -       |
| Silk Road    | 7       | 11      | 18      | 27      | 40      |

## Game Configuration

- **Players**: 4 (configurable in `main.cpp`)
- **Cards per player**: 6 (for 4 players)
- **Bazaar size**: 5 cards
- **Random seed**: Uses current time (can be fixed for reproducibility)

## Customization

To run multiple simulations or change the number of players, modify `main.cpp`:

```cpp
Game game(4, seed);  // Change 4 to desired number of players (3-4 supported)
```

To use a fixed random seed for reproducible games:

```cpp
unsigned int seed = 12345;  // Fixed seed
Game game(4, seed);
```

## Requirements

- C++17 compatible compiler (g++ recommended)
- Make utility

## License

This is a simulation implementation based on the Merchant Empire game rules.