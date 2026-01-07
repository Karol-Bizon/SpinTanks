#include "game.hpp"
#include <iostream>

int main() {

    std::size_t tankCount = 2;

    std::cout << "Witaj w SpinTanks!" << std::endl;
    std::cout << "Podaj liczbę czołgów (1-4): ";
    std::cin >> tankCount;

    if (tankCount < 1 || tankCount > 4) {
        std::cout << "Nieprawidłowa liczba czołgów. Ustawiono domyślnie na 2." << std::endl;
        tankCount = 2;
    }

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

    Game game(tankCount);
    game.run();
}