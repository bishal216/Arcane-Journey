#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct SaveData {
    int32_t totalCoins = 0;
    float bestTime = -1.f;  // pure (no artifacts) best
    uint16_t discoveries = 0;
    std::vector<bool> artifactOwned;
    std::vector<bool> artifactEquipped;
    std::vector<bool> achievCompleted;
    std::vector<bool> achievClaimed;
    // Eye cosmetic artifact owned flags (ids 100-107)
    std::vector<bool> eyeOwned;
    std::vector<bool> eyeEquipped;

    bool load(const std::string& path);
    void save(const std::string& path) const;
};