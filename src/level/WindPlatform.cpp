#include "level/WindPlatform.hpp"

#include <algorithm>

#include "systems/DiscoveryTracker.hpp"
#include "systems/ArtifactManager.hpp"

WindPlatformManager g_windPlatforms;

void WindPlatformManager::add(float x, float y, float w, float h, int dir, float strength, sf::Color col) {
    m_platforms.emplace_back(x, y, w, h, dir, strength, col);
}

void WindPlatformManager::update(float dt) {
    for (auto& p : m_platforms) p.update(dt);
}

void WindPlatformManager::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_platforms) p.draw(window);
}

void WindPlatformManager::reset() {
    for (auto& p : m_platforms) {
        p.shape.setPosition(p.origin);
        p.pulseTimer = 0.f;
        // Restore base color alpha
        p.shape.setFillColor(p.baseColor);
    }
}

void WindPlatformManager::applyWind(sf::Vector2f& playerVel, const sf::FloatRect& playerRect, float dt) {
    for (const auto& wp : m_platforms) {
        if (wp.contains(playerRect)) {
            wp.applyForce(playerVel, dt);
            g_discovery.discover(PlatType::MoveH); // Reuse MoveH discovery or add new
        }
    }
}