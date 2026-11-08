#include "systems/SaveData.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <fstream>

// Format v5:
//   [4]  magic "ARCJ"
//   [1]  version = 5
//   [4]  totalCoins (int32)
//   [4]  coinFraction (float)
//   [4]  bestTime   (float)
//   [2]  discoveries (uint16 bitmask)
//   [1]  numArtifacts
//   [n]  artifactOwned flags
//   [n]  artifactEquipped flags
//   [1]  numAchievements
//   [n]  achievCompleted flags
//   [n]  achievClaimed flags
//   [1]  numEyes
//   [n]  eyeOwned flags
//   [n]  eyeEquipped flags

static constexpr char MAGIC[4] = {'A', 'R', 'C', 'J'};
static constexpr uint8_t VERSION = 5;

bool SaveData::load(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;

    char magic[4];
    f.read(magic, 4);
    if (f.fail() || magic[0] != 'A' || magic[1] != 'R' || magic[2] != 'C' || magic[3] != 'J')
        return false;

    uint8_t ver;
    f.read(reinterpret_cast<char*>(&ver), 1);
    if (ver != VERSION) return false;

    // Read into locals first so a truncated/corrupt file leaves this object untouched
    int32_t coins = 0;
    float frac = 0.f, best = -1.f;
    uint16_t disc = 0;
    f.read(reinterpret_cast<char*>(&coins), 4);
    f.read(reinterpret_cast<char*>(&frac), 4);
    f.read(reinterpret_cast<char*>(&best), 4);
    f.read(reinterpret_cast<char*>(&disc), 2);
    if (f.fail()) return false;

    auto readBoolVec = [&](std::vector<bool>& v) {
        uint8_t n = 0;
        f.read(reinterpret_cast<char*>(&n), 1);
        v.assign(n, false);
        for (int i = 0; i < n && !f.fail(); ++i) {
            uint8_t b = 0;
            f.read(reinterpret_cast<char*>(&b), 1);
            v[i] = b != 0;
        }
    };

    SaveData tmp;
    readBoolVec(tmp.artifactOwned);
    readBoolVec(tmp.artifactEquipped);
    readBoolVec(tmp.achievCompleted);
    readBoolVec(tmp.achievClaimed);
    readBoolVec(tmp.eyeOwned);
    readBoolVec(tmp.eyeEquipped);
    if (f.fail()) return false;

    totalCoins = coins;
    coinFraction = frac;
    bestTime = best;
    discoveries = disc;
    artifactOwned = std::move(tmp.artifactOwned);
    artifactEquipped = std::move(tmp.artifactEquipped);
    achievCompleted = std::move(tmp.achievCompleted);
    achievClaimed = std::move(tmp.achievClaimed);
    eyeOwned = std::move(tmp.eyeOwned);
    eyeEquipped = std::move(tmp.eyeEquipped);
    return true;
}

void SaveData::save(const std::string& path) const {
    // Write to a temp file then rename, so a crash mid-write can't corrupt the save
    const std::string tmpPath = path + ".tmp";
    std::ofstream f(tmpPath, std::ios::binary);
    if (!f) return;

    f.write(MAGIC, 4);
    f.write(reinterpret_cast<const char*>(&VERSION), 1);

    int32_t coins = totalCoins;
    f.write(reinterpret_cast<const char*>(&coins), 4);
    f.write(reinterpret_cast<const char*>(&coinFraction), 4);
    f.write(reinterpret_cast<const char*>(&bestTime), 4);
    f.write(reinterpret_cast<const char*>(&discoveries), 2);

    auto writeBoolVec = [&](const std::vector<bool>& v) {
        uint8_t n = (uint8_t)std::min(v.size(), (size_t)255);
        f.write(reinterpret_cast<const char*>(&n), 1);
        for (int i = 0; i < n; ++i) {
            uint8_t b = v[i] ? 1 : 0;
            f.write(reinterpret_cast<const char*>(&b), 1);
        }
    };

    writeBoolVec(artifactOwned);
    writeBoolVec(artifactEquipped);
    writeBoolVec(achievCompleted);
    writeBoolVec(achievClaimed);
    writeBoolVec(eyeOwned);
    writeBoolVec(eyeEquipped);

    f.close();
    if (f.fail()) {
        std::remove(tmpPath.c_str());
        return;
    }
    std::remove(path.c_str());  // rename() won't overwrite on Windows
    std::rename(tmpPath.c_str(), path.c_str());
}