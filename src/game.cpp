#include "game.hpp"

static constexpr float WORLD_WIDTH  = 1000.f;
static constexpr float WORLD_HEIGHT = 1000.f;

static const sf::Keyboard::Key TANK_KEYS[4] = {
    sf::Keyboard::Space,
    sf::Keyboard::Enter,
    sf::Keyboard::W,
    sf::Keyboard::Up
};

static const sf::Vector2f TANK_POSITIONS[4] = {
    {0.3f*WORLD_WIDTH, 0.3f*WORLD_HEIGHT},
    {0.7f*WORLD_WIDTH, 0.7f*WORLD_HEIGHT},
    {0.7f*WORLD_WIDTH, 0.3f*WORLD_HEIGHT},
    {0.3f*WORLD_WIDTH, 0.7f*WORLD_HEIGHT}
};


Game::Game(std::size_t tankCount)
: window_(sf::VideoMode(
      static_cast<unsigned>(WORLD_WIDTH),
      static_cast<unsigned>(WORLD_HEIGHT)),
      "SpinTanks")
{
    window_.setFramerateLimit(60);

    tankCount = std::clamp(tankCount, std::size_t{1}, std::size_t{4});
    tanks_.reserve(tankCount);

    const sf::FloatRect world{0.f, 0.f, WORLD_WIDTH, WORLD_HEIGHT};

    for (std::size_t i = 0; i < tankCount; ++i) {
        tanks_.emplace_back(TANK_POSITIONS[i], TANK_KEYS[i]);
        tanks_.back().setWorldBounds(world);
    }
}


void Game::run() {
    while (window_.isOpen()) {
        processEvents();
        float dt = clock_.restart().asSeconds();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event e;
    while (window_.pollEvent(e)) {
        if (e.type == sf::Event::Closed)
            window_.close();

        for (auto& tank : tanks_)
            tank.handleEvent(e);
    }
}

void Game::update(float dt) {
    for (auto& tank : tanks_)
        tank.update(dt);
}

void Game::render() {
    window_.clear();
    for (auto& tank : tanks_)
        tank.draw(window_);
    window_.display();
}
