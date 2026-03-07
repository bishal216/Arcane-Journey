#pragma once
#include <bitset>
#include <cstdint>
#include <string>

// One entry per discoverable platform type.
// Index must match the order in WISDOM table in HubUI.cpp.
enum class PlatType : int {
    Static = 0,
    MoveH,
    MoveV,
    Crumbling,
    Bounce,
    Disappearing,
    OneWay,
    Hatch,
    Rope,
    Ladder,
    Teleport,
    Coin,
    Goal,
    COUNT
};

class DiscoveryTracker {
   public:
    void discover(PlatType t) { m_found.set((int)t); }
    bool isDiscovered(PlatType t) const { return m_found.test((int)t); }

    // Serialise to/from a uint16_t bitmask for SaveData
    uint16_t toBits() const { return (uint16_t)m_found.to_ulong(); }
    void fromBits(uint16_t b) { m_found = std::bitset<(int)PlatType::COUNT>(b); }

    void reset() { m_found.reset(); }

   private:
    std::bitset<(int)PlatType::COUNT> m_found;
};

// Global instance — included by any manager that needs to report discoveries.
// Declared in DiscoveryTracker.cpp, extern here.
extern DiscoveryTracker g_discovery;
