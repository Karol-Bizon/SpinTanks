#include "tank.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>

static constexpr float PI = 3.14159265f;

//definicja czolgu, jego rozmiar chyba nie rowna sie rozmiarowi tekstury --- obserwowalne podczas gry -krzys
Tank::Tank(sf::Vector2f position, sf::Keyboard::Key controlKey, const sf::Texture& texture)
: controlKey_(controlKey)
{
    body_.setSize({60.f * 1.5f, 60.f * 1.5f}); // tu zmieniam, bylo 60.f robie 30.f, nvm, nie to
    body_.setTexture(&texture, true);
    body_.setOrigin(body_.getSize() * 0.5f);
    body_.setPosition(position);

    //rzeczy pasek hp - kg
    //HP bar background
    hpBarBack_.setSize({ hpBarWidth_, hpBarHeight_ });
    hpBarBack_.setOrigin(hpBarWidth_ / 2.f, hpBarHeight_ / 2.f);
    hpBarBack_.setFillColor(sf::Color(50, 50, 50, 200));

    //HP bar front (green)
    hpBarFront_.setSize({ hpBarWidth_, hpBarHeight_ });
    hpBarFront_.setOrigin(hpBarWidth_ / 2.f, hpBarHeight_ / 2.f);
    hpBarFront_.setFillColor(sf::Color::Green);

    //hp text (font must byæ ustawiony przez setFont jeœli chcesz tekst)
    hpText_.setCharacterSize(12);
    hpText_.setFillColor(sf::Color::White);

    //rzeczy przeladowanie - kg
    //reload bar background
    reloadBarBack_.setSize({ reloadBarWidth_, reloadBarHeight_ });
    reloadBarBack_.setOrigin(reloadBarWidth_ / 2.f, reloadBarHeight_ / 2.f);
    reloadBarBack_.setFillColor(sf::Color(50, 50, 50, 200));

    //reload bar front
    reloadBarFront_.setSize({ reloadBarWidth_, reloadBarHeight_ });
    reloadBarFront_.setOrigin(0.f, reloadBarHeight_ / 2.f); //origin po lewej
    reloadBarFront_.setFillColor(reloadColor_);
}

void Tank::setFont(const sf::Font& font) {
    font_ = &font;
    hpText_.setFont(font);
}


// apply damage
void Tank::takeDamage(float amount) {
    //if (amount <= 0.f) return;
    hp_ -= amount;
    hp_ = std::max(0.f, hp_);
    hp_ = std::min(hp_, maxHp_);
    // flash tank
    recentlyDamaged_ = true;
    damageClock_.restart();
    body_.setFillColor(sf::Color::Red);
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


//setid
void Tank::setID(int ide) {
    id_ = ide;
    //customowe kolorki hp barow
    static const sf::Color hpColors[4] = {
    sf::Color::Red,
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Magenta
    };
    hpBarFront_.setFillColor(hpColors[id_-1]);
}


void Tank::update(float dt) {

    //moj dodatek kg
    //naprawa koloru

    if (isPoweredUP_) {
        body_.setFillColor(sf::Color(15, 250, 15));
    }

    if (recentlyDamaged_) {
        if (damageClock_.getElapsedTime().asSeconds() > damageFlashDuration_) {
            recentlyDamaged_ = false;
            body_.setFillColor(originalColor_);
        }
    }
    //HP bar position (nad czo³giem)
    sf::Vector2f pos = body_.getPosition();
    float barX = pos.x;
    float barY = pos.y - (body_.getSize().y / 2.f) - hpBarOffsetY_;
    hpBarBack_.setPosition(barX, barY);

    //front width proportional to HP
    float ratio = (maxHp_ > 0.f) ? (hp_ / maxHp_) : 0.f;
    ratio = std::clamp(ratio, 0.f, 1.f);

    //Ustaw origin po lewej krawêdzi
    hpBarFront_.setOrigin(0.f, hpBarHeight_ / 2.f);
    hpBarFront_.setSize({ hpBarWidth_ * ratio, hpBarHeight_ });
    hpBarFront_.setPosition(barX - hpBarWidth_ / 2.f, barY);

    //update numeric text
    if (font_) {
        std::ostringstream ss;
        ss << static_cast<int>(hp_) << " / " << static_cast<int>(maxHp_);
        hpText_.setString(ss.str());
        sf::FloatRect trect = hpText_.getLocalBounds();
        hpText_.setOrigin(trect.width / 2.f, trect.height / 2.f);
        hpText_.setPosition(barX, barY - 10.f);
    }


    //sprawdzamy pierwyj czy czolg zyje, jak nie zyje to ma miec uciete raczki i nuzki -kg
    if (!isAlive()) {
        return; // tu by mozna moze jakas teksture ustawic czolgu?? albo zeby znikal??
    }

    //ustawienie paska przeladowania - kg
    float reloadBarX = pos.x;
    float reloadBarY = pos.y - (body_.getSize().y / 2.f) - hpBarOffsetY_ + reloadBarOffsetY_; // dopasuj
    reloadBarBack_.setPosition(reloadBarX, reloadBarY);

    //przeladowanie: 1 = gotowy, <1 = w trakcie
    float reloadRatio = 1.f;
    if (!isReloaded_) {
        float elapsed = reloadCLK_.getElapsedTime().asSeconds();
        if (reloadDuration_ <= 0.f) reloadRatio = 1.f;
        else reloadRatio = std::clamp(elapsed / reloadDuration_, 0.f, 1.f);

        if (elapsed >= reloadDuration_) {
            isReloaded_ = true;
        }
    }

    //ustaw front: origin po lewej, szerokoœæ proporcjonalna do ratio
    reloadBarFront_.setOrigin(0.f, reloadBarHeight_ / 2.f);
    reloadBarFront_.setSize({ reloadBarWidth_ * reloadRatio, reloadBarHeight_ });
    reloadBarFront_.setPosition(reloadBarX - reloadBarWidth_ / 2.f, reloadBarY);


    //sprawdzamy czy zakoñczy³o siê prze³adowanie
    if (!isReloaded_) {
        if (reloadCLK_.getElapsedTime().asSeconds() >= reloadDuration_) {
            isReloaded_ = true;
        }
    }
    //std::cout << reloadCLK_.getElapsedTime().asSeconds() << std::endl; //debug timera reloada

    //sprawdzamy i jezeli po timerze, to restorujemy
    RestoreDefaults();


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
    target.draw(hpBarBack_);
    target.draw(hpBarFront_);
    target.draw(reloadBarBack_);
    target.draw(reloadBarFront_);
    if (font_) target.draw(hpText_);

    //DEBUGG ONLY // jak chcecie to usuncie, narazie mi pasuje bo pokazuje co sie dzieje -- podobny hitbox dodalem w pocisku -kg
    //ORAZ zmodyfikowwalem rozmiar tekstury pocisku!!! wazne dla collider
    sf::RectangleShape debug(body_);
    debug.setFillColor(sf::Color::Transparent);
    debug.setOutlineThickness(1.f);
    debug.setOutlineColor(sf::Color::Red);
    target.draw(debug);
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

//logika przeladowania - kg
void Tank::setReloadTime(float ile) {
    reloadDuration_ = std::max(0.f, ile);
}

void Tank::Reload() {
    isReloaded_ = false;
    reloadCLK_.restart();
}

void Tank::setDMG(float ile) {
    DMG_ = ile;
}

//logika czasowego dzialania powerup
void Tank::PowerrUpSTART() {
    isPoweredUP_ = true;
    PowerUpCLK_.restart();
    //body_.setFillColor(sf::Color(167, 250, 246)); //kolor bycia powerupowanym
}

void Tank::RestoreDefaults() {
    if (isPoweredUP_) {
        if (PowerUpCLK_.getElapsedTime().asSeconds() >= powerUpDuration_) {
            isPoweredUP_ = false;

            DMG_ = baza_.BASE_dmg_;
            reloadDuration_ = baza_.BASE_reload_;
            maxHp_ = baza_.BASE_maxHP_;
            turnSpeed_ = baza_.BASE_rotsped_;
            moveSpeed_ = baza_.BASE_sped_;
            body_.setFillColor(originalColor_);
        }
    }
}
