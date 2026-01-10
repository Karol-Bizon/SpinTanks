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
}
