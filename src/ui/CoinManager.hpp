#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "core/World.hpp"  // for CoinSpawn
#include "ui/Coin.hpp"

class CoinManager {
   public:
    CoinManager() = default;

    void init(const std::vector<CoinSpawn>& spawns);

    void reset();
    void resetRun();

    void update(sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window) const;

    int totalCoins() const { return (int)m_coins.size(); }
    int collectedCount() const { return m_collectedTotal; }
    int runTotal() const { return m_runTotal; }
    float coinFraction() const { return m_coinFraction; }
    void setCollectedCount(int v) { m_collectedTotal = v; }
    void setCoinFraction(float v) { m_coinFraction = v; }

   private:
    std::vector<Coin> m_coins;
    int m_collectedTotal = 0;
    int m_runTotal = 0;
    float m_coinFraction = 0.f;  // tracks fractional coins from coinMult
};