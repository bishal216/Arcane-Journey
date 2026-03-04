#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct SaveData {
    int               totalCoins    = 0;
    float             bestTime      = -1.f;
    uint16_t          discoveries   = 0;      // bitmask from DiscoveryTracker
    std::vector<bool> unlocked;
    std::vector<bool> equipped;

    bool load(const std::string& path);
    void save(const std::string& path) const;
};