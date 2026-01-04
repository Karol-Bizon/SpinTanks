#pragma once
#include <SFML/Graphics.hpp>

class Tank {
public:
    Tank(sf::Vector2f position, sf::Keyboard::Key controlKey);

    void handleEvent(const sf::Event& e);
    void update(float dt);
    void draw(sf::RenderTarget& target) const;

    void setWorldBounds(const sf::FloatRect& bounds);

    void setMovingForward(bool enabled);
    void setTurnSpeed(float degPerSec);
    void setMoveSpeed(float pxPerSec);

    sf::Vector2f getPosition() const;
    float getAngleDeg() const;
    sf::FloatRect getAABB() const;

private:
    void flipRotationDirection();
    void clampToWorld();

private:
    sf::RectangleShape body_;
    sf::Keyboard::Key controlKey_;

    float angleDeg_ = 0.f;
    float turnSpeed_ = 120.f;
    float moveSpeed_ = 180.f;
    float turnDirection_ = 1.f;

    bool movingForward_ = false;

    sf::FloatRect worldBounds_{0.f, 0.f, -1.f, -1.f};
};
