#pragma once
#include "unit.hpp"
#include <vector>
#include <string>

class UnitPool {
private:
    std::vector<Unit> allUnits;

public:
    UnitPool();

    std::vector<Unit> getUnitsByTier(int tier) const;
    Unit getRandomUnitByTier(int tier) const;

    void printAllUnits() const;
};
