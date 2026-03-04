#include "systems/SaveData.hpp"
#include <fstream>
#include <cstdint>

// Format v2:
//   [4]  magic "ARCJ"
//   [1]  version = 2
//   [4]  totalCoins (int32)
//   [4]  bestTime   (float)
//   [2]  discoveries (uint16 bitmask)
//   [1]  numCosmetics
//   [n]  unlocked flags
//   [n]  equipped flags

static constexpr char    MAGIC[4] = {'A','R','C','J'};
static constexpr uint8_t VERSION  = 2;

bool SaveData::load(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;

    char magic[4];
    f.read(magic, 4);
    if (f.fail() || magic[0]!='A'||magic[1]!='R'||magic[2]!='C'||magic[3]!='J')
        return false;

    uint8_t ver;
    f.read(reinterpret_cast<char*>(&ver), 1);
    if (ver != VERSION) return false;   // old save — start fresh

    int32_t coins;
    f.read(reinterpret_cast<char*>(&coins), 4);
    totalCoins = coins;

    f.read(reinterpret_cast<char*>(&bestTime), 4);

    f.read(reinterpret_cast<char*>(&discoveries), 2);

    uint8_t n;
    f.read(reinterpret_cast<char*>(&n), 1);
    unlocked.resize(n, false);
    equipped.resize(n, false);
    for (int i = 0; i < n; ++i) { uint8_t v; f.read(reinterpret_cast<char*>(&v),1); unlocked[i]=v!=0; }
    for (int i = 0; i < n; ++i) { uint8_t v; f.read(reinterpret_cast<char*>(&v),1); equipped[i]=v!=0; }

    return !f.fail();
}

void SaveData::save(const std::string& path) const {
    std::ofstream f(path, std::ios::binary);
    if (!f) return;

    f.write(MAGIC, 4);
    f.write(reinterpret_cast<const char*>(&VERSION), 1);

    int32_t coins = totalCoins;
    f.write(reinterpret_cast<const char*>(&coins), 4);
    f.write(reinterpret_cast<const char*>(&bestTime), 4);
    f.write(reinterpret_cast<const char*>(&discoveries), 2);

    uint8_t n = (uint8_t)std::min(unlocked.size(), (size_t)255);
    f.write(reinterpret_cast<const char*>(&n), 1);
    for (int i = 0; i < n; ++i) { uint8_t v = unlocked[i]?1:0; f.write(reinterpret_cast<const char*>(&v),1); }
    for (int i = 0; i < n; ++i) { uint8_t v = equipped[i]?1:0; f.write(reinterpret_cast<const char*>(&v),1); }
}