#include "game.hpp"
#include <iostream>

int main() {

    std::size_t tankCount = 2;
    unsigned maxBlocks = 8;

    std::cout << "Witaj w SpinTanks!" << std::endl;

    std::cout << "- Edytor (PRZED gra): LPM stawia, PPM usuwa, Enter start\n";

    std::cout << "Sterowanie czołgami:" << std::endl;
    for (std::size_t i = 0; i < tankCount; ++i) {
        std::cout << "Czołg " << (i + 1) << ": ";
        switch (i) {
            case 0: std::cout << "Spacja"; break;
            case 1: std::cout << "Enter"; break;
            case 2: std::cout << "W"; break;
            case 3: std::cout << "Strzałka w górę"; break;
        }
        std::cout << std::endl;
    }

    Game game;
    game.run();
}