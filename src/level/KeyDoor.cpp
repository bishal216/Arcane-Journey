#include "level/KeyDoor.hpp"

#include <algorithm>

#include "systems/DiscoveryTracker.hpp"
#include "systems/ArtifactManager.hpp"

KeyDoorManager g_keyDoors;

void KeyDoorManager::addDoor(float x, float y, float w, float h, const std::string& switchId, int doorId,
                             sf::Color lockedCol, sf::Color unlockedCol) {
    m_doors.emplace_back(x, y, w, h, switchId, doorId, lockedCol, unlockedCol);
}

void KeyDoorManager::addSwitch(float x, float y, float w, float h, const std::string& switchId,
                               sf::Color col) {
    m_switches.emplace_back(x, y, w, h, switchId, col);
}

void KeyDoorManager::update(float dt) {
    for (auto& d : m_doors) d.update(dt);
    for (auto& s : m_switches) s.update(dt);
}

void KeyDoorManager::draw(sf::RenderWindow& window) const {
    for (const auto& d : m_doors) d.draw(window);
    for (const auto& s : m_switches) s.draw(window);
}

void KeyDoorManager::reset() {
    for (auto& d : m_doors) d.reset();
    for (auto& s : m_switches) s.reset();
}

void KeyDoorManager::syncDoorStates() {
    for (auto& d : m_doors) {
        bool anySwitchOn = false;
        for (const auto& s : m_switches) {
            if (s.switchId == d.switchId && s.activated) {
                anySwitchOn = true;
                break;
            }
        }
        if (anySwitchOn) d.unlock();
        else d.lock();
    }
}

bool KeyDoorManager::tryActivateSwitch(const sf::FloatRect& playerRect) {
    for (auto& s : m_switches) {
        if (s.bounds().findIntersection(playerRect)) {
            if (s.tryActivate()) {
                syncDoorStates();
                return true;
            }
        }
    }
    return false;
}

void KeyDoorManager::resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                                   bool& onGround, bool& dashAvail, int& jumpsLeft) {
    for (auto& d : m_doors) {
        if (!d.isSolid()) continue;

        sf::FloatRect pr = d.bounds();
        if (!playerRect.findIntersection(pr)) continue;

        float playerBottom = playerRect.position.y + playerRect.size.y;
        float playerTop = playerRect.position.y;
        float platTop = pr.position.y;
        float platBottom = pr.position.y + pr.size.y;
        float playerLeft = playerRect.position.x;
        float playerRight = playerRect.position.x + playerRect.size.x;
        float platLeft = pr.position.x;
        float platRight = pr.position.x + pr.size.x;

        float overlapTop = playerBottom - platTop;
        float overlapBottom = platBottom - playerTop;
        float overlapLeft = playerRight - platLeft;
        float overlapRight = platRight - playerLeft;

        float minOverlap = std::min({overlapTop, overlapBottom, overlapLeft, overlapRight});

        if (minOverlap == overlapTop && playerVel.y >= 0.f) {
            playerPos.y = platTop - playerRect.size.y / 2.f;
            playerVel.y = 0.f;
            onGround = true;
            dashAvail = true;
            jumpsLeft = 1;
        } else if (minOverlap == overlapBottom && playerVel.y < 0.f) {
            playerPos.y = platBottom + playerRect.size.y / 2.f;
            playerVel.y = 0.f;
        } else if (minOverlap == overlapLeft) {
            playerPos.x = platLeft - playerRect.size.x / 2.f;
            playerVel.x = 0.f;
        } else if (minOverlap == overlapRight) {
            playerPos.x = platRight + playerRect.size.x / 2.f;
            playerVel.x = 0.f;
        }

        playerRect.position = {playerPos.x - playerRect.size.x / 2.f,
                               playerPos.y - playerRect.size.y / 2.f};
    }
}