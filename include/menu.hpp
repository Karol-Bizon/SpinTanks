#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class Menu {
public:
    enum class Screen {
        MAIN,
        PLAYER_SELECT
    };

    Menu(float width, float height);

    void handleEvent(const sf::Event&);

    void render(sf::RenderWindow&);

    bool startRequested() const;

    std::size_t getPlayerCount() const;

    
private:
    sf::Font font_;
    sf::Text text_;
    bool startRequested_ = false;

    Screen screen_ = Screen::MAIN;

    void handleMainMenuEvent(const sf::Event&);
    void handlePlayerSelectEvent(const sf::Event&);

    void renderMainMenu(sf::RenderWindow&);
    void renderPlayerSelect(sf::RenderWindow&);

    std::size_t playerCount_ = 2;
    bool startGame_ = false;
};