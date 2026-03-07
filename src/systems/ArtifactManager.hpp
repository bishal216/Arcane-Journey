#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

#include "systems/Artifact.hpp"

// Merged QuirkMods — built from up to 3 equipped artifacts
// For numeric mods: take the highest (most impactful) value
// For bools: OR them together
struct ActiveMods {
    float jumpMult = 1.f;
    float fallMult = 1.f;
    float speedMult = 1.f;
    float dashSpeedMult = 1.f;
    float gravityMult = 1.f;
    float frictionMult = 1.f;
    float crumbleSpeedMult = 1.f;
    float platformSpeedMult = 1.f;
    float disappearMult = 1.f;
    float coinMult = 1.f;
    float coinMagnetRadius = 0.f;
    float sizeScale = 1.f;
    float climbSpeedMult = 1.f;    // rope/ladder climb speed
    float dashCooldownMult = 1.f;  // <1 = faster cooldown, >1 = slower
    float dashDurationMult = 1.f;  // how long dash lasts
    float wallJumpMult = 1.f;      // wall jump X and Y force
    float bounceMult = 1.f;        // bounce landing velocity (Bouncy Boots)
    bool dashEnabled = true;
    bool doubleJump = true;
    bool wallJump = true;
    bool inputMirrored = false;
    bool glassCannon = false;
    bool bouncy = false;
    bool haunted = false;
    bool coinsSpawn = true;
    bool loseCoinsOnFail = false;
    bool countdownTimer = false;
    float countdownSeconds = 600.f;

    // Equipped cosmetic slots (artifact id or -1)
    int hatArtifact = -1;
    int bootsArtifact = -1;
    int capeArtifact = -1;
    int auraArtifact = -1;
    int eyesArtifact = -1;
    int wingsArtifact = -1;
};

class ArtifactManager {
   public:
    ArtifactManager();

    // --- Data ---
    std::vector<Artifact>& artifacts() { return m_artifacts; }
    const std::vector<Artifact>& artifacts() const { return m_artifacts; }
    const ActiveMods& mods() const { return m_mods; }

    // --- Shop (Merchant) ---
    bool canAfford(int id, int coins) const;
    bool buy(int id, int& coins);  // returns true on success

    // --- Loadout (Tailor) ---

    bool equip(int id);
    void unequip(int id);
    void grantEyeCosmetic(int id);  // grant eye slot cosmetic (achievement reward)
    int equippedCount() const;

    // --- Rebuild mods from equipped artifacts ---
    void rebuildMods();

    // --- Serialisation helpers ---
    std::vector<bool> ownedBits() const;
    std::vector<bool> equippedBits() const;
    void fromBits(const std::vector<bool>& owned, const std::vector<bool>& equipped);

    // --- Draw panels ---
    // Merchant panel — full shop grid
    void drawShop(sf::RenderWindow& window, const sf::Font& font, int coins, int& cursor) const;
    // Tailor panel — equipped loadout + owned collection
    void drawTailor(sf::RenderWindow& window, const sf::Font& font, int& cursor) const;

    // --- Draw player cosmetics (called from Player::draw) ---
    void drawCosmetics(sf::RenderWindow& window, sf::Vector2f playerPos, float scaleX, float scaleY,
                       bool facingLeft) const;

    // --- Haunted ghost ---
    void updateGhost(float dt, sf::Vector2f playerPos);
    void drawGhost(sf::RenderWindow& window) const;
    void drawBadges(sf::RenderWindow& window, const sf::Font& font) const;

    // --- Feedback flash on equip/unequip ---
    struct EquipFlash {
        int id;
        float timer;    // counts down from 1.0
        bool wasEquip;  // true = equipped, false = unequipped
    };
    void updateFlashes(float dt);  // call every frame from main

   private:
    std::vector<Artifact> m_artifacts;
    ActiveMods m_mods;
    std::vector<EquipFlash> m_equipFlashes;

    // Haunted ghost
    struct GhostFrame {
        sf::Vector2f pos;
        float timer;
    };
    std::vector<GhostFrame> m_ghostHistory;
    sf::Vector2f m_ghostPos = {-9999.f, -9999.f};
    static constexpr float GHOST_DELAY = 1.f;

    void buildArtifactList();
    void drawCard(sf::RenderWindow& window, const sf::Font& font, const Artifact& a,
                  sf::Vector2f pos, bool hovered, bool showPrice, int coins) const;

    // Take highest of two multiplier values (further from 1.0 wins)
    static float highestMult(float a, float b);
};

extern ArtifactManager g_artifacts;