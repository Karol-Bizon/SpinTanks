#pragma once
#include <SFML/Graphics.hpp>

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

    //rzeczy zwiazane z HP - kg

    //set id
    void setID(int ide);
    int getID() { return id_; }

    void takeDamage(float ile);
    bool isAlive() const { return hp_ > 0.f; }
    float getHp() const { return hp_; }
    float getMaxHp() const { return maxHp_; }
    void setMaxHp(float m) { maxHp_ = m; if (hp_ > maxHp_) hp_ = maxHp_; }

    //pasek hp
    void setFont(const sf::Font& font);

private:
    void flipRotationDirection();
    void clampToWorld();

private:
    sf::RectangleShape body_;
    sf::Keyboard::Key controlKey_;

    float angleDeg_ = 0.f;
    float turnSpeed_ = 30.f; // za szybko do testow strzelania, zmieniam na 30.f, bylo 180.f
    float moveSpeed_ = 180.f;
    float turnDirection_ = 1.f;

    bool movingForward_ = false;
    

    sf::FloatRect worldBounds_{0.f, 0.f, -1.f, -1.f};

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
};
