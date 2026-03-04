#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "ui/Coin.hpp"
#include "core/World.hpp"  // for CoinSpawn

class CoinManager {
public:
    CoinManager() = default;

    void init(const std::vector<CoinSpawn>& spawns);  // call once after TileLoader

    void reset();
    void resetRun();

    void update(sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window) const;

    int  totalCoins()     const { return (int)m_coins.size(); }
    int  collectedCount() const { return m_collectedTotal; }
    int  runTotal()       const { return m_runTotal; }
    void setCollectedCount(int v) { m_collectedTotal = v; }

private:
    std::vector<Coin> m_coins;
    int m_collectedTotal = 0;
    int m_runTotal       = 0;
};