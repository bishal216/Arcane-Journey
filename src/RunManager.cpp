#include "RunManager.hpp"

RunManager::RunManager(Player& player, LevelData& level, CoinManager& coins, HubUI& hubUI,
                       SectionAnnouncer& announcer, SaveData& save, const std::string& savePath)
    : m_player(player),
      m_level(level),
      m_coins(coins),
      m_hubUI(hubUI),
      m_announcer(announcer),
      m_save(save),
      m_savePath(savePath) {}

void RunManager::init(float worldH) {
    m_foolThreshold = worldH;  // fallback: bottom of world
    for (const auto& s : m_level.sections())
        if (s.name == "I - The Fool") {
            m_foolThreshold = s.startY;
            break;
        }
}

// ---------------------------------------------------------------------------
// update — call every frame while game is playing and hub UI is closed
// ---------------------------------------------------------------------------
void RunManager::update(float dt, GameState& state) {
    float py = m_player.position().y;

    // -- Glass Cannon --------------------------------------------------------
    if (m_player.glassCannonTriggered() && m_runActive) {
        m_player.clearGlassCannon();
        applyLossPenalty();
        endRun("Glass Cannon! Sent back to hub.");
        m_player.reset();
        m_player.resetRun();
        return;
    }

    // -- Dash tracking -------------------------------------------------------
    if (m_player.hasDashBeenUsed()) m_dashUsedThisRun = true;

    // -- Leaving hub — start run ---------------------------------------------
    if (m_inHub && py < m_foolThreshold) {
        m_inHub = false;
        m_runActive = true;
        m_timerRunning = true;
        m_runTimer = 0.f;
        m_dashUsedThisRun = false;
        m_player.resetRun();
        m_coins.resetRun();
    }

    // -- Natural return to hub (fell back) -----------------------------------
    if (m_runActive && py >= m_foolThreshold && state != GameState::Won) {
        applyLossPenalty();

        std::ostringstream ss;
        int mins = (int)(m_runTimer / 60.f);
        float secs = m_runTimer - mins * 60.f;
        ss << "Run ended — " << mins << ":" << std::fixed << std::setprecision(2) << std::setw(5)
           << std::setfill('0') << secs;
        endRun(ss.str());
        return;
    }

    // -- Timer tick ----------------------------------------------------------
    if (m_timerRunning) m_runTimer += dt;
    if (m_runEndTimer > 0.f) m_runEndTimer -= dt;

    // -- Countdown expiry (Overclock Gauntlet) --------------------------------
    if (m_timerRunning && m_runActive && g_artifacts.mods().countdownTimer) {
        if (g_artifacts.mods().countdownSeconds - m_runTimer <= 0.f) {
            applyLossPenalty();
            endRun("Time's up! Sent back to hub.");
            m_player.reset();
            m_player.resetRun();
            return;
        }
    }

    // -- Section announcer ---------------------------------------------------
    int sec = m_level.getSectionIndex(py);
    if (sec >= 0 && !m_announcer.hasSeen(sec)) {
        m_announcer.markSeen(sec);
        m_announcer.trigger(m_level.sections()[sec].name);
    }

    // -- Win -----------------------------------------------------------------
    if (state == GameState::Won) {
        m_timerRunning = false;
        m_runActive = false;
        g_juice.onGoal(m_player.position());

        RunResult result;
        result.time = m_runTimer;
        result.coinsCollected = m_coins.collectedCount();
        result.totalCoins = m_coins.totalCoins();
        result.dashUsed = m_dashUsedThisRun;
        for (const auto& a : g_artifacts.artifacts())
            if (a.equipped) result.equippedIds.push_back(a.id);

        // Pure best time — only if no gameplay artifacts equipped
        bool anyGameplay = false;
        for (const auto& a : g_artifacts.artifacts())
            if (a.equipped && a.id < 100) {
                anyGameplay = true;
                break;
            }
        if (!anyGameplay) m_hubUI.setBestTime(m_runTimer);

        g_achievements.onRunComplete(result);
        doSave();
    }
}

// ---------------------------------------------------------------------------
// fullReset — R key
// ---------------------------------------------------------------------------
void RunManager::fullReset(GameState& state) {
    doSave();
    m_player.reset();
    m_player.resetRun();
    m_coins.reset();
    restoreCoinsFromSave();
    m_level.resetDynamic();
    m_announcer.resetSeen((int)m_level.sections().size());
    m_hubUI.close();

    state = GameState::Playing;
    m_inHub = true;
    m_runActive = false;
    m_runTimer = 0.f;
    m_timerRunning = false;
    m_runEndTimer = 0.f;
    m_dashUsedThisRun = false;
    m_runEndMessage.clear();
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------
void RunManager::doSave() {
    m_save.discoveries = g_discovery.toBits();
    m_save.totalCoins = m_coins.collectedCount();
    m_save.coinFraction = m_coins.coinFraction();
    m_save.bestTime = m_hubUI.bestTime();
    m_save.artifactOwned = g_artifacts.ownedBits();
    m_save.artifactEquipped = g_artifacts.equippedBits();
    m_save.achievCompleted = g_achievements.completedBits();
    m_save.achievClaimed = g_achievements.claimedBits();
    m_save.eyeOwned.assign(8, false);
    m_save.eyeEquipped.assign(8, false);
    for (const auto& a : g_artifacts.artifacts()) {
        if (a.id >= 100 && a.id <= 107) {
            m_save.eyeOwned[a.id - 100] = a.owned;
            m_save.eyeEquipped[a.id - 100] = a.equipped;
        }
    }
    m_save.save(m_savePath);
}

void RunManager::applyLossPenalty() {
    if (g_artifacts.mods().loseCoinsOnFail) {
        int lost = m_coins.collectedCount() / 2;
        m_coins.setCollectedCount(m_coins.collectedCount() - lost);
    }
}

void RunManager::endRun(const std::string& message) {
    m_runActive = false;
    m_timerRunning = false;
    m_inHub = true;
    m_runEndTimer = 3.5f;
    m_runEndMessage = message;
    m_level.resetDynamic();
    m_save.totalCoins = m_coins.collectedCount();
    m_save.coinFraction = m_coins.coinFraction();
    doSave();
    restoreCoinsFromSave();
}

void RunManager::restoreCoinsFromSave() {
    m_coins.reset();
    m_coins.setCollectedCount(m_save.totalCoins);
    m_coins.setCoinFraction(m_save.coinFraction);
}