#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>

#include "core/Player.hpp"
#include "level/LevelData.hpp"
#include "systems/AchievementManager.hpp"
#include "systems/ArtifactManager.hpp"
#include "systems/Juice.hpp"
#include "systems/SaveData.hpp"
#include "ui/CoinManager.hpp"
#include "ui/HubUI.hpp"
#include "ui/SectionAnnouncer.hpp"

// ---------------------------------------------------------------------------
// RunManager — owns all run-state logic
//   - hub / run transitions
//   - timer (count-up and countdown)
//   - Glass Cannon teleport
//   - loseCoinsOnFail
//   - win detection + achievement submission
//   - run-end banner text
// ---------------------------------------------------------------------------
class RunManager {
   public:
    RunManager(Player& player, LevelData& level, CoinManager& coins, HubUI& hubUI,
               SectionAnnouncer& announcer, SaveData& save, const std::string& savePath);

    // Call once after level is loaded to cache the hub boundary
    void init(float worldH);

    // Main update — call every frame when hub UI is closed and state == Playing
    void update(float dt, GameState& state);

    // Full reset (R key) — saves, resets everything, returns to hub
    void fullReset(GameState& state);

    // Explicit save — call after hub interactions (buy, equip, claim)
    void save() { doSave(); }

    // Accessors for HUD / drawing
    bool inHub() const { return m_inHub; }
    bool runActive() const { return m_runActive; }
    bool timerRunning() const { return m_timerRunning; }
    float runTimer() const { return m_runTimer; }
    float runEndTimer() const { return m_runEndTimer; }
    const std::string& runEndMessage() const { return m_runEndMessage; }

   private:
    // References to shared systems
    Player& m_player;
    LevelData& m_level;
    CoinManager& m_coins;
    HubUI& m_hubUI;
    SectionAnnouncer& m_announcer;
    SaveData& m_save;
    std::string m_savePath;

    // Run state
    bool m_inHub = true;
    bool m_runActive = false;
    bool m_timerRunning = false;
    float m_runTimer = 0.f;
    float m_runEndTimer = 0.f;
    bool m_dashUsedThisRun = false;
    std::string m_runEndMessage;

    float m_foolThreshold = 0.f;

    // Internal helpers
    void doSave();
    void endRun(const std::string& message);  // shared hub-return logic
    void applyLossPenalty();                  // deduct coins if loseCoinsOnFail
    void restoreCoinsFromSave();
    void setRunEndBanner(const std::string& msg);
};