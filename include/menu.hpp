#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class Menu {
public:
    enum class Screen {
        MAIN,
        PLAYER_SELECT,
        GAME_OVER
    };

    Menu(float width, float height);

    void handleEvent(const sf::Event&);

    void render(sf::RenderWindow&);

    bool startRequested() const;

    bool quitRequested() const;

    std::size_t getPlayerCount() const;

    void setScreen(Screen);

    void setTankWinner(int id) { tankWinnerID_ = id; }

    std::string getTankWinnerKey() const;

private:
    sf::Font font_;
    sf::Text text_;
    bool startRequested_ = false;
    bool quitRequested_ = false;

    Screen screen_ = Screen::MAIN;

    void handleMainMenuEvent(const sf::Event&);
    void handlePlayerSelectEvent(const sf::Event&);

    void renderMainMenu(sf::RenderWindow&);
    void renderPlayerSelect(sf::RenderWindow&);

    void handleGameOverEvent(const sf::Event&);
    void renderGameOver(sf::RenderWindow&);

    std::size_t playerCount_ = 2;
    bool startGame_ = false;

    sf::Texture backgroundTex_;
    sf::Sprite  backgroundSprite_;

    void centerText(sf::Text& text, float x, float y);

    float width_;
    float height_;

    int tankWinnerID_ = -1;
};