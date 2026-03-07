#include "ui/CoinManager.hpp"

#include <cmath>

#include "systems/ArtifactManager.hpp"
#include "systems/DiscoveryTracker.hpp"
#include "systems/Juice.hpp"

static const float COLLECT_RADIUS = 24.f;

void CoinManager::init(const std::vector<CoinSpawn>& spawns) {
    m_coins.clear();
    for (const auto& s : spawns) m_coins.emplace_back(s.cx, s.cy);
    m_collectedTotal = 0;
    m_runTotal = 0;
}

void CoinManager::reset() {
    for (auto& c : m_coins) c.collectedThisRun = false;
    m_collectedTotal = 0;
    m_runTotal = 0;
    m_coinFraction = 0.f;
}

void CoinManager::resetRun() {
    // m_collectedTotal is authoritative — set by main from save.totalCoins
    // Just clear the per-run counter and fraction accumulator
    m_runTotal = 0;
    m_coinFraction = 0.f;
    g_discovery.discover(PlatType::Coin);
}

void CoinManager::update(sf::Vector2f playerPos) {
    const auto& qm = g_artifacts.mods();

    // Coin Magnet — use larger radius if artifact equipped
    float radius = std::max(COLLECT_RADIUS, qm.coinMagnetRadius);

    for (auto& c : m_coins) {
        if (c.collectedThisRun) continue;
        sf::Vector2f coinPos = c.shape.getPosition();
        float dx = playerPos.x - coinPos.x;
        float dy = playerPos.y - coinPos.y;
        if (std::sqrt(dx * dx + dy * dy) < radius) {
            c.collectedThisRun = true;
            // coinMult — add fractional coins (tracked as float, floored for display)
            m_collectedTotal += (int)qm.coinMult;
            m_coinFraction += qm.coinMult - (int)qm.coinMult;
            if (m_coinFraction >= 1.f) {
                m_collectedTotal++;
                m_coinFraction -= 1.f;
            }
            g_discovery.discover(PlatType::Coin);
            g_juice.onCoinCollect(coinPos);
            m_runTotal++;
        }
    }
}

void CoinManager::draw(sf::RenderWindow& window) const {
    for (const auto& c : m_coins)
        if (!c.collectedThisRun) window.draw(c.shape);
}