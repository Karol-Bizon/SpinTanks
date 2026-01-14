#include "powerUPPS.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <random>

PowerUP::PowerUP(const sf::Texture& tex, sf::Vector2f pos)
{
    sprite_.setTexture(tex);
    auto b = sprite_.getLocalBounds();
    sprite_.setOrigin(b.width * 0.5f, b.height * 0.5f);
    sprite_.setPosition(pos);
}

void PowerUP::update(float dt) {
    //sprite_.move(vel_ * dt); wiemy juz ze ta funkcja dziala
    //std::cout << "i am a powerup!" << std::endl;
    //std::cout << AliveTimer_.getElapsedTime().asSeconds() << std::endl;
 

    // tutaj mozemy sprawdzic timer po prostu - kg
    if (AliveTimer_.getElapsedTime().asSeconds() > 17.f && isAlive()) { kill(); }
}

void PowerUP::draw(sf::RenderTarget& target) const {
    target.draw(sprite_);

    //DEBUG - pokazuje collider powerupp (ogromnie wielki byl oryginalnie XD)
    sf::FloatRect box = sprite_.getGlobalBounds();
    sf::RectangleShape debug;
    debug.setPosition({ box.left, box.top });
    debug.setSize({ box.width, box.height });
    debug.setFillColor(sf::Color::Transparent);
    debug.setOutlineThickness(1.f);
    debug.setOutlineColor(sf::Color::Yellow);
    target.draw(debug);
}



void PowerUP::placeRandom(const sf::FloatRect& bounds) {
    //jeœli bounds ma zerowy rozmiar, nic nie rób
    if (bounds.width <= 0.f || bounds.height <= 0.f) return;

    //statyczny RNG
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distX(bounds.left, bounds.left + bounds.width);
    std::uniform_real_distribution<float> distY(bounds.top, bounds.top + bounds.height);

    float x = distX(gen);
    float y = distY(gen);

    sprite_.setPosition(x, y);
}

void PowerUP::setTexture(const sf::Texture& tex) {
    sprite_.setTexture(tex);
}


void PowerUP::setBonuses(float DMG, float rel, float heal, float max, float rot, float sped) {
    bonuses_.dmgBONUS = DMG;
    bonuses_.reloadBONUS_ = rel;
    bonuses_.healBONUS_ = heal;
    bonuses_.maxhpBONUS_ = max;
    bonuses_.rotspedBONUS_ = rot;
    bonuses_.spedBONUS_ = sped;
}


