#include "level/FanPlatform.hpp"

#include <algorithm>

#include "systems/DiscoveryTracker.hpp"
#include "systems/ArtifactManager.hpp"

FanPlatformManager g_fanPlatforms;

void FanPlatformManager::add(float x, float y, float w, float h, float strength, sf::Color col) {
    m_platforms.emplace_back(x, y, w, h, strength, col);
}

void FanPlatformManager::update(float dt) {
    for (auto& p : m_platforms) p.update(dt);
}

void FanPlatformManager::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_platforms) p.draw(window);
}

void FanPlatformManager::reset() {
    for (auto& p : m_platforms) {
        p.shape.setPosition(p.origin);
        p.particleTimer = 0.f;
        p.shape.setFillColor(p.baseColor);
    }
}

void FanPlatformManager::applyFan(sf::Vector2f& playerVel, const sf::FloatRect& playerRect, float dt) {
    for (const auto& fp : m_platforms) {
        if (fp.contains(playerRect)) {
            fp.applyForce(playerVel, dt);
            g_discovery.discover(PlatType::MoveV); // Reuse MoveV discovery
        }
    }
}