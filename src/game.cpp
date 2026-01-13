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

//definicje sterowania
static const sf::Keyboard::Key MOVE_KEYS[4] = {
    sf::Keyboard::Space,
    sf::Keyboard::Enter,
    sf::Keyboard::W,
    sf::Keyboard::Up
};

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
}


void Game::run() {
    while (window_.isOpen()) {
        processEvents();
        float dt = clock_.restart().asSeconds();

        if (state_ == GameState::MENU) {

            menu_.render(window_);

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
            continue;
        }

        if (e.type == sf::Event::Closed)
            window_.close();

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
    if (editorEnabled_) return;

    for (auto& tank : tanks_) {
    sf::Vector2f before = tank.getPosition();
    float beforeAng = tank.getAngleDeg();

    tank.update(dt); //ruch czolgow

    if (tankHitsBlocks(tank)) {
        tank.setPosition(before); //powinno cofac czolgi po kolizji z granica mapy, dziala tu, ale gorzej dziala przy kolizji z blokami
    }
}

    for (auto& p : projectiles_)
        p.update(dt);
    const sf::FloatRect world{0.f, 0.f, WORLD_WIDTH, WORLD_HEIGHT};

    for (auto& p : projectiles_) {
        if (!world.contains(p.getPosition()))
            p.kill(); //zabijanie particlesow poza swiatem
        sf::Vector2u cell;
        if (projectileHitCell(p, cell)) {
            damageBlockAt(cell.x, cell.y); //uszkadzanie blokow
            p.kill();
        }
        ///damage ! - kg
        for (auto& p : projectiles_) {
            if (!p.isAlive()) continue; // moze ma byc break a nie continue?

            for (auto& t : tanks_) {
                if (!t.isAlive()) continue;
                if (p.getOwner() == t.getID()) continue; //czy nie strzelamy sami do siebie (NOTE: czolg ma ID, projectile ma OWNER)
                if (p.getAABB().intersects(t.getAABB())) {
                    projectileHitTank(p, t);
                    break;
                }
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
}

void Game::render() {
    window_.clear();

    window_.draw(backgroundSprite_);

    window_.draw(map_);

    //rysowanie powerupps
    for (auto& pu : powerupps_)
        pu.draw(window_);

    for (auto& tank : tanks_)
        tank.draw(window_);

    for (auto& p : projectiles_)
        p.draw(window_);

    window_.display();
}

void Game::handleEditorInput(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Enter) {
            editorEnabled_ = false;
        }
    }

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
    sf::Vector2f world = window_.mapPixelToCoords(pixel);

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
        map_.set(x, y, 1);
        builtBlocks_++;
    }
}


//strzelanie
void Game::handleShootInput(const sf::Event& e) {
    if (e.type != sf::Event::KeyPressed) return;

    for (std::size_t i = 0; i < tanks_.size(); ++i) {
        if (e.key.code == SHOOT_KEYS[i]) {
            spawnProjectile(tanks_[i]);
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
    sf::FloatRect box = t.getAABB();

    sf::Vector2i c0 = map_.worldToCell({box.left, box.top});
    sf::Vector2i c1 = map_.worldToCell({box.left + box.width, box.top + box.height});

    int minX = std::max(0, std::min(c0.x, c1.x));
    int maxX = std::min<int>(static_cast<int>(map_.getWidth()) - 1, std::max(c0.x, c1.x));
    int minY = std::max(0, std::min(c0.y, c1.y));
    int maxY = std::min<int>(static_cast<int>(map_.getHeight()) - 1, std::max(c0.y, c1.y));

    const sf::Vector2u ts = map_.getTileSizePx();
    const sf::Vector2f sc = map_.getScale();

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            if (map_.get(static_cast<unsigned>(x), static_cast<unsigned>(y)) != 0) {
                sf::Vector2f cellPos = map_.cellToWorld(static_cast<unsigned>(x), static_cast<unsigned>(y));
                sf::FloatRect tileRect(cellPos.x, cellPos.y,
                                       ts.x * sc.x, ts.y * sc.y);

                if (box.intersects(tileRect))
                    return true;
            }
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
            //
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

}
