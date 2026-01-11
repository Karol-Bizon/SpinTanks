#include "tank.hpp"
#include <cmath>

static constexpr float PI = 3.14159265f;

//definicja czolgu, jego rozmiar chyba nie rowna sie rozmiarowi tekstury --- obserwowalne podczas gry -krzys
Tank::Tank(sf::Vector2f position, sf::Keyboard::Key controlKey, const sf::Texture& texture)
: controlKey_(controlKey)
{
    body_.setSize({60.f * 1.5f, 60.f * 1.5f});
    body_.setTexture(&texture, true);
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
//granice swiata wiadome dla czolgow
void Tank::setWorldBounds(const sf::FloatRect& bounds) {
    worldBounds_ = bounds;
}
//collision box czolgu??
sf::FloatRect Tank::getAABB() const {
    return body_.getGlobalBounds();
}
//wektor do strzelania
sf::Vector2f Tank::getForward() const {
    float rad = angleDeg_ * PI / 180.f;
    return { std::cos(rad), std::sin(rad) };
}

void Tank::update(float dt) {
    //dlaczego jest 2x to samo??? -krzys
    if (!movingForward_) {
        angleDeg_ += turnDirection_ * turnSpeed_ * dt;
    }

    float rad = angleDeg_ * PI / 180.f;
     if (!movingForward_) {
        angleDeg_ += turnDirection_ * turnSpeed_ * dt;
    }

    if (movingForward_) {
        body_.move(getForward() * moveSpeed_ * dt);
    }

    body_.setRotation(angleDeg_ + 90.f);
    clampToWorld();
}

void Tank::clampToWorld() {
    if (worldBounds_.width < 0.f || worldBounds_.height < 0.f)
        return;

    sf::Vector2f pos = body_.getPosition();
    sf::FloatRect aabb = getAABB();

    float left   = worldBounds_.left + (aabb.width  * 0.5f);
    float right  = worldBounds_.left + worldBounds_.width  - (aabb.width  * 0.5f);
    float top    = worldBounds_.top  + (aabb.height * 0.5f);
    float bottom = worldBounds_.top  + worldBounds_.height - (aabb.height * 0.5f);

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
