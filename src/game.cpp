#include "game.hpp"
#include<iostream>
#include <random>

int randomInt(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

static constexpr float WORLD_WIDTH  = 1000.f;
static constexpr float WORLD_HEIGHT = 1000.f;
static constexpr float START_POSITION_FRAC = 0.15f;


static const sf::Keyboard::Key SHOOT_KEYS[4] = {
    // sf::Keyboard::LShift,
    // sf::Keyboard::RShift,
    // sf::Keyboard::F,
    // sf::Keyboard::RControl
    sf::Keyboard::Space,
    sf::Keyboard::Enter,
    sf::Keyboard::W,
    sf::Keyboard::Up
};

static const sf::Keyboard::Key TANK_KEYS[4] = {
    sf::Keyboard::Space,
    sf::Keyboard::Enter,
    sf::Keyboard::W,
    sf::Keyboard::Up
};

//pozycje poczatkowe czolgow
static const sf::Vector2f TANK_POSITIONS[4] = {
    {START_POSITION_FRAC*WORLD_WIDTH        , START_POSITION_FRAC*WORLD_HEIGHT},
    {(1.f - START_POSITION_FRAC)*WORLD_WIDTH, (1.f - START_POSITION_FRAC)*WORLD_HEIGHT},
    {(1.f - START_POSITION_FRAC)*WORLD_WIDTH, START_POSITION_FRAC*WORLD_HEIGHT},
    {START_POSITION_FRAC*WORLD_WIDTH        , (1.f - START_POSITION_FRAC)*WORLD_HEIGHT}
};

static const char* TANK_FILES[4] = {
    "graphics/tank1.png",
    "graphics/tank2.png",
    "graphics/tank3.png",
    "graphics/tank4.png"
};

//tekstury powerupow
static const char* POWERUP_FILES[7] = {
    "graphics/generic_powerup.png",
    "graphics/generic_powerup_inv.png",
    "graphics/generic_powerup_heal.png",
    "graphics/generic_powerup_moreDMG.png",
    "graphics/generic_powerup_SPED.png",
    "graphics/generic_powerup_moreHP.png",
    "graphics/generic_powerup_fastRELOAD.png"
};
int upTEX_COUNT = 7; // ile tekstur powerupp

Game::Game()
: window_(sf::VideoMode(
      static_cast<unsigned>(WORLD_WIDTH),
      static_cast<unsigned>(WORLD_HEIGHT)),
      "SpinTanks"),
  maxBuildBlocks_(100),
  builtBlocks_(0),
  editorEnabled_(true),
  state_(GameState::MENU),
  menu_(WORLD_WIDTH, WORLD_HEIGHT)
{   
    //fps
    window_.setFramerateLimit(60);

    worldView_ = sf::View(sf::FloatRect(0.f, 0.f, WORLD_WIDTH, WORLD_HEIGHT));


     if (!backgroundTex_.loadFromFile("graphics/background.png")) {
        //
    } else {
        backgroundSprite_.setTexture(backgroundTex_);
        sf::Vector2u s = backgroundTex_.getSize();
        backgroundSprite_.setScale(
            WORLD_WIDTH  / static_cast<float>(s.x),
            WORLD_HEIGHT / static_cast<float>(s.y)
        );
    }

    if (!topBarTex_.loadFromFile("graphics/ui_top_panel.png")) {
        std::cout << "ERROR: nie moge wczytac ui_top_panel.png\n";
    } else {
        topBarSprite_.setTexture(topBarTex_);

        // skalowanie na szerokość okna i wysokość topUiHeight_
        sf::Vector2u s = topBarTex_.getSize();
        topBarSprite_.setScale(
            WORLD_WIDTH / static_cast<float>(s.x),
            topUiHeight_ / static_cast<float>(s.y)
        );
        topBarSprite_.setPosition(0.f, 0.f);
    }


    if (!fireTex_.loadFromFile("graphics/fire.png")) {
        //
    }

    //ladujemy tekstury powerupow
    for (int i = 0; i < upTEX_COUNT; i++) {
        if (!poweruppTex_[i].loadFromFile(POWERUP_FILES[i])) {
            //
        }
    }


    //stad usunalem do poweruppow dawny kod - kg


    if (!map_.loadAtlasFromFiles({
        "graphics/block.png",
        "graphics/block_s1.png",
        "graphics/block_s2.png"
    })) {
        std::cout << "ERROR: nie moge wczytac atlasu blokow\n";
    }

    map_.create(60, 60, 0);
    map_.setPosition(0.f, 0.f);
    map_.setScale(0.4f, 0.4f); 
    currentBrush_ = 1;

    if (!menuMusic_.openFromFile("audio/menu_music.ogg")) {
    std::cout << "Failed to load menu music!\n";
    } else {
        menuMusic_.setLoop(true);
        menuMusic_.setVolume(40.f); // np. 0–100
    }

    if (!gameMusic_.openFromFile("audio/battle_music.ogg")) {
    std::cout << "Failed to load gameplay music\n";
    } else {
        gameMusic_.setLoop(true);
        gameMusic_.setVolume(35.f);
    }

    // GAME OVER
    if (!gameOverMusic_.openFromFile("audio/final_music.ogg")) {
        std::cout << "Failed to load game over music\n";
    } else {
        gameOverMusic_.setLoop(false);
        gameOverMusic_.setVolume(45.f);
    }

    if (!uiFont_.loadFromFile("graphics/main_font.ttf")) {
        std::cout << "ERROR: nie moge wczytac fontu UI\n";
    }

    introText_.setFont(uiFont_);
    introText_.setCharacterSize(64);
    introText_.setFillColor(sf::Color::White);
    introText_.setString("TRYB RYSOWANIA");
    auto ib = introText_.getLocalBounds();
    introText_.setOrigin(ib.width / 2.f, ib.height / 2.f);
    introText_.setPosition(WORLD_WIDTH / 2.f, WORLD_HEIGHT / 2.f - 80.f);


    fightText_.setFont(uiFont_);
    fightText_.setString("FIGHT");
    fightText_.setCharacterSize(90);
    fightText_.setFillColor(sf::Color::Red);
    auto fb = fightText_.getLocalBounds();
    fightText_.setOrigin(fb.width / 2.f, fb.height / 2.f);
    fightText_.setPosition(WORLD_WIDTH / 2.f, WORLD_HEIGHT / 2.f);

}


void Game::run() {
    while (window_.isOpen()) {

        processEvents();
        updateMusic();
        float dt = clock_.restart().asSeconds();

        if (state_ == GameState::MENU) {

            window_.clear(sf::Color::Black);
            window_.setView(window_.getDefaultView()); 
            menu_.render(window_);
            window_.display();

            if (menu_.startRequested()) {

                startGame(menu_.getPlayerCount());
                state_ = GameState::PLAYING;

            }
            continue;
        }

        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event e;
    while (window_.pollEvent(e)) {

        if (state_ == GameState::MENU) {
            menu_.handleEvent(e);
            if (menu_.quitRequested()) {
                window_.close();
            }
            continue;
        }

        if (state_ == GameState::GAME_OVER) {
            menu_.handleEvent(e);

            if (menu_.quitRequested()) {
                window_.close();
            }

            if (menu_.startRequested()) {
                resetGame();
                menu_.setScreen(Menu::Screen::MAIN);
                state_ = GameState::MENU;
            }    
        }

        if (e.type == sf::Event::Closed)
            window_.close();

        if (!editorEnabled_ && editorPhase_ == EditorPhase::OFF && e.type == sf::Event::KeyPressed) {
            for (std::size_t i = 0; i < tanks_.size(); ++i) {
                if (tankLabelVisible_[i] && e.key.code == TANK_KEYS[i]) {
                    tankLabelVisible_[i] = false;
                    break;
                }
            }
        }

        if (editorEnabled_) {
            handleEditorInput(e);
        } else {
            for (auto& tank : tanks_)
                tank.handleEvent(e);
            handleShootInput(e);
        }
        
    }
}


void Game::update(float dt) {
    if (editorEnabled_ && editorPhase_ == EditorPhase::INTRO) {
        if (phaseClock_.getElapsedTime().asSeconds() >= introShowTime_) {
            editorPhase_ = EditorPhase::BUILD;
        }
        return; // w intro nic nie aktualizujemy
    }

    if (editorEnabled_) return;

    if (editorPhase_ == EditorPhase::PREFIGHT) {
        if (phaseClock_.getElapsedTime().asSeconds() >= preFightDuration_) {
            editorPhase_ = EditorPhase::OFF;
        }
        return;
    }

    for (auto& tank : tanks_) {
        sf::Vector2f before = tank.getPosition();

        tank.update(dt);


        if (tankHitsBlocks(tank)) {
            sf::Vector2f after = tank.getPosition();

            tank.setPosition({ after.x, before.y });
            if (tankHitsBlocks(tank)) {

                tank.setPosition({ before.x, after.y });
                if (tankHitsBlocks(tank)) {
                    tank.setPosition(before);
                }
            }
        }

        sf::Vector2f afterBlocks = tank.getPosition();

        if (tankHitsOtherTanks(tank)) {

            tank.setPosition({ afterBlocks.x, before.y });
            if (tankHitsOtherTanks(tank)) {

                tank.setPosition({ before.x, afterBlocks.y });
                if (tankHitsOtherTanks(tank)) {
                    tank.setPosition(before);
                }
            }
        }
    }


    for (auto& p : projectiles_)
            p.update(dt);
        const sf::FloatRect world{0.f, 0.f, WORLD_WIDTH, WORLD_HEIGHT};

        for (auto& p : projectiles_) {
        if (!world.contains(p.getPosition()))
            p.kill();

        sf::Vector2u cell;
        if (projectileHitCell(p, cell)) {
            damageBlockAt(cell.x, cell.y);
            p.kill();
        }
    }
    for (auto& p : projectiles_) {
        if (!p.isAlive()) continue;

        for (auto& t : tanks_) {
            if (!t.isAlive()) continue;
            if (p.getOwner() == t.getID()) continue;

            if (p.getAABB().intersects(t.getAABB())) {
                projectileHitTank(p, t);
                break;
            }
        }
    }

    ///POWERUPSS ! - kg
    for (auto& up : powerupps_) {
        if (!up.isAlive()) continue;

        for (auto& t : tanks_) {
            if (!t.isAlive()) continue;
            //std::cout << "testujemy po czolgach!" << std::endl;
            if (up.getAABB().intersects(t.getAABB())) {
                tankHitPowerupp(t, up);
                std::cout << "tank hit powerup!" << std::endl;
                break;
            }
        }
    }
    
    //update i spawn powerup�w - kg
    spawnPowerupps();
    for (auto& pu : powerupps_) {
        pu.update(dt);
    }

    //zabijamy analogicznie
    powerupps_.erase(
        std::remove_if(powerupps_.begin(), powerupps_.end(),
            [](const PowerUP& p) { return !p.isAlive(); }),
        powerupps_.end()
    );


    projectiles_.erase(
        std::remove_if(projectiles_.begin(), projectiles_.end(),
                       [](const Projectile& p){ return !p.isAlive(); }),
        projectiles_.end()
    );

    if (state_ == GameState::PLAYING) {
        if (aliveTanks() <= 1) {
            state_ = GameState::GAME_OVER;
            menu_.setScreen(Menu::Screen::GAME_OVER);
            menu_.setTankWinner(getTankWinner());
        }
    }
}

void Game::render() {
    float winW = static_cast<float>(window_.getSize().x);
    float winH = static_cast<float>(window_.getSize().y);

    float topFrac = topUiHeight_ / winH;
    topFrac = std::clamp(topFrac, 0.f, 0.4f);

    worldView_.setViewport(sf::FloatRect(0.f, topFrac, 1.f, 1.f - topFrac));

    window_.clear();

    window_.setView(worldView_);

    window_.draw(backgroundSprite_);

    window_.draw(map_);

    for (auto& pu : powerupps_)
        pu.draw(window_);

    for (auto& tank : tanks_)
        tank.draw(window_);

    for (auto& p : projectiles_)
        p.draw(window_);
    
    if (editorPhase_ == EditorPhase::PREFIGHT || editorPhase_ == EditorPhase::OFF)
        drawTankKeyLabels();

    window_.setView(window_.getDefaultView());

    if (state_ == GameState::PLAYING) {
        drawPowerUpTopBar();
    }

    if (editorPhase_ == EditorPhase::INTRO) {
        drawEditorIntro();
    }

    if (editorPhase_ == EditorPhase::PREFIGHT) {
        drawFightBanner();
    }

    if (state_ == GameState::GAME_OVER) {
        drawPowerUpTopBar();
        menu_.render(window_);
    }

    window_.display();
}

void Game::handleEditorInput(const sf::Event& e) {
    if (editorPhase_ == EditorPhase::INTRO) return;

    if (e.type == sf::Event::KeyPressed) {
        if (editorPhase_ == EditorPhase::BUILD) {
            editorEnabled_ = false;
            editorPhase_ = EditorPhase::PREFIGHT;
            phaseClock_.restart();
            return;
        }
    }

    // blokujemy rysowanie w INTRO 
    if (editorPhase_ != EditorPhase::BUILD) return;

    if (e.type == sf::Event::MouseButtonPressed) {
        if (e.mouseButton.button == sf::Mouse::Left)
            paintAtMouse(false);
        if (e.mouseButton.button == sf::Mouse::Right)
            paintAtMouse(true);
    }

    if (e.type == sf::Event::MouseMoved) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            paintAtMouse(false);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            paintAtMouse(true);
    }
}

void Game::paintAtMouse(bool erase) {
    sf::Vector2i pixel = sf::Mouse::getPosition(window_);

    // jeśli klik w top UI, ignoruj
    if (pixel.y < static_cast<int>(topUiHeight_)) return;

    sf::Vector2f world = window_.mapPixelToCoords(pixel, worldView_);

    sf::Vector2i cell = map_.worldToCell(world);
    if (cell.x < 0 || cell.y < 0) return;
    if (static_cast<unsigned>(cell.x) >= map_.getWidth()) return;
    if (static_cast<unsigned>(cell.y) >= map_.getHeight()) return;

    const unsigned x = static_cast<unsigned>(cell.x);
    const unsigned y = static_cast<unsigned>(cell.y);

    auto id = map_.get(x, y);

    if (erase) {
        if (id != 0) {
            map_.set(x, y, 0);
            if (builtBlocks_ > 0) builtBlocks_--;
        }
        return;
    }

    if (id == 0) {
        if (builtBlocks_ >= maxBuildBlocks_) {
            return;
        }
        if (cellOverlapsAnyTank(x, y)) {
            return;
        }
        map_.set(x, y, 1);
        builtBlocks_++;
    }
}

bool Game::cellOverlapsAnyTank(unsigned cellX, unsigned cellY) const {
    for (const auto& t : tanks_) {
        if (!t.isAlive()) continue;

        sf::Vector2f c = t.getHitboxCenter();

        sf::Vector2i tc = map_.worldToCell(c);

        if (tc.x < 0 || tc.y < 0) continue;

        if (static_cast<unsigned>(tc.x) == cellX &&
            static_cast<unsigned>(tc.y) == cellY) {
            return true; // ten kafel zawiera środek hitboxu
        }
    }
    return false;
}


//strzelanie - tu poprawiam - kg
void Game::handleShootInput(const sf::Event& e) {
    /*
    if (e.type != sf::Event::KeyPressed) return;

    for (std::size_t i = 0; i < tanks_.size(); ++i) {
        if (e.key.code == SHOOT_KEYS[i]) {
            spawnProjectile(tanks_[i]);
        }
    }
    */

    //patch - kg
    if (e.type == sf::Event::KeyPressed) {
        for (std::size_t i = 0; i < tanks_.size(); ++i) {
            if (e.key.code == SHOOT_KEYS[i] && !shotLocked_[i]) {
                spawnProjectile(tanks_[i]);
                shotLocked_[i] = true; //zablokuj dopoki nie będzie KeyReleased
            }
        }
    }
    else if (e.type == sf::Event::KeyReleased) {
        for (std::size_t i = 0; i < tanks_.size(); ++i) {
            if (e.key.code == SHOOT_KEYS[i]) {
                shotLocked_[i] = false; //odblokuj
            }
        }
    }
}

void Game::spawnProjectile(Tank& tank) {
    
    //WAZNE: sprawdzamy czy czolg zyje
    if (!tank.isAlive()) {
        return;
    }

    //TEZ WAZNE: sprawdzamy czy czolg przeladowany
    //Tank k = tank;
    if (!tank.isReloaded()) {
        return;
    }

    sf::Vector2f forward = tank.getForward();
    sf::Vector2f start = tank.getPosition() + forward * 40.f;

    constexpr float bulletSpeed = 500.f;
    sf::Vector2f vel = forward * bulletSpeed;

    projectiles_.emplace_back(fireTex_, start, vel);
    
    //wiemy przez kogo wystrzelony pocisk i ile dmg ma zadac
    projectiles_.back().setOwner(tank.getID());
    projectiles_.back().setDMG(tank.getDMG());
    //std::cout << "Pocisk ide:" << k.getID()<<"<T P>" << projectiles_.back().getOwner()<< std::endl; //debug - dziala, pociski maja dobre id

    //po strzale - przeladowujemy
    tank.Reload();
}


//tu trza troche pogrzebac
bool Game::tankHitsBlocks(const Tank& t) const {

    sf::Vector2f center = t.getHitboxCenter();
    float radius = t.getHitboxRadius();

    // ⬇⬇⬇ TO JEST TWÓJ STARY KOD, NIETKNIĘTY ⬇⬇⬇

    sf::Vector2i c0 = map_.worldToCell({center.x - radius, center.y - radius});
    sf::Vector2i c1 = map_.worldToCell({center.x + radius, center.y + radius});

    int minX = std::max(0, std::min(c0.x, c1.x));
    int maxX = std::min<int>(static_cast<int>(map_.getWidth()) - 1,
                             std::max(c0.x, c1.x));
    int minY = std::max(0, std::min(c0.y, c1.y));
    int maxY = std::min<int>(static_cast<int>(map_.getHeight()) - 1,
                             std::max(c0.y, c1.y));

    const sf::Vector2u ts = map_.getTileSizePx();
    const sf::Vector2f sc = map_.getScale();

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (map_.get(static_cast<unsigned>(x), static_cast<unsigned>(y)) != 0) {

                sf::Vector2f cellPos =
                    map_.cellToWorld(static_cast<unsigned>(x),
                                     static_cast<unsigned>(y));

                sf::FloatRect tileRect(
                    cellPos.x, cellPos.y,
                    ts.x * sc.x, ts.y * sc.y
                );

                // ===== JEDYNA ZMIANA =====
                float cx = std::clamp(center.x,
                                      tileRect.left,
                                      tileRect.left + tileRect.width);
                float cy = std::clamp(center.y,
                                      tileRect.top,
                                      tileRect.top + tileRect.height);

                float dx = center.x - cx;
                float dy = center.y - cy;

                if (dx*dx + dy*dy < radius*radius)
                    return true;
            }
        }
    }

    return false;

    // sf::FloatRect box = t.getHitBox();

    // sf::Vector2i c0 = map_.worldToCell({box.left, box.top});
    // sf::Vector2i c1 = map_.worldToCell({box.left + box.width, box.top + box.height});

    // int minX = std::max(0, std::min(c0.x, c1.x));
    // int maxX = std::min<int>(static_cast<int>(map_.getWidth()) - 1, std::max(c0.x, c1.x));
    // int minY = std::max(0, std::min(c0.y, c1.y));
    // int maxY = std::min<int>(static_cast<int>(map_.getHeight()) - 1, std::max(c0.y, c1.y));

    // const sf::Vector2u ts = map_.getTileSizePx();
    // const sf::Vector2f sc = map_.getScale();

    // for (int y = minY; y <= maxY; ++y) {
    //     for (int x = minX; x <= maxX; ++x) {
    //         if (map_.get(static_cast<unsigned>(x), static_cast<unsigned>(y)) != 0) {
    //             sf::Vector2f cellPos = map_.cellToWorld(static_cast<unsigned>(x), static_cast<unsigned>(y));
    //             sf::FloatRect tileRect(cellPos.x, cellPos.y,
    //                                    ts.x * sc.x, ts.y * sc.y);

    //             if (box.intersects(tileRect))
    //                 return true;
    //         }
    //     }
    // }
    // return false;


}

