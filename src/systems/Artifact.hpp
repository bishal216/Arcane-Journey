#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ---------------------------------------------------------------------------
// Cosmetic slot — what the artifact renders on the player
// ---------------------------------------------------------------------------
enum class CosmeticSlot { None, Hat, Boots, Cape, Aura, Eyes, Wings };

// ---------------------------------------------------------------------------
// Quirk effect carried by this artifact
// ---------------------------------------------------------------------------
struct ArtifactMod {
    float jumpMult = 1.f;
    float fallMult = 1.f;
    float speedMult = 1.f;
    float dashSpeedMult = 1.f;
    float dashCooldownMult = 1.f;
    float dashDurationMult = 1.f;
    float gravityMult = 1.f;
    float frictionMult = 1.f;
    float wallJumpMult = 1.f;
    float climbSpeedMult = 1.f;
    float bounceMult = 1.f;
    float crumbleSpeedMult = 1.f;
    float platformSpeedMult = 1.f;
    float disappearMult = 1.f;
    float coinMult = 1.f;
    float coinMagnetRadius = 0.f;
    float sizeScale = 1.f;
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
};

// ---------------------------------------------------------------------------
// Artifact definition
// ---------------------------------------------------------------------------
struct Artifact {
    int id;
    std::string name;
    std::string description;
    std::string quirkyFlavour;  // short flavour text shown in Tailor
    int price;
    sf::Color color;
    CosmeticSlot slot;
    ArtifactMod mod;

    // Runtime state
    bool owned = false;
    bool equipped = false;
};