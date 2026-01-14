#pragma once
#include <SFML/Graphics.hpp>

struct Bonuses {
    //co moga robic power upp?: przeladowanie, leczenie, max_hp + leczenie, szybkosc ruchu/obrotu
    float dmgBONUS = 27.f; ///dziala jako mnoznik!
    float reloadBONUS_ = 0.1f;
    float healBONUS_ = 10.f;
    float maxhpBONUS_ = 240.f;
    float rotspedBONUS_ = 10.f;
    float spedBONUS_ = 15.f;
};

class PowerUP {
public:
    PowerUP(const sf::Texture& tex, sf::Vector2f pos);

    void update(float dt);
    void draw(sf::RenderTarget& target) const;

    bool isAlive() const { return alive_; }
    void kill() { alive_ = false; }

    void placeRandom(const sf::FloatRect& bounds);
    void setBonuses(float DMG, float rel, float heal, float max, float rot, float sped);
    Bonuses getBonuses() { return bonuses_; }

    void setTexture(const sf::Texture& tex);

    sf::FloatRect getAABB() const { return sprite_.getGlobalBounds(); }
    sf::Vector2f getPosition() const { return sprite_.getPosition(); }
    sf::Vector2f getCenter() const { return sprite_.getPosition(); }

private:
    sf::Sprite sprite_;
    bool alive_ = true;
    Bonuses bonuses_;
    sf::Clock AliveTimer_;
};