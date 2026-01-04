#include "tank.hpp"
#include <cmath>

static constexpr float PI = 3.14159265f;

Tank::Tank(sf::Vector2f position, sf::Keyboard::Key controlKey)
: controlKey_(controlKey)
{
    body_.setSize({60.f, 30.f});
    body_.setOrigin(body_.getSize() * 0.5f);
    body_.setPosition(position);
}

void Tank::setMovingForward(bool enabled) {
    movingForward_ = enabled;
}

void Tank::setTurnSpeed(float degPerSec) {
    turnSpeed_ = degPerSec;
}

void Tank::setMoveSpeed(float pxPerSec) {
    moveSpeed_ = pxPerSec;
}

sf::Vector2f Tank::getPosition() const {
    return body_.getPosition();
}

float Tank::getAngleDeg() const {
    return angleDeg_;
}

void Tank::setWorldBounds(const sf::FloatRect& bounds) {
    worldBounds_ = bounds;
}

sf::FloatRect Tank::getAABB() const {
    return body_.getGlobalBounds();
}

void Tank::update(float dt) {
    if (!movingForward_) {
        angleDeg_ += turnDirection_ * turnSpeed_ * dt;
    }

    float rad = angleDeg_ * PI / 180.f;
    sf::Vector2f forward(std::cos(rad), std::sin(rad));

    if (movingForward_) {
        body_.move(forward * moveSpeed_ * dt);
    }

    body_.setRotation(angleDeg_);
    clampToWorld();
}

void Tank::clampToWorld() {
    if (worldBounds_.width < 0.f || worldBounds_.height < 0.f)
        return;

    sf::Vector2f pos = body_.getPosition();
    sf::Vector2f half = body_.getSize() * 0.5f;

    float left   = worldBounds_.left + half.x;
    float right  = worldBounds_.left + worldBounds_.width  - half.x;
    float top    = worldBounds_.top  + half.y;
    float bottom = worldBounds_.top  + worldBounds_.height - half.y;

    if (pos.x < left)   pos.x = left;
    if (pos.x > right)  pos.x = right;
    if (pos.y < top)    pos.y = top;
    if (pos.y > bottom) pos.y = bottom;

    body_.setPosition(pos);
}

void Tank::draw(sf::RenderTarget& target) const {
    target.draw(body_);
}

void Tank::flipRotationDirection() {
    turnDirection_ *= -1.f;
}

void Tank::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == controlKey_) {
        movingForward_ = true;
    }

    if (e.type == sf::Event::KeyReleased && e.key.code == controlKey_) {
        movingForward_ = false;
        flipRotationDirection();
    }
}
