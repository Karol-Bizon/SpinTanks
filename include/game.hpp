#pragma once

#include <SFML/Graphics.hpp>
#include "tank.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

private:
    sf::RenderWindow window_;
    sf::Clock clock_;

    Tank tank1_;
    Tank tank2_;
};