bool Game::tankHitsOtherTanks(const Tank& me) const {
    // if (!me.isAlive()) return false;

    // const sf::FloatRect box = me.getHitBox();

    // for (const auto& t : tanks_) {
    //     if (&t == &me) continue;
    //     if (!t.isAlive()) continue;

    //     sf::FloatRect otherRect = t.getHitBox();

    //     if (box.intersects(otherRect)) {
    //         return true;
    //     }
    // }
    // return false;

    sf::Vector2f c1 = me.getHitboxCenter();
    float r1 = me.getHitboxRadius();

    for (const auto& notme : tanks_) {
        if (&notme == &me) continue;

        sf::Vector2f c2 = notme.getHitboxCenter();
        float r2 = notme.getHitboxRadius();
        sf::Vector2f d = c1 - c2;
        if (d.x*d.x + d.y*d.y < (r1 + r2)*(r1 + r2)) {
            return true;
        }
    }
    return false;
}

bool Game::projectileHitCell(const Projectile& p, sf::Vector2u& outCell) const {
    sf::Vector2f center = p.getCenter();

    sf::Vector2i cell = map_.worldToCell(center);
    if (cell.x < 0 || cell.y < 0) return false;
    if (static_cast<unsigned>(cell.x) >= map_.getWidth()) return false;
    if (static_cast<unsigned>(cell.y) >= map_.getHeight()) return false;

    if (map_.get(static_cast<unsigned>(cell.x), static_cast<unsigned>(cell.y)) != 0) {
        outCell = { static_cast<unsigned>(cell.x), static_cast<unsigned>(cell.y) };
        return true;
    }
    return false;
}


