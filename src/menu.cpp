#include "menu.hpp"
#include <iostream>




Menu::Menu(float width, float height)
{
    if (!font_.loadFromFile("graphics/main_font.ttf")) {
        std::cout << "Failed to load font for menu!" << std::endl;
    }

    text_.setFont(font_);
    // text_.setString("PRESS ENTER TO START");
    // text_.setCharacterSize(40);
    // text_.setFillColor(sf::Color::White);

    // // Center the text
    // sf::FloatRect bounds = text_.getLocalBounds();
    // text_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    // text_.setPosition(width / 2.f, height / 2.f);

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

    width_ = width;
    height_ = height;

}

static sf::RectangleShape makeTextBackdrop(const sf::Text& txt, float padding = 6.f, sf::Color color = sf::Color(0,0,0,110))
{
    sf::FloatRect b = txt.getGlobalBounds();
    sf::RectangleShape r;
    r.setPosition(b.left - padding, b.top - padding);
    r.setSize({ b.width + 2.f * padding, b.height + 2.f * padding });
    r.setFillColor(color);
    r.setOutlineThickness(1.f);
    r.setOutlineColor(sf::Color(255, 255, 255, 18));
    return r;
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

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape) {
        quitRequested_ = true;
    }
}

void Menu::handlePlayerSelectEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left) {
            if (playerCount_ > 2) playerCount_--;
        } else if (event.key.code == sf::Keyboard::Right) {
            if (playerCount_ < 4) playerCount_++;
        } else if (event.key.code == sf::Keyboard::Enter) {
            startRequested_ = true;
        }
    }

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape) {
        quitRequested_ = true;
    }
}

bool Menu::startRequested() const {
    return startRequested_;
}

bool Menu::quitRequested() const {
    return quitRequested_;
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
    sf::Text title("SPIN TANKS", font_, 72);
    centerText(title, width_ / 2.f, height_ * 0.3f);
    sf::Text prompt("--- PRESS ENTER TO START ---", font_, 32);
    centerText(prompt, width_ / 2.f, height_ * 0.5f);
    window.draw(title);
    window.draw(prompt);
}

void Menu::renderPlayerSelect(sf::RenderWindow& window) {
    sf::Text title("CHOOSE NUMBER OF PLAYERS", font_, 64);
    centerText(title, width_ / 2.f, height_ * 0.3f);

    sf::Text players(
        std::to_string(playerCount_) + " Players",
        font_,
        48
    );
    centerText(players, width_ / 2.f, height_ * 0.5f);

    sf::Text info("USE ARROWS TO CHANGE NUMBER OF PLAYERS", font_, 32);
    centerText(info, width_ / 2.f, height_ * 0.7f);

    sf::Text info2("PRESS ENTER TO PROCEED", font_, 32);
    centerText(info2, width_ / 2.f, height_ * 0.8f);

    window.draw(title);
    window.draw(players);
    window.draw(makeTextBackdrop(info, 7.f, sf::Color(0,0,0,150)));
    window.draw(info);
    window.draw(makeTextBackdrop(info2, 7.f, sf::Color(0,0,0,150)));
    window.draw(info2);
}

void Menu::renderGameOver(sf::RenderWindow& window) {

    sf::Text title("GAME OVER", font_, 80);
    centerText(title, width_ / 2.f, height_ * 0.3f);

    sf::Text message(
        "PLAYER " + getTankWinnerKey() + " WINS!",
        font_,
        64
    );
    centerText(message, width_ / 2.f, height_ * 0.5f);

    sf::Text info("RETURN TO MAIN MENU - BACKSPACE", font_, 32);
    centerText(info, width_ / 2.f, height_ * 0.85f);

    sf::Text info2("QUIT GAME - ESCAPE", font_, 32);
    centerText(info2, width_ / 2.f, height_ * 0.9f);

    window.draw(title);
    window.draw(info);
    window.draw(info2);
    window.draw(message);
}

void Menu::setScreen(Screen s) {
    screen_ = s;
    startRequested_ = false;
}

void Menu::handleGameOverEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Backspace) {
        startRequested_ = true;
    }

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape) {
        quitRequested_ = true;
    }
}

void Menu::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(
        bounds.left + bounds.width  / 2.f,
        bounds.top  + bounds.height / 2.f
    );
    text.setPosition(x, y);
}

std::string Menu::getTankWinnerKey() const {
    if (tankWinnerID_ < 0) return "NONE";
    
    switch (tankWinnerID_) {
        case 1: return "SPACE";
        case 2: return "ENTER";
        case 3: return "W";
        case 4: return "UPARROW";
    }

    return "UNKNOWN";
}

