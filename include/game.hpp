#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "tank.hpp"
#include "tilemap.hpp"
#include "projectile.hpp"
#include "powerUPPS.hpp"

enum class GameState {
    MENU,
    PLAYING,
    EXIT
};

class Game {
public:
    explicit Game(std::size_t tankCount, unsigned maxBuildBlocks);
    void run();
    unsigned maxBuildBlocks_ = 8;
    unsigned builtBlocks_ = 0;
    bool editorEnabled_ = true;

private:
    void processEvents();
    void update(float dt);
    void render();

    void handleEditorInput(const sf::Event& e);
    void paintAtMouse(bool erase);

    void handleShootInput(const sf::Event& e);
    //void spawnProjectile(const Tank& tank); nie bedzie zodyn konstantyn, bo reload nie dziala -kg
    void spawnProjectile(Tank& tank);

    bool tankHitsBlocks(const Tank& t) const;
    bool projectileHitCell(const Projectile& p, sf::Vector2u& outCell) const;
    void damageBlockAt(unsigned x, unsigned y);

    void projectileHitTank(Projectile& p, Tank& t);

    void tankHitPowerupp(Tank& t, PowerUP& up);
    void spawnPowerupps();

    GameState state_;
    void renderMenu();

private:
    sf::RenderWindow window_;
    sf::Clock clock_;

    sf::Texture backgroundTex_;
    sf::Sprite  backgroundSprite_;

    sf::Texture tankTex_[4];

    sf::Texture fireTex_;

    sf::Texture poweruppTex_[7];
    sf::Clock PowerUp_cooldown_;
    float PowerUp_cooldownTIME_ = 17.f;
    bool canSpawnPowerUp_ = true;

    std::vector<Tank> tanks_;
    std::vector<Projectile> projectiles_;
    std::vector<PowerUP> powerupps_;

    TileMap map_;
    TileMap::TileId currentBrush_ = 1; 
};
