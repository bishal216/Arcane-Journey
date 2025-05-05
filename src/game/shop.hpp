#pragma once
#include "unit.hpp"
#include "unitpool.hpp"
#include "player.hpp"
#include <vector>
#include <memory>

class Shop {
private:
    UnitPool& unitPool;           // Link to UnitPool for random units
    std::vector<Unit> availableUnits;  // Units currently in the shop
    int gold;                      // Player's current gold

public:
    Shop(UnitPool& pool, int initialGold);

    // Display current units in the shop
    void displayShop() const;

    // Reroll the shop (costs 1 gold)
    void reroll();

    // Buy a unit from the shop
    bool buyUnit(int index, Player& player);

    // Freeze a unit (keeps it for next round)
    void freezeUnit(int index);

    // Get current gold
    int getGold() const { return gold; }

    // Add/remove gold
    void addGold(int amount);
    void subtractGold(int amount);
};