void Game::damageBlockAt(unsigned x, unsigned y) {
    TileMap::TileId id = map_.get(x, y);
    if (id == 1) map_.set(x, y, 2);
    else if (id == 2) map_.set(x, y, 3);
    else if (id == 3) {
        map_.set(x, y, 0);
        if (builtBlocks_ > 0) builtBlocks_--;
    }
}


//obrazenia
void Game::projectileHitTank(Projectile& p, Tank& t) {
    float DAMAGE = p.getDMG();
    t.takeDamage(DAMAGE);
    p.kill();
}

//powerupp
void Game::tankHitPowerupp(Tank& t, PowerUP& up) {
    Bonuses b = up.getBonuses();
    up.kill();
    t.PowerrUpSTART();
    //ustawiamy po kolei rzeczy z powerupa:

    //damage - dziala jako mnoznik!
    if (b.dmgBONUS != 0.f) { t.setDMG(t.getDMG() * b.dmgBONUS); }

    //healing
    if (b.healBONUS_ != 0.f) { t.takeDamage(-b.healBONUS_); }

    //maxHP
    if (b.maxhpBONUS_ != 0.f) {
        t.setMaxHp(b.maxhpBONUS_);
        t.takeDamage(-77777.f);
    }

    //reload time
    if (b.reloadBONUS_ != 0.f) { t.setReloadTime(b.reloadBONUS_); }

    //rotacja sped
    if (b.rotspedBONUS_ != 0.f) { t.setTurnSpeed(b.rotspedBONUS_); }

    //ruchsped
    if (b.spedBONUS_ != 0.f) { t.setMoveSpeed(b.spedBONUS_); }

}
//power upp spawn - kg
void Game::spawnPowerupps() {
    if (PowerUp_cooldown_.getElapsedTime().asSeconds() >= PowerUp_cooldownTIME_) {
        canSpawnPowerUp_ = true;
    }
    if (!canSpawnPowerUp_) {
        return;
    }
    //jesli mozemy
    const sf::FloatRect world{0.f, 0.f, WORLD_WIDTH, WORLD_HEIGHT};


    //TESTOWY spawn powerupp co 20s
    //wybieramy pomiedzy 3 klasami power up: heal, dmg, speed
    int ktory = randomInt(1,5); // haha zaczynam liczyc od jedynki >:)


    powerupps_.reserve(1);

    //tu sie dodaje powerupy
    switch (ktory){
    case 1: //heal
        powerupps_.emplace_back(poweruppTex_[2], sf::Vector2f{ 500.f, 500.f });
        powerupps_.back().placeRandom(world);
        powerupps_.back().setBonuses(0.f, 0.f, 20.f, 0.f, 0.f, 0.f);
        break;
    case 2: //dmgboost
        powerupps_.emplace_back(poweruppTex_[3], sf::Vector2f{ 500.f, 500.f });
        powerupps_.back().placeRandom(world);
        powerupps_.back().setBonuses(4.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        break;
    case 3: //speed
        powerupps_.emplace_back(poweruppTex_[4], sf::Vector2f{ 500.f, 500.f });
        powerupps_.back().placeRandom(world);
        powerupps_.back().setBonuses(0.f, 0.f, 0.f, 0.f, 60.f, 360.f);
        break;
    case 4: //maxhp
        powerupps_.emplace_back(poweruppTex_[5], sf::Vector2f{ 500.f, 500.f });
        powerupps_.back().placeRandom(world);
        powerupps_.back().setBonuses(0.f, 0.f, 0.f, 400.f, 0.f, 0.f);
        break;
    case 5: //reloadspeed
        powerupps_.emplace_back(poweruppTex_[6], sf::Vector2f{ 500.f, 500.f });
        powerupps_.back().placeRandom(world);
        powerupps_.back().setBonuses(0.f, 0.2f, 0.f, 0.f, 0.f, 0.f);
        break;
    default:
        break;
    }


    //for (int i = 0; i < 2; i++) {
    //    powerupps_.emplace_back(poweruppTex_[i], sf::Vector2f{ 500.f, 500.f });
    //    powerupps_.back().placeRandom(world);
    //    powerupps_.back().setTexture(poweruppTex_[0]); // tak mozna ustawic teksture jaka sie chce powerupps
    //}
    canSpawnPowerUp_ = false;
    PowerUp_cooldown_.restart();
}

void Game::startGame(std::size_t tankCount) {
    
    //upewniamy sie co do ilosci czolgow
    tankCount = std::clamp(tankCount, std::size_t{1}, std::size_t{4});

    //chyba ladujemy tekstury czolgow
    for (std::size_t i = 0; i < tankCount; ++i) {
        if (!tankTex_[i].loadFromFile(TANK_FILES[i])) {
        }
    }

    tanks_.reserve(tankCount);
    //granice swiata
    const sf::FloatRect world{0.f, 0.f, WORLD_WIDTH, WORLD_HEIGHT};

    //spawn czolgow
    for (std::size_t i = 0; i < tankCount; ++i) {
        tanks_.emplace_back(TANK_POSITIONS[i], TANK_KEYS[i], tankTex_[i]);

        //dodalem ze wiemy ktory czolg jest ktory bo maja ID od 1 do 4 ! -kg
        Tank& t = tanks_.back();
        t.setID(i+1);
        //t.setFont(); //pozniej trza dodac zeby byl font!!! aka plik z fontem danym
        tanks_.back().setWorldBounds(world);
    }

    shotLocked_.assign(tanks_.size(), false);

    editorEnabled_ = true;
    editorPhase_ = EditorPhase::INTRO;
    phaseClock_.restart();
    for (int i = 0; i < 4; ++i) tankLabelVisible_[i] = true;
}

std::size_t Game::aliveTanks() const {
    std::size_t count = 0;
    for (const auto& t : tanks_) {
        if (t.isAlive())
            ++count;
    }
    return count;
}

void Game::updateMusic() {

    auto stopAll = [&]() {
        menuMusic_.stop();
        gameMusic_.stop();
        gameOverMusic_.stop();
    };

    switch (state_) {
        case GameState::MENU:
            if (menuMusic_.getStatus() != sf::Music::Playing) {
                stopAll();
                menuMusic_.play();
            }
            break;

        case GameState::PLAYING:
            if (gameMusic_.getStatus() != sf::Music::Playing) {
                stopAll();
                gameMusic_.play();
            }
            break;

        case GameState::GAME_OVER:
            if (gameOverMusic_.getStatus() != sf::Music::Playing) {
                stopAll();
                gameOverMusic_.play();
            }
            break;
    }
}

std::string Game::keyToString(sf::Keyboard::Key k) const {
    switch (k) {
        case sf::Keyboard::Space: return "SPACE";
        case sf::Keyboard::Enter: return "ENTER";
        case sf::Keyboard::W:     return "W";
        case sf::Keyboard::Up:    return "UP";
        default: return "?";
    }
}

void Game::drawFightBanner() {
    window_.draw(fightText_);
}

void Game::drawTankKeyLabels() {
    if (!uiFont_.getInfo().family.empty()) {
        for (std::size_t i = 0; i < tanks_.size(); ++i) {
            if (!tanks_[i].isAlive()) continue;
            if (!tankLabelVisible_[i]) continue;

            sf::Text t;
            t.setFont(uiFont_);
            t.setCharacterSize(18);
            t.setFillColor(sf::Color::White);

            t.setString(keyToString(TANK_KEYS[i]));

            sf::Vector2f pos = tanks_[i].getPosition();
            t.setPosition(pos.x - 24.f, pos.y - 85.f); 

            window_.draw(t);
        }
    }
}

void Game::drawEditorIntro() {

    sf::Text sub;
    sub.setFont(uiFont_);
    sub.setCharacterSize(26);
    sub.setFillColor(sf::Color(220, 220, 220));
    auto b = sub.getLocalBounds();
    sub.setOrigin(b.width / 2.f, b.height / 2.f);
    sub.setPosition(WORLD_WIDTH / 2.f, WORLD_HEIGHT / 2.f - 10.f);

    window_.draw(introText_);
    window_.draw(sub);
}

static sf::RectangleShape makeTextBackdrop(const sf::Text& txt, float padding = 6.f, sf::Color color = sf::Color(0,0,0,110))
{
    sf::FloatRect b = txt.getGlobalBounds();
    sf::RectangleShape r;
    r.setPosition(b.left - padding, b.top - padding);
    r.setSize({ b.width + 2.f * padding, b.height + 2.f * padding });
    r.setFillColor(color);
    r.setOutlineThickness(1.f);
    r.setOutlineColor(sf::Color(255, 255, 255, 18));
    return r;
}


void Game::drawPowerUpTopBar() {
    window_.draw(topBarSprite_);

    //TRYB EDYTORA
    if (editorEnabled_ && (editorPhase_ == EditorPhase::INTRO || editorPhase_ == EditorPhase::BUILD)) {

        sf::Text title;
        title.setFont(uiFont_);
        title.setCharacterSize(26);
        title.setFillColor(sf::Color::White);
        title.setString("TRYB RYSOWANIA");
        title.setPosition(20.f, 15.f);

        window_.draw(makeTextBackdrop(title, 7.f, sf::Color(0,0,0,200)));
        window_.draw(title);

        sf::Text info;
        info.setFont(uiFont_);
        info.setCharacterSize(18);
        info.setFillColor(sf::Color(230, 230, 230));

        std::string blocks =
            "Dostepne bloki: " + std::to_string(maxBuildBlocks_ - builtBlocks_) +
            " / " + std::to_string(maxBuildBlocks_);

        info.setString(
            "LPM - stawianie    |    PPM - usuwanie    |    ENTER - zatwierdzenie\n" +
            blocks + "\n" +
            "Nie mozna stawiac bloku na kaflu ze srodkiem hitboxu czolgu"
        );
        info.setPosition(20.f, 55.f);

        window_.draw(makeTextBackdrop(info, 8.f, sf::Color(0,0,0,200)));
        window_.draw(info);

        return;
    }

    //TRYB GRY 
    sf::Text title;
    title.setFont(uiFont_);
    title.setCharacterSize(26);
    title.setFillColor(sf::Color::White);
    title.setString("POWER-UPY");
    title.setPosition(20.f, 15.f);

    window_.draw(makeTextBackdrop(title, 7.f, sf::Color(0,0,0,200)));
    window_.draw(title);

    float x = 20.f;
    float y = 55.f;

    struct Item { const char* name; const char* desc; int tex; };
    static Item items[] = {
        {"HEAL", "HEALS 20 HP", 2},
        {"DMG BOOST", "DMG x4", 3},
        {"SPEED", "FAST MOVE\nSLOW TURN", 4},
        {"MAX HP", "SETS MAXHP TO 400", 5},
        {"FAST RELOAD", "RELOAD TIME x0.4", 6},
    };

    const float blockW = 190.f;
    const float iconSize = 34.f;

    for (auto &it : items) {
        sf::Sprite icon;
        icon.setTexture(poweruppTex_[it.tex]);
        auto b = icon.getLocalBounds();
        icon.setOrigin(b.width * 0.5f, b.height * 0.5f);
        icon.setScale(iconSize / b.width, iconSize / b.height);
        icon.setPosition(x + 18.f, y + 18.f);
        window_.draw(icon);

        sf::Text name;
        name.setFont(uiFont_);
        name.setCharacterSize(16);
        name.setFillColor(sf::Color::White);
        name.setString(it.name);
        name.setPosition(x + 40.f, y + 2.f);

        window_.draw(makeTextBackdrop(name, 5.f, sf::Color(0,0,0,200)));
        window_.draw(name);

        sf::Text desc;
        desc.setFont(uiFont_);
        desc.setCharacterSize(14);
        desc.setFillColor(sf::Color(220, 220, 220));
        desc.setString(it.desc);
        desc.setPosition(x + 40.f, y + 24.f);

        window_.draw(makeTextBackdrop(desc, 5.f, sf::Color(0,0,0,200)));
        window_.draw(desc);

        x += blockW;
        if (x + blockW > WORLD_WIDTH) break;
    }

}

void Game::resetGame() {

    // --- TANKI ---
    tanks_.clear();

    // --- POCISKI ---
    projectiles_.clear();


    // --- EDYTOR ---
    editorEnabled_ = false;
    editorPhase_ = EditorPhase::OFF;

    map_.clear();

    builtBlocks_ = 0;
}

int Game::getTankWinner() {
    if(aliveTanks() == 1) {
        for (auto& t : tanks_) {
            if (t.isAlive()) {
                return t.getID();
            }
        }
    }
    return -1;
}