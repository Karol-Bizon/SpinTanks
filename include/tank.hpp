#pragma once
#include <SFML/Graphics.hpp>


struct baseParameters {
    //PODSTAWOWE PARAMETRY CZOLGU
    float BASE_dmg_ = 7.7f;
    float BASE_reload_ = 0.5f;
    float BASE_maxHP_ = 100.f;
    float BASE_rotsped_ = 90.f;
    float BASE_sped_ = 180.f;
};

namespace TankParameters {
    inline float tankSize = 80.f;
}

class Tank {
public:
    Tank(sf::Vector2f position, sf::Keyboard::Key controlKey, const sf::Texture& texture);

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

    sf::Vector2f getForward() const;
    void setPosition(sf::Vector2f p) { body_.setPosition(p); }

    sf::FloatRect getHitBox() const;
    sf::Vector2f getHitboxCenter() const;
    float getHitboxRadius() const;

    //rzeczy zwiazane z HP - kg

    //set id
    void setID(int ide);
    int getID() { return id_; }

    void takeDamage(float ile);
    bool isAlive() const { return hp_ > 0.f; }
    float getHp() const { return hp_; }
    float getMaxHp() const { return maxHp_; }
    void setMaxHp(float m) { maxHp_ = m; if (hp_ > maxHp_) hp_ = maxHp_; }
    float getDMG() { return DMG_; }
    void setDMG(float ile);

    //pasek hp
    void setFont(const sf::Font& font);

    //rzeczy zwiazane z RELOAD - kg
    void setReloadTime(float ile);
    bool isReloaded() const { return isReloaded_; }
    void Reload();


    void setReloadColor(const sf::Color& c) { reloadColor_ = c; reloadBarFront_.setFillColor(reloadColor_); }
    void setReloadBarSize(float w, float h) {
        reloadBarWidth_ = w; reloadBarHeight_ = h;
        reloadBarBack_.setSize({ reloadBarWidth_, reloadBarHeight_ });
        reloadBarFront_.setSize({ reloadBarWidth_, reloadBarHeight_ });
    }

    //rzeczy przywracanie podstawowych wartosci
    //funkcja odpalajaca clock i 
    void PowerrUpSTART();
    void RestoreDefaults();

    //rzeczy ktore sie da ustawic: move speed, turn speed, maxhp, dmg, reload time (WSZYSTKO yipii) - kg

private:
    void flipRotationDirection();
    void clampToWorld();

private:
    sf::RectangleShape body_;
    sf::Keyboard::Key controlKey_;
    float angleDeg_ = 0.f;
    float turnSpeed_ = 140.f; // za szybko do testow strzelania, zmieniam na 30.f, bylo 180.f
    float moveSpeed_ = 180.f;
    float turnDirection_ = 1.f;
    float DMG_ = 7.7f;

    bool movingForward_ = false;
    

    sf::FloatRect worldBounds_{0.f, 0.f, -1.f, -1.f};

    //dodatek hp - kg
    //id
    int id_ = 0;


    //zycie
    float hp_ = 100.f;
    float maxHp_ = 100.f;
    //bool isAlive_ = true;

    //hp bar graphics
    sf::RectangleShape hpBarBack_;
    sf::RectangleShape hpBarFront_;
    float hpBarWidth_ = 48.f;
    float hpBarHeight_ = 6.f;
    float hpBarOffsetY_ = 18.f;

    //optional numeric text
    const sf::Font* font_ = nullptr;
    sf::Text hpText_;

    //damage flash
    mutable sf::Clock damageClock_;
    float damageFlashDuration_ = 0.14f;
    bool recentlyDamaged_ = false;
    sf::Color originalColor_ = sf::Color::White;

    /// reload logika - kg
    bool isReloaded_ = true;
    sf::Clock reloadCLK_;
    float reloadDuration_ = 0.5f;

    //pasek przeladowania
    sf::RectangleShape reloadBarBack_;
    sf::RectangleShape reloadBarFront_;
    float reloadBarWidth_ = 48.f;
    float reloadBarHeight_ = 6.f;
    float reloadBarOffsetY_ = 7.f; //odleg�o�� od �rodka czo�gu (np. poni�ej HP)
    sf::Color reloadColor_ = sf::Color(200, 200, 50); //domy�lny kolor (zolty)

    //rzeczy reload podstawowych wartosci
    baseParameters baza_;
    sf::Clock PowerUpCLK_;
    float powerUpDuration_ = 10.f; //10s
    bool isPoweredUP_ = false;
};
