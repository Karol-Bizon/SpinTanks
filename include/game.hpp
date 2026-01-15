#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include "tank.hpp"
#include "tilemap.hpp"
#include "projectile.hpp"
#include "powerUPPS.hpp"
#include "menu.hpp"



enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

class Game {
public:
    explicit Game();
    void run();
    unsigned maxBuildBlocks_ = 100;
    unsigned builtBlocks_ = 0;
    bool editorEnabled_ = true;

private:
    void processEvents();
    void update(float dt);
    void render();

    void startGame(std::size_t tankCount);

    void handleEditorInput(const sf::Event& e);
    void paintAtMouse(bool erase);

    void handleShootInput(const sf::Event& e);
    //void spawnProjectile(const Tank& tank); nie bedzie zodyn konstantyn, bo reload nie dziala -kg
    void spawnProjectile(Tank& tank);

    bool tankHitsBlocks(const Tank& t) const;
    bool projectileHitCell(const Projectile& p, sf::Vector2u& outCell) const;
    void damageBlockAt(unsigned x, unsigned y);
    bool cellOverlapsAnyTank(unsigned cellX, unsigned cellY) const;

    void projectileHitTank(Projectile& p, Tank& t);
    bool tankHitsOtherTanks(const Tank& me) const;

    void tankHitPowerupp(Tank& t, PowerUP& up);
    void spawnPowerupps();

    GameState state_;
    Menu menu_;

    std::size_t aliveTanks() const;

    sf::Music menuMusic_;
    sf::Music gameMusic_;
    sf::Music gameOverMusic_;

    void updateMusic();

    std::string keyToString(sf::Keyboard::Key k) const;
    void drawEditorIntro();    
    void drawFightBanner();
    void drawTankKeyLabels();

    void drawPowerUpTopBar();

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

    //strzal jeden per wcisniecie guzika strzalu - kg
    std::vector<bool> shotLocked_;

    enum class EditorPhase { INTRO, BUILD, PREFIGHT, OFF };

    EditorPhase editorPhase_ = EditorPhase::INTRO;

    sf::Font uiFont_;
    sf::Text introText_;            
    float introShowTime_ = 1.2f;    

    sf::Text fightText_;

    sf::Clock phaseClock_;

    float preFightDuration_ = 1.0f;  

    struct PowerUpInfo {
        std::string name;
        std::string desc;
        int texIndex; 
    };

    sf::View worldView_;
    float topUiHeight_ = 140.f; 

    sf::Texture topBarTex_;
    sf::Sprite  topBarSprite_;

    bool tankLabelVisible_[4] = { true, true, true, true };

    void resetGame();
};
