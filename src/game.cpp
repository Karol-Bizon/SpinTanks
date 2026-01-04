#include "game.hpp"

static constexpr float WORLD_WIDTH  = 800.f;
static constexpr float WORLD_HEIGHT = 600.f;

Game::Game()
: window_(sf::VideoMode(800, 600), "SpinTanks"),
  tank1_({250.f, 300.f}, sf::Keyboard::Space),
  tank2_({550.f, 300.f}, sf::Keyboard::Enter)
{
    window_.setFramerateLimit(60);

    const sf::FloatRect world{0.f, 0.f, 800.f, 600.f};
    tank1_.setWorldBounds(world);
    tank2_.setWorldBounds(world);
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

        tank1_.handleEvent(e);
        tank2_.handleEvent(e);
    }
}


void Game::update(float dt) {
    tank1_.update(dt);
    tank2_.update(dt);
}

void Game::render() {
    window_.clear();
    tank1_.draw(window_);
    tank2_.draw(window_);
    window_.display();
}
