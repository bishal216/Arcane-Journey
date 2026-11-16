#include "level/LevelData.hpp"

#include <iostream>

void LevelData::addStatic(float x, float y, float w, float h, sf::Color col) {
    m_world.addPlat(x, y, w, h, col, false);
}
void LevelData::addGoal(float x, float y, float w, float h) {
    m_world.addPlat(x, y, w, h, sf::Color(255, 215, 0), true);
}
void LevelData::addMoving(float x, float y, float w, float h, sf::Vector2f a, sf::Vector2f b,
                          float speed, sf::Color col) {
    m_moving.add(x, y, w, h, a, b, speed, col);
}
void LevelData::addCrumbling(float x, float y, float w, float h, sf::Color col) {
    m_crumbling.add(x, y, w, h, col);
}
void LevelData::addBounce(float x, float y, float w, float h, float force, sf::Color col) {
    m_bounce.add(x, y, w, h, force, col);
}
void LevelData::addDisappearing(float x, float y, float w, float h, sf::Color col, float on,
                                float off, float fade, float phase) {
    m_disappearing.add(x, y, w, h, col, on, off, fade, phase);
}
void LevelData::addRope(float cx, float topY, float h, sf::Color col) {
    m_climbables.addRope(cx, topY, h, col);
}
void LevelData::addLadder(float cx, float topY, float h, sf::Color col) {
    m_climbables.addLadder(cx, topY, h, col);
}
void LevelData::addTrapdoorOneWay(float x, float y, float w, float h, sf::Color col) {
    m_trapdoors.addOneWay(x, y, w, h, col);
}
void LevelData::addTrapdoorHatch(float x, float y, float w, float h, sf::Color col) {
    m_trapdoors.addHatch(x, y, w, h, col);
}
void LevelData::addTeleport(float x, float y, float w, float h) { m_teleports.add(x, y, w, h); }
void LevelData::addSinking(float x, float y, float w, float h, sf::Color col) {
    m_sinking.add(x, y, w, h, col);
}
void LevelData::addWind(float x, float y, float w, float h, int dir, float strength,
                        sf::Color col) {
    m_wind.add(x, y, w, h, dir, strength, col);
}
void LevelData::addFan(float x, float y, float w, float h, float strength,
                       sf::Color col) {
    m_fan.add(x, y, w, h, strength, col);
}
void LevelData::addKeyDoor(float x, float y, float w, float h, const std::string& switchId, int doorId,
                           sf::Color lockedCol, sf::Color unlockedCol) {
    m_keyDoors.addDoor(x, y, w, h, switchId, doorId, lockedCol, unlockedCol);
}
void LevelData::addKeySwitch(float x, float y, float w, float h, const std::string& switchId,
                             sf::Color col) {
    m_keyDoors.addSwitch(x, y, w, h, switchId, col);
}
void LevelData::addCoin(float cx, float cy) { m_world.addCoin(cx, cy); }
void LevelData::addSection(float worldY, const std::string& name) {
    m_world.addSection(worldY, name);
}
void LevelData::sortSections() { m_world.sortSections(); }
void LevelData::finalize() {
    int unpaired = m_teleports.pairAll();
    if (unpaired > 0)
        std::cerr << "Warning: " << unpaired
                  << " teleport block(s) have no partner (odd number of 'T' spans in level.txt); "
                     "they are drawn with a red outline and will not teleport.\n";
}

// --- Runtime ---

void LevelData::update(float dt) {
    m_moving.update(dt);
    m_crumbling.update(dt);
    m_bounce.update(dt);
    m_disappearing.update(dt);
    m_climbables.update(dt);
    m_trapdoors.update(dt);
    m_sinking.update(dt);
    m_wind.update(dt);
    m_fan.update(dt);
    m_keyDoors.update(dt);
    // Teleport update called separately in resolvePlayer (needs playerPos)
}

void LevelData::draw(sf::RenderWindow& window) const {
    m_world.draw(window);
    m_teleports.draw(window);
    m_climbables.draw(window);
    m_trapdoors.draw(window);
    m_moving.draw(window);
    m_crumbling.draw(window);
    m_bounce.draw(window);
    m_disappearing.draw(window);
    m_sinking.draw(window);
    m_wind.draw(window);
    m_fan.draw(window);
    m_keyDoors.draw(window);
}

void LevelData::resolvePlayer(Player& player, float dt) {
    sf::Vector2f pos = player.position();
    sf::Vector2f vel = player.velocity();
    sf::FloatRect bounds = player.globalBounds();
    bool onGround = player.isOnGround();
    bool dash = player.isDashAvail();
    int jumps = player.jumpsLeft();
    bool climbing = player.isClimbing();
    float climbIn = player.climbInput();

    // Solid collision platforms
    m_moving.resolvePlayer(pos, vel, bounds, onGround, dash, jumps);
    m_crumbling.resolvePlayer(pos, vel, bounds, onGround, dash, jumps);
    m_disappearing.resolvePlayer(pos, vel, bounds, onGround, dash, jumps);
    m_trapdoors.resolvePlayer(pos, vel, bounds, onGround, dash, jumps, player.isDropping());
    m_teleports.resolvePlayer(pos, vel, bounds, onGround, dash, jumps);
    m_sinking.resolvePlayer(pos, vel, bounds, onGround, dash, jumps, dt);
    m_keyDoors.resolvePlayer(pos, vel, bounds, onGround, dash, jumps);

    // Special platforms (bounce, forces)
    m_bounce.resolvePlayer(pos, vel, bounds, onGround);
    m_wind.applyWind(vel, bounds, dt);
    m_fan.applyFan(vel, bounds, dt);

    m_climbables.resolvePlayer(pos, vel, bounds, onGround, climbing, climbIn,
                               player.climbGrabBlocked(), dt);

    // Teleport update needs current onGround state and can modify pos
    m_teleports.update(dt, pos, onGround);

    player.setPosition(pos);
    player.setVelocity(vel);
    player.setOnGround(onGround);
    player.setDashAvail(dash);
    player.setJumpsLeft(jumps);
    player.setClimbing(climbing);
    player.setOnRope(m_climbables.lastGrabWasRope());
    player.setOnOneWay(m_trapdoors.standingOnOneWay());
}

void LevelData::resetDynamic() {
    m_crumbling.reset();
    m_moving.reset();
    m_disappearing.reset();
    m_trapdoors.reset();
    m_teleports.reset();
    m_sinking.reset();
    m_wind.reset();
    m_fan.reset();
    m_keyDoors.reset();
}

const std::vector<Platform>& LevelData::platforms() const { return m_world.platforms(); }
const std::vector<Section>& LevelData::sections() const { return m_world.sections(); }
const std::vector<CoinSpawn>& LevelData::coinSpawns() const { return m_world.coinSpawns(); }
int LevelData::getSectionIndex(float worldY) const { return m_world.getSectionIndex(worldY); }