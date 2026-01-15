#include "projectile.hpp"

Projectile::Projectile(const sf::Texture& tex, sf::Vector2f pos, sf::Vector2f vel)
: vel_(vel)
{
    sprite_.setTexture(tex);
    auto b = sprite_.getLocalBounds();
    sprite_.setOrigin(b.width * 0.5f, b.height * 0.5f);
    sprite_.setPosition(pos);
}

void Projectile::update(float dt) {
    sprite_.move(vel_ * dt);
}

void Projectile::draw(sf::RenderTarget& target) const {
    target.draw(sprite_);

    //DEBUG - pokazuje collider pocisku (ogromnie wielki byl oryginalnie XD)
    // sf::CircleShape debug;
    // float r = getHitboxRadius();
    // debug.setRadius(r);
    // debug.setOrigin(r, r);
    // debug.setPosition(getHitboxCenter());
    // debug.setFillColor(sf::Color::Transparent);
    // debug.setOutlineThickness(1.f);
    // debug.setOutlineColor(sf::Color::Green);
    // target.draw(debug);
}
//wiemy kto strzela
void Projectile::setOwner(int tankn) {
    owner_ = tankn;
}

void Projectile::setDMG(float ile) {
    damage_ = ile;
}

sf::Vector2f Projectile::getHitboxCenter() const {
    return sprite_.getPosition();
}

float Projectile::getHitboxRadius() const {
    // logiczny promień – MNIEJSZY niż sprite
    sf::FloatRect b = sprite_.getLocalBounds();
    return std::min(b.width, b.height) * 0.35f;
}