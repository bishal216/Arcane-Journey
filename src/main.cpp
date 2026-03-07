#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include "Constants.hpp"
#include "core/Camera.hpp"
#include "core/Player.hpp"
#include "level/LevelData.hpp"
#include "level/TileLoader.hpp"
#include "systems/AchievementManager.hpp"
#include "systems/ArtifactManager.hpp"
#include "systems/DiscoveryTracker.hpp"
#include "systems/Juice.hpp"
#include "systems/SaveData.hpp"
#include "ui/CoinManager.hpp"
#include "ui/HubUI.hpp"
#include "ui/Npc.hpp"
#include "ui/SectionAnnouncer.hpp"
#include "ui/SplashScreen.hpp"

static const std::string SAVE_PATH = "save.dat";

static constexpr float BODY_W = 22.f;
static constexpr float BODY_H = 32.f;

int main() {
    sf::RenderWindow window(sf::VideoMode({WIN_W, WIN_H}), "Arcane Journey", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("assets/fonts/DejaVuSans.ttf"))
        (void)font.openFromFile("C:/Windows/Fonts/arial.ttf");

    // -----------------------------------------------------------------------
    // Load level
    // -----------------------------------------------------------------------
    LevelData level;
    float worldW = 0.f;
    float worldH = TileLoader::load("assets/levels/level.txt", level, &worldW);

    // -----------------------------------------------------------------------
    // Construct managers
    // -----------------------------------------------------------------------
    Player player;
    Camera camera;
    SectionAnnouncer announcer(font);
    CoinManager coins;
    coins.init(level.coinSpawns());
    NpcManager npcs;
    HubUI hubUI(font, coins);

    player.setWorldH(worldH);
    player.setWorldW(worldW);
    camera.setWorldH(worldH);
    camera.setWorldW(worldW);
    npcs.build(worldH);
    player.reset();

    // -----------------------------------------------------------------------
    // Load save
    // -----------------------------------------------------------------------
    SaveData save;
    if (save.load(SAVE_PATH)) {
        coins.setCollectedCount(save.totalCoins);
        coins.setCoinFraction(save.coinFraction);
        if (save.bestTime > 0.f) hubUI.setBestTime(save.bestTime);
        g_artifacts.fromBits(save.artifactOwned, save.artifactEquipped);
        g_achievements.restoreBits(save.achievCompleted, save.achievClaimed);
        // Restore owned eye cosmetics
        for (int i = 0; i < (int)save.eyeOwned.size(); ++i)
            if (save.eyeOwned[i]) g_artifacts.grantEyeCosmetic(100 + i);
    }

    auto doSave = [&]() {
        save.discoveries = g_discovery.toBits();
        save.totalCoins = coins.collectedCount();
        save.coinFraction = coins.coinFraction();
        save.bestTime = hubUI.bestTime();
        save.artifactOwned = g_artifacts.ownedBits();
        save.artifactEquipped = g_artifacts.equippedBits();
        save.achievCompleted = g_achievements.completedBits();
        save.achievClaimed = g_achievements.claimedBits();
        // Eye cosmetics — 8 slots (ids 100-107)
        save.eyeOwned.assign(8, false);
        save.eyeEquipped.assign(8, false);
        for (const auto& a : g_artifacts.artifacts()) {
            if (a.id >= 100 && a.id <= 107) {
                save.eyeOwned[a.id - 100] = a.owned;
                save.eyeEquipped[a.id - 100] = a.equipped;
            }
        }
        save.save(SAVE_PATH);
    };

    // -----------------------------------------------------------------------
    // Game state
    // -----------------------------------------------------------------------
    announcer.resetSeen((int)level.sections().size());

    float foolThreshold = worldH;
    for (const auto& s : level.sections())
        if (s.name == "I - The Fool") {
            foolThreshold = s.startY;
            break;
        }

    GameState state = GameState::Playing;
    bool inHub = true;
    bool runActive = false;
    float runTimer = 0.f;
    bool timerRunning = false;
    float runEndTimer = 0.f;
    bool dashUsedThisRun = false;  // for Ghost Run achievement

    auto doReset = [&]() {
        doSave();
        player.reset();
        player.resetRun();
        coins.reset();
        coins.setCollectedCount(save.totalCoins);
        coins.setCoinFraction(save.coinFraction);
        announcer.resetSeen((int)level.sections().size());
        state = GameState::Playing;
        inHub = true;
        runActive = false;
        runTimer = 0.f;
        timerRunning = false;
        runEndTimer = 0.f;
        dashUsedThisRun = false;
        hubUI.close();
    };

    // -----------------------------------------------------------------------
    // Section background colors — one per tarot card, keyed by section name
    // -----------------------------------------------------------------------
    // Each color is the "midnight" mood of that card — dark but distinct
    struct BgColor {
        sf::Color top;
        sf::Color bottom;
    };
    auto lerpColor = [](sf::Color a, sf::Color b, float t) -> sf::Color {
        return sf::Color((uint8_t)(a.r + (b.r - a.r) * t), (uint8_t)(a.g + (b.g - a.g) * t),
                         (uint8_t)(a.b + (b.b - a.b) * t));
    };

    // name → flat clear color (we lerp between adjacent sections)
    static const std::pair<std::string, sf::Color> SEC_COLORS[] = {
        {"Training Grounds", sf::Color(18, 8, 38)},           // deep purple — home
        {"I - The Fool", sf::Color(30, 18, 65)},              // indigo — naive wonder
        {"II - The Magician", sf::Color(12, 35, 65)},         // dark teal — arcane
        {"III - The High Priestess", sf::Color(15, 15, 75)},  // midnight blue — mystery
        {"IV - The Empress", sf::Color(18, 50, 18)},          // forest — abundance
        {"V - The Emperor", sf::Color(60, 12, 12)},           // crimson — authority
        {"VI - The Hierophant", sf::Color(35, 22, 60)},       // purple — tradition
        {"VII - The Lovers", sf::Color(65, 15, 45)},          // rose-dark — passion
        {"VIII - The Chariot", sf::Color(12, 18, 65)},        // royal blue — drive
        {"IX - Strength", sf::Color(65, 35, 8)},              // amber — courage
        {"X - The Hermit", sf::Color(22, 22, 40)},            // slate — solitude
        {"XI - Wheel of Fortune", sf::Color(50, 22, 60)},     // purple-gold — fate
        {"XII - Justice", sf::Color(15, 40, 60)},             // steel blue — order
        {"XIII - The Hanged Man", sf::Color(8, 45, 50)},      // teal — surrender
        {"XIV - Death", sf::Color(8, 8, 15)},                 // near-black — transition
        {"XV - Temperance", sf::Color(15, 50, 65)},           // cyan — balance
        {"XVI - The Devil", sf::Color(60, 8, 8)},             // blood red — bondage
        {"XVII - The Tower", sf::Color(55, 28, 8)},           // burnt orange — upheaval
        {"XVIII - The Star", sf::Color(8, 18, 70)},           // cobalt — hope
        {"XIX - The Moon", sf::Color(12, 12, 55)},            // cold navy — illusion
        {"XX - The Sun", sf::Color(70, 55, 8)},               // gold — joy
        {"XXI - Judgement", sf::Color(55, 15, 55)},           // violet — awakening
        {"XXII - The World", sf::Color(15, 60, 40)},          // emerald — completion
    };

    auto getSectionColor = [&](const std::string& name) -> sf::Color {
        for (const auto& p : SEC_COLORS)
            if (p.first == name) return p.second;
        return sf::Color(18, 8, 38);
    };

    sf::Color bgCurrent(18, 8, 38);
    sf::Color bgTarget(18, 8, 38);
    sf::Text hudText(font, "", 20);
    hudText.setFillColor(sf::Color(210, 210, 210));
    hudText.setOutlineColor(sf::Color::Black);
    hudText.setOutlineThickness(1.5f);

    sf::Text promptText(font, "", 22);
    promptText.setOutlineColor(sf::Color::Black);
    promptText.setOutlineThickness(2.f);

    sf::Text winText(font, "YOU REACHED THE TOP!\nPress R to play again", 36);
    winText.setFillColor(sf::Color(255, 215, 0));
    winText.setOutlineColor(sf::Color::Black);
    winText.setOutlineThickness(2.f);

    sf::Text runEndText(font, "", 26);
    runEndText.setOutlineColor(sf::Color::Black);
    runEndText.setOutlineThickness(2.f);

    sf::RectangleShape dashIndicator({16.f, 16.f});
    dashIndicator.setOutlineColor(sf::Color(255, 255, 255, 120));
    dashIndicator.setOutlineThickness(1.f);

    // Chronicler "new achievement" notification badge
    sf::Text achievNotif(font, "! New achievement", 16);
    achievNotif.setFillColor(sf::Color(120, 255, 120));
    achievNotif.setOutlineColor(sf::Color::Black);
    achievNotif.setOutlineThickness(1.5f);

    // -----------------------------------------------------------------------
    // Splash screen
    // -----------------------------------------------------------------------
    {
        SplashScreen splash(font);
        sf::Clock splashClock;
        while (window.isOpen() && !splash.isDone()) {
            float sdt = splashClock.restart().asSeconds();
            if (sdt > 0.05f) sdt = 0.05f;
            while (const std::optional ev = window.pollEvent()) {
                if (ev->is<sf::Event::Closed>()) {
                    window.close();
                    return 0;
                }
                splash.handleInput(*ev);
            }
            splash.update(sdt);
            window.clear();
            splash.draw(window);
            window.display();
        }
    }

    // -----------------------------------------------------------------------
    // Main loop
    // -----------------------------------------------------------------------
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        // -------------------------------------------------------------------
        // Events
        // -------------------------------------------------------------------
        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                doSave();
                window.close();
            }

            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::R) {
                    doReset();
                } else if (hubUI.isOpen()) {
                    hubUI.handleInput(k->code);
                    doSave();
                } else if (k->code == sf::Keyboard::Key::E && inHub) {
                    int zi = npcs.getNearbyNpc(player.position());
                    if (zi >= 0) {
                        switch (npcs.typeAt(zi)) {
                            case NpcType::Shop:
                                hubUI.open(HubPanel::Shop);
                                break;
                            case NpcType::Cosmetics:
                                hubUI.open(HubPanel::Cosmetics);
                                break;
                            case NpcType::TimeKeeper:
                                hubUI.open(HubPanel::TimeKeeper);
                                break;
                            case NpcType::Lore:
                                hubUI.open(HubPanel::Lore);
                                break;
                            case NpcType::WiseMan:
                                hubUI.open(HubPanel::WiseMan);
                                break;
                        }
                    }
                } else if (k->code == sf::Keyboard::Key::Escape) {
                    hubUI.close();
                }
            }
        }

        // -------------------------------------------------------------------
        // Update
        // -------------------------------------------------------------------
        if (!hubUI.isOpen() && state == GameState::Playing) {
            player.update(dt, level.platforms(), state);
            level.update(dt);
            level.resolvePlayer(player, dt, state);
            coins.update(player.position());
            g_artifacts.updateGhost(dt, player.position());

            // Glass Cannon — hard fall detected, force hub return
            if (player.glassCannonTriggered() && runActive) {
                player.clearGlassCannon();
                player.reset();
                player.resetRun();
                if (g_artifacts.mods().loseCoinsOnFail) {
                    int lost = coins.collectedCount() / 2;
                    coins.setCollectedCount(coins.collectedCount() - lost);
                }
                runActive = false;
                timerRunning = false;
                inHub = true;
                runEndTimer = 3.5f;
                level.resetDynamic();
                save.totalCoins = coins.collectedCount();
                save.coinFraction = coins.coinFraction();
                doSave();
                coins.reset();
                coins.setCollectedCount(save.totalCoins);
                coins.setCoinFraction(save.coinFraction);
                runEndText.setString("Glass Cannon! Sent back to hub.");
                auto rb = runEndText.getLocalBounds();
                runEndText.setOrigin(
                    {rb.position.x + rb.size.x / 2.f, rb.position.y + rb.size.y / 2.f});
                runEndText.setPosition({(float)WIN_W / 2.f, (float)WIN_H - 100.f});
            }

            // Track dash usage for Ghost Run achievement
            if (player.hasDashBeenUsed()) dashUsedThisRun = true;

            float py = player.position().y;

            // Leaving hub — start run
            if (inHub && py < foolThreshold) {
                inHub = false;
                runActive = true;
                timerRunning = true;
                runTimer = 0.f;
                dashUsedThisRun = false;
                player.resetRun();
                coins.resetRun();
            }

            // Returning to hub without winning — end run
            if (runActive && py >= foolThreshold && state != GameState::Won) {
                runActive = false;
                timerRunning = false;
                inHub = true;
                runEndTimer = 3.5f;
                level.resetDynamic();
                if (g_artifacts.mods().loseCoinsOnFail) {
                    int lost = coins.collectedCount() / 2;
                    coins.setCollectedCount(coins.collectedCount() - lost);
                }
                save.totalCoins = coins.collectedCount();
                save.coinFraction = coins.coinFraction();
                doSave();
                coins.reset();
                coins.setCollectedCount(save.totalCoins);
                coins.setCoinFraction(save.coinFraction);

                int mins = (int)(runTimer / 60.f);
                float secs = runTimer - mins * 60.f;
                std::ostringstream ss;
                ss << "Run ended — " << mins << ":" << std::fixed << std::setprecision(2)
                   << std::setw(5) << std::setfill('0') << secs;
                runEndText.setString(ss.str());
                auto rb = runEndText.getLocalBounds();
                runEndText.setOrigin(
                    {rb.position.x + rb.size.x / 2.f, rb.position.y + rb.size.y / 2.f});
                runEndText.setPosition({(float)WIN_W / 2.f, (float)WIN_H - 100.f});
            }

            if (timerRunning) runTimer += dt;
            if (runEndTimer > 0.f) runEndTimer -= dt;

            // Countdown timer — Overclock Gauntlet: run ends when it hits 0
            if (timerRunning && runActive && g_artifacts.mods().countdownTimer) {
                float timeLeft = g_artifacts.mods().countdownSeconds - runTimer;
                if (timeLeft <= 0.f) {
                    // Time's up — force hub return
                    player.reset();
                    player.resetRun();
                    if (g_artifacts.mods().loseCoinsOnFail) {
                        int lost = coins.collectedCount() / 2;
                        coins.setCollectedCount(coins.collectedCount() - lost);
                    }
                    runActive = false;
                    timerRunning = false;
                    inHub = true;
                    runEndTimer = 3.5f;
                    level.resetDynamic();
                    save.totalCoins = coins.collectedCount();
                    save.coinFraction = coins.coinFraction();
                    doSave();
                    coins.reset();
                    coins.setCollectedCount(save.totalCoins);
                    coins.setCoinFraction(save.coinFraction);
                    runEndText.setString("Time's up! Sent back to hub.");
                    auto rb = runEndText.getLocalBounds();
                    runEndText.setOrigin(
                        {rb.position.x + rb.size.x / 2.f, rb.position.y + rb.size.y / 2.f});
                    runEndText.setPosition({(float)WIN_W / 2.f, (float)WIN_H - 100.f});
                }
            }

            // Section announcer
            int sec = level.getSectionIndex(py);
            if (sec >= 0 && !announcer.hasSeen(sec)) {
                announcer.markSeen(sec);
                announcer.trigger(level.sections()[sec].name);
            }

            // Win
            if (state == GameState::Won) {
                timerRunning = false;
                runActive = false;
                g_juice.onGoal(player.position());

                // Build run result
                RunResult result;
                result.time = runTimer;
                result.coinsCollected = coins.collectedCount();
                result.totalCoins = coins.totalCoins();
                result.dashUsed = dashUsedThisRun;
                for (const auto& a : g_artifacts.artifacts())
                    if (a.equipped) result.equippedIds.push_back(a.id);

                // Pure best time — only if no gameplay artifacts equipped
                bool anyGameplayArtifact = false;
                for (const auto& a : g_artifacts.artifacts())
                    if (a.equipped && a.id < 100) {
                        anyGameplayArtifact = true;
                        break;
                    }
                if (!anyGameplayArtifact) hubUI.setBestTime(runTimer);

                // Check achievements
                g_achievements.onRunComplete(result);

                doSave();
            }
        }

        announcer.update(dt);
        g_juice.update(dt);
        hubUI.update(dt);
        npcs.update(dt, player.position());
        camera.update(player.position());

        // Smoothly lerp background color toward current section's color
        {
            const auto& secs = level.sections();
            int si = level.getSectionIndex(player.position().y);
            if (si >= 0 && si < (int)secs.size()) {
                sf::Color colCurrent = getSectionColor(secs[si].name);

                // Blend toward the next section above (si-1) as player approaches it
                // secTop = this section's startY (player entered here coming from below)
                // secBottom = previous section's startY (the boundary above us)
                if (si > 0) {
                    float secTop = secs[si].startY;        // lower boundary (larger Y)
                    float secAbove = secs[si - 1].startY;  // upper boundary (smaller Y)
                    float height = secTop - secAbove;
                    if (height > 0.f) {
                        // t=0 when player just entered (near secTop), t=1 when near top of section
                        float t = std::clamp((secTop - player.position().y) / height, 0.f, 1.f);
                        sf::Color colAbove = getSectionColor(secs[si - 1].name);
                        bgTarget = lerpColor(colCurrent, colAbove, t * 0.5f);
                    } else {
                        bgTarget = colCurrent;
                    }
                } else {
                    bgTarget = getSectionColor(secs[si].name);
                }
            }
            // Smooth transition
            float speed = std::min(1.f, dt * 2.f);
            bgCurrent = lerpColor(bgCurrent, bgTarget, speed);
        }

        // -------------------------------------------------------------------
        // Draw — world space
        // -------------------------------------------------------------------
        camera.apply(window);
        {
            sf::View view = window.getView();
            view.move(g_juice.shakeOffset());
            window.setView(view);
        }

        window.clear(bgCurrent);

        level.draw(window);
        npcs.draw(window, font);
        coins.draw(window);

        g_artifacts.drawGhost(window);

        for (const auto& img : g_juice.afterimages()) {
            sf::RectangleShape ghost({BODY_W * img.scaleX, BODY_H * img.scaleY});
            ghost.setOrigin({BODY_W * img.scaleX / 2.f, BODY_H * img.scaleY / 2.f});
            ghost.setPosition(img.pos);
            ghost.setFillColor(
                sf::Color(img.color.r, img.color.g, img.color.b, (uint8_t)(img.alpha * 180.f)));
            window.draw(ghost);
        }

        player.draw(window);
        g_juice.draw(window);

        // -------------------------------------------------------------------
        // Draw — screen space (HUD, UI)
        // -------------------------------------------------------------------
        camera.applyDefault(window);

        g_artifacts.drawBadges(window, font);

        dashIndicator.setFillColor(player.isDashAvail() ? sf::Color(100, 200, 255, 200)
                                                        : sf::Color(60, 40, 100, 180));
        dashIndicator.setPosition({10.f, 36.f});
        window.draw(dashIndicator);

        // HUD text
        float progress = std::clamp(1.f - (player.position().y / worldH), 0.f, 1.f);
        std::ostringstream hud;
        hud << "Height: " << (int)(progress * 100) << "%"
            << "   Coins: " << coins.collectedCount() << "/" << coins.totalCoins();
        if (timerRunning) {
            const auto& mods = g_artifacts.mods();
            if (mods.countdownTimer) {
                float timeLeft = std::max(0.f, mods.countdownSeconds - runTimer);
                int mins = (int)(timeLeft / 60.f);
                float secs = timeLeft - mins * 60.f;
                // Flash red when under 30 seconds
                if (timeLeft < 30.f) {
                    float flash = std::fmod(timeLeft, 0.5f) < 0.25f ? 255.f : 180.f;
                    hudText.setFillColor(sf::Color((uint8_t)flash, 60, 60));
                } else {
                    hudText.setFillColor(sf::Color(210, 210, 210));
                }
                hud << "   Time: " << mins << ":" << std::fixed << std::setprecision(2)
                    << std::setw(5) << std::setfill('0') << secs << " ⬇";
            } else {
                hudText.setFillColor(sf::Color(210, 210, 210));
                int mins = (int)(runTimer / 60.f);
                float secs = runTimer - mins * 60.f;
                hud << "   Time: " << mins << ":" << std::fixed << std::setprecision(2)
                    << std::setw(5) << std::setfill('0') << secs;
            }
        } else {
            hudText.setFillColor(sf::Color(210, 210, 210));
        }
        if (inHub) hud << "   [E] Interact";
        hud << "   [R] Restart";
        hudText.setString(hud.str());
        hudText.setPosition({10.f, 10.f});
        window.draw(hudText);

        // NPC prompt
        if (inHub && !hubUI.isOpen()) {
            int zi = npcs.getNearbyNpc(player.position());
            if (zi >= 0) {
                std::string npcPrompt = "[E] Open " + npcs.nameAt(zi);
                // Add "!" badge if Chronicler has unclaimed achievements
                if (npcs.typeAt(zi) == NpcType::TimeKeeper && g_achievements.hasUnclaimed())
                    npcPrompt += "  ★ New!";
                promptText.setString(npcPrompt);
                promptText.setFillColor(sf::Color(255, 255, 180));
            } else {
                promptText.setString("Jump up to begin your journey");
                promptText.setFillColor(sf::Color(160, 140, 200));
            }
            auto pb = promptText.getLocalBounds();
            promptText.setOrigin({pb.position.x + pb.size.x / 2.f, 0.f});
            promptText.setPosition({(float)WIN_W / 2.f, (float)WIN_H - 60.f});
            window.draw(promptText);
        }

        // Run end banner
        if (runEndTimer > 0.f) {
            uint8_t a = (uint8_t)(std::min(1.f, runEndTimer / 0.5f) * 255.f);
            runEndText.setFillColor(sf::Color(200, 160, 255, a));
            runEndText.setOutlineColor(sf::Color(0, 0, 0, a));
            window.draw(runEndText);
        }

        // Unclaimed achievement toast (bottom-left, fades after win)
        if (state == GameState::Won && g_achievements.hasUnclaimed()) {
            window.draw(achievNotif);
        }

        announcer.draw(window);
        hubUI.draw(window);

        // Win screen
        if (state == GameState::Won) {
            auto b = winText.getLocalBounds();
            winText.setPosition({(float)WIN_W / 2.f - b.size.x / 2.f, (float)WIN_H / 2.f - 40.f});
            window.draw(winText);
        }

        // Flash overlay
        if (g_juice.flashAlpha() > 0.f) {
            sf::RectangleShape flash({(float)WIN_W, (float)WIN_H});
            flash.setFillColor(sf::Color(255, 255, 255, (uint8_t)(g_juice.flashAlpha() * 255.f)));
            window.draw(flash);
        }

        window.display();
    }

    doSave();
    return 0;
}