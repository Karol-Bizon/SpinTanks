#pragma once
#include <SFML/Graphics.hpp>

class Projectile {
public:
    Projectile(const sf::Texture& tex, sf::Vector2f pos, sf::Vector2f vel);

    void update(float dt);
    void draw(sf::RenderTarget& target) const;

    bool isAlive() const { return alive_; }
    void kill() { alive_ = false; }
    void setOwner(int tankn);
    int getOwner() { return owner_; }
    float getDMG() { return damage_; }
    void setDMG(float ile);

    sf::FloatRect getAABB() const { return sprite_.getGlobalBounds(); }
    sf::Vector2f getPosition() const { return sprite_.getPosition(); }
    sf::Vector2f getCenter() const { return sprite_.getPosition(); }

    sf::Vector2f getHitboxCenter() const;
    float getHitboxRadius() const;

private:
    sf::Sprite sprite_;
    sf::Vector2f vel_;
    bool alive_ = true;
    int owner_ = 0;
    float damage_ = 7.7f;
};
