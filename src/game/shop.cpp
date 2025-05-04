#include "shop.hpp"
#include <iostream>

Shop::Shop(UnitPool& pool, int initialGold) : unitPool(pool), gold(initialGold) {
    reroll(); // Start with a random set of units
}

void Shop::displayShop() const {
    std::cout << "Shop: Gold = " << gold << "\n";
    std::cout << "Available Units:\n";
    for (int i = 0; i < availableUnits.size(); ++i) {
        const auto& unit = availableUnits[i];
        std::cout << i + 1 << ". " << unit.getName() << " - "
                  << "Attack: " << unit.getAttack() << ", Health: " << unit.getHealth()
                  << " - " << unit.getAbilityDescription() << "\n";
    }
}

void Shop::reroll() {
    availableUnits.clear();
    // Populate the shop with random units from tier 1, for example
    for (int i = 0; i < 5; ++i) {
        availableUnits.push_back(unitPool.getRandomUnitByTier(1)); // Random unit of tier 1
    }
}

bool Shop::buyUnit(int index, Player& player) {
    if (index < 1 || index > availableUnits.size()) {
        std::cout << "Invalid unit selection!\n";
        return false;
    }

    const auto& unit = availableUnits[index - 1];

    // Check if player has enough gold
    if (gold >= 3) { // Example cost for each unit
        subtractGold(3);
        if (player.addUnit(unit)) {
            std::cout << "Bought " << unit.getName() << " and added to your team!\n";
        }
        return true;
    } else {
        std::cout << "Not enough gold to buy " << unit.getName() << "!\n";
        return false;
    }
}


void Shop::freezeUnit(int index) {
    // In a real game, we'd handle freezing logic here
    std::cout << "Frozen " << availableUnits[index - 1].getName() << " for next round!\n";
}

void Shop::addGold(int amount) {
    gold += amount;
}

void Shop::subtractGold(int amount) {
    if (gold >= amount) {
        gold -= amount;
    } else {
        std::cout << "Not enough gold!\n";
    }
}
