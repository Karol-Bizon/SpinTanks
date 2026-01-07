#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "tank.hpp"

class Game {
public:
    explicit Game(std::size_t tankCount);
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

private:
    sf::RenderWindow window_;
    sf::Clock clock_;
    std::vector<Tank> tanks_;
};
