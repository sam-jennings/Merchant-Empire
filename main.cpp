#include "Game.h"
#include <iostream>
#include <ctime>

int main() {
    // Seed with current time for randomness, or use a fixed seed for reproducibility
    unsigned int seed = static_cast<unsigned int>(time(nullptr));
    
    std::cout << "Merchant Empire - 4 Player Simulation" << std::endl;
    std::cout << "Random seed: " << seed << std::endl;
    std::cout << std::endl;
    
    Game game(4, seed);
    game.play();
    
    return 0;
}