#include "systems/DiscoveryTracker.hpp"
#include "ui/CoinManager.hpp"
#include <cmath>

static const float COLLECT_RADIUS = 24.f;

void CoinManager::init(const std::vector<CoinSpawn>& spawns) {
    m_coins.clear();
    for (const auto& s : spawns)
        m_coins.emplace_back(s.cx, s.cy);
    m_collectedTotal = 0;
    m_runTotal       = 0;
}

void CoinManager::reset() {
    for (auto& c : m_coins) c.collectedThisRun = false;
    m_collectedTotal = 0;
    m_runTotal       = 0;
}

void CoinManager::resetRun() {
    m_collectedTotal = 0;
    for (auto& c : m_coins)
        if (c.collectedThisRun) m_collectedTotal++;
            g_discovery.discover(PlatType::Coin);
}

void CoinManager::update(sf::Vector2f playerPos) {
    for (auto& c : m_coins) {
        if (c.collectedThisRun) continue;
        sf::Vector2f coinPos = c.shape.getPosition();
        float dx = playerPos.x - coinPos.x;
        float dy = playerPos.y - coinPos.y;
        if (std::sqrt(dx*dx + dy*dy) < COLLECT_RADIUS) {
            c.collectedThisRun = true;
            m_collectedTotal++;
            g_discovery.discover(PlatType::Coin);
            m_runTotal++;
        }
    }
}

void CoinManager::draw(sf::RenderWindow& window) const {
    for (const auto& c : m_coins)
        if (!c.collectedThisRun)
            window.draw(c.shape);
}