#include "menu.hpp"
#include <iostream>

Menu::Menu(float width, float height) {
    if (!font_.loadFromFile("graphics/main_font.ttf")) {
        std::cout << "Failed to load font for menu!" << std::endl;
    }

    text_.setFont(font_);
    text_.setString("PRESS ENTER TO START");
    text_.setCharacterSize(40);
    text_.setFillColor(sf::Color::White);

    // Center the text
    sf::FloatRect bounds = text_.getLocalBounds();
    text_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    text_.setPosition(width / 2.f, height / 2.f);

    if (!backgroundTex_.loadFromFile("graphics/bg_wallpaper.png")) {
        std::cout << "Failed to load menu background!\n";
    } else {
        backgroundSprite_.setTexture(backgroundTex_);

        sf::Vector2u s = backgroundTex_.getSize();
        backgroundSprite_.setScale(
            width  / static_cast<float>(s.x),
            height / static_cast<float>(s.y)
        );
    }

}

void Menu::handleEvent(const sf::Event& event) {
    switch (screen_) {
        case Screen::MAIN:
            handleMainMenuEvent(event);
            break;
        case Screen::PLAYER_SELECT:
            handlePlayerSelectEvent(event);
            break;
        case Screen::GAME_OVER:
            handleGameOverEvent(event);
            break;
    }
}

void Menu::handleMainMenuEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Enter) {
        screen_ = Screen::PLAYER_SELECT;
    }
}

void Menu::handlePlayerSelectEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left) {
            if (playerCount_ > 1) playerCount_--;
        } else if (event.key.code == sf::Keyboard::Right) {
            if (playerCount_ < 4) playerCount_++;
        } else if (event.key.code == sf::Keyboard::Enter) {
            startRequested_ = true;
        }
    }
}

bool Menu::startRequested() const {
    return startRequested_;
}

std::size_t Menu::getPlayerCount() const {
    return playerCount_;
}

void Menu::render(sf::RenderWindow& window) {

    switch (screen_) {
        case Screen::MAIN:
            window.draw(backgroundSprite_);
            renderMainMenu(window);
            break;
        case Screen::PLAYER_SELECT:
            window.draw(backgroundSprite_);
            renderPlayerSelect(window);
            break;
        case Screen::GAME_OVER:
            renderGameOver(window);
            break;
    }
}


void Menu::renderMainMenu(sf::RenderWindow& window) {
    text_.setString("PRESS ENTER TO START");
    window.draw(text_);
}

void Menu::renderPlayerSelect(sf::RenderWindow& window) {
    text_.setString(
        "PLAYERS: " + std::to_string(playerCount_) +
        "\nLEFT / RIGHT\nENTER TO START"
    );
    window.draw(text_);
}

void Menu::renderGameOver(sf::RenderWindow& window) {
    text_.setString("GAME OVER\nENTER");
    text_.setCharacterSize(80);
    text_.setFillColor(sf::Color::Red);

    auto b = text_.getLocalBounds();
    text_.setOrigin(b.width / 2.f, b.height / 2.f);
    text_.setPosition(
        window.getSize().x / 2.f,
        window.getSize().y / 2.f
    );

    window.draw(text_);
}

void Menu::setScreen(Screen s) {
    screen_ = s;
    startRequested_ = false;
}

void Menu::handleGameOverEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Enter) {
        startRequested_ = true; // wracamy do menu / restart
    }
}