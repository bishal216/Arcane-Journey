#pragma once
#include <functional>
#include <string>
#include <vector>

// What run stats we track and pass to the achievement checker
struct RunResult {
    float time = 0.f;
    int coinsCollected = 0;
    int totalCoins = 0;
    bool dashUsed = false;
    // artifact ids that were equipped during the run
    std::vector<int> equippedIds;
};

// Condition types — for display in Chronicler UI
enum class AchievCondition {
    SpecificArtifact,  // complete with artifact X equipped
    ArtifactCombo,     // complete with artifact X AND Y equipped
    UnderTime,         // complete in under N seconds
    NoDash,            // complete without using dash
    AllCoins,          // collect every coin
    DevilsPact,        // complete with Devil's Pact specifically
};

struct Achievement {
    int id;
    std::string name;
    std::string description;  // shown in Chronicler list
    std::string flavour;      // shown when claimed
    AchievCondition condition;

    // Condition parameters
    int artifactId1 = -1;   // for SpecificArtifact / ArtifactCombo / DevilsPact
    int artifactId2 = -1;   // for ArtifactCombo second artifact
    float timeLimit = 0.f;  // for UnderTime

    // Reward — id into the cosmetic artifact list (eyes slot)
    int rewardArtifactId = -1;

    // State
    bool completed = false;  // condition was met at least once
    bool claimed = false;    // reward collected from Chronicler
    bool notified = false;   // "new!" badge shown

    std::function<bool(const RunResult&)> check;
};