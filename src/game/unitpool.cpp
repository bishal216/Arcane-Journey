#include "unitpool.hpp"
#include <random>
#include <ctime>
#include <iostream>

UnitPool::UnitPool() {
    // Tier 1 units
    allUnits.emplace_back("Ant", 2, 1, 1, TriggerType::OnFaint, "Give random ally +2/+1");
    allUnits.emplace_back("Fish", 2, 3, 1, TriggerType::None, "All allies get +1/+1 on level up");
    allUnits.emplace_back("Otter", 1, 2, 1, TriggerType::None, "Give random ally +1/+1 when bought");

    // Later: Add Tier 2, Tier 3, etc.
}

std::vector<Unit> UnitPool::getUnitsByTier(int tier) const {
    std::vector<Unit> result;
    for (const auto& unit : allUnits) {
        if (unit.getTier() == tier)
            result.push_back(unit);
    }
    return result;
}

Unit UnitPool::getRandomUnitByTier(int tier) const {
    std::vector<Unit> tierUnits = getUnitsByTier(tier);
    if (tierUnits.empty()) {
        throw std::runtime_error("No units available in this tier.");
    }

    static std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
    std::uniform_int_distribution<> dist(0, tierUnits.size() - 1);
    return tierUnits[dist(rng)];
}

void UnitPool::printAllUnits() const {
    for (const auto& unit : allUnits)
        unit.printInfo();
}
