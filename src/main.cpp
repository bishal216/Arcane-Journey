#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include "Constants.hpp"
#include "RunManager.hpp"
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

    // Load level
    LevelData level;
    float worldW = 0.f;
    float worldH = TileLoader::load("assets/levels/level.txt", level, &worldW);

    // Construct managers
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

    // Load save
    SaveData save;
    if (save.load(SAVE_PATH)) {
        coins.setCollectedCount(save.totalCoins);
        coins.setCoinFraction(save.coinFraction);
        if (save.bestTime > 0.f) hubUI.setBestTime(save.bestTime);
        g_artifacts.fromBits(save.artifactOwned, save.artifactEquipped);
        g_achievements.restoreBits(save.achievCompleted, save.achievClaimed);
        for (int i = 0; i < (int)save.eyeOwned.size(); ++i)
            if (save.eyeOwned[i]) g_artifacts.grantEyeCosmetic(100 + i);
    }

    // Run manager
    announcer.resetSeen((int)level.sections().size());
    RunManager runMgr(player, level, coins, hubUI, announcer, save, SAVE_PATH);
    runMgr.init(worldH);

    GameState state = GameState::Playing;

    // Section background colors
    auto lerpColor = [](sf::Color a, sf::Color b, float t) -> sf::Color {
        return sf::Color((uint8_t)(a.r + (b.r - a.r) * t), (uint8_t)(a.g + (b.g - a.g) * t),
                         (uint8_t)(a.b + (b.b - a.b) * t));
    };

    static const std::pair<std::string, sf::Color> SEC_COLORS[] = {
        {"Training Grounds", sf::Color(18, 8, 38)},
        {"I - The Fool", sf::Color(30, 18, 65)},
        {"II - The Magician", sf::Color(12, 35, 65)},
        {"III - The High Priestess", sf::Color(15, 15, 75)},
        {"IV - The Empress", sf::Color(18, 50, 18)},
        {"V - The Emperor", sf::Color(60, 12, 12)},
        {"VI - The Hierophant", sf::Color(35, 22, 60)},
        {"VII - The Lovers", sf::Color(65, 15, 45)},
        {"VIII - The Chariot", sf::Color(12, 18, 65)},
        {"IX - Strength", sf::Color(65, 35, 8)},
        {"X - The Hermit", sf::Color(22, 22, 40)},
        {"XI - Wheel of Fortune", sf::Color(50, 22, 60)},
        {"XII - Justice", sf::Color(15, 40, 60)},
        {"XIII - The Hanged Man", sf::Color(8, 45, 50)},
        {"XIV - Death", sf::Color(8, 8, 15)},
        {"XV - Temperance", sf::Color(15, 50, 65)},
        {"XVI - The Devil", sf::Color(60, 8, 8)},
        {"XVII - The Tower", sf::Color(55, 28, 8)},
        {"XVIII - The Star", sf::Color(8, 18, 70)},
        {"XIX - The Moon", sf::Color(12, 12, 55)},
        {"XX - The Sun", sf::Color(70, 55, 8)},
        {"XXI - Judgement", sf::Color(55, 15, 55)},
        {"XXII - The World", sf::Color(15, 60, 40)},
    };

    auto getSectionColor = [&](const std::string& name) -> sf::Color {
        for (const auto& p : SEC_COLORS)
            if (p.first == name) return p.second;
        return sf::Color(18, 8, 38);
    };

    sf::Color bgCurrent(18, 8, 38);
    sf::Color bgTarget(18, 8, 38);

    // HUD texts + shapes
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

    sf::Text achievNotif(font, "! New achievement", 16);
    achievNotif.setFillColor(sf::Color(120, 255, 120));
    achievNotif.setOutlineColor(sf::Color::Black);
    achievNotif.setOutlineThickness(1.5f);

    // Splash screen
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

    // Main loop
    sf::Clock clock;
    std::string lastRunEndMsg;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        // Events
        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                runMgr.fullReset(state);
                window.close();
            }

            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::R) {
                    runMgr.fullReset(state);
                } else if (hubUI.isOpen()) {
                    hubUI.handleInput(k->code);
                    runMgr.save();
                } else if (k->code == sf::Keyboard::Key::E && runMgr.inHub()) {
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

        // Update
        if (!hubUI.isOpen() && state == GameState::Playing) {
            player.update(dt, level.platforms(), state);
            level.update(dt);
            level.resolvePlayer(player, dt, state);
            coins.update(player.position());
            g_artifacts.updateGhost(dt, player.position());

            runMgr.update(dt, state);

            // Refresh run-end banner when a new message arrives
            if (runMgr.runEndMessage() != lastRunEndMsg) {
                lastRunEndMsg = runMgr.runEndMessage();
                if (!lastRunEndMsg.empty()) {
                    runEndText.setString(lastRunEndMsg);
                    auto rb = runEndText.getLocalBounds();
                    runEndText.setOrigin(
                        {rb.position.x + rb.size.x / 2.f, rb.position.y + rb.size.y / 2.f});
                    runEndText.setPosition({(float)WIN_W / 2.f, (float)WIN_H - 100.f});
                }
            }
        }

        announcer.update(dt);
        g_juice.update(dt);
        g_artifacts.updateFlashes(dt);
        hubUI.update(dt);
        npcs.update(dt, player.position());
        camera.update(player.position());

        // Background color lerp
        {
            const auto& secs = level.sections();
            int si = level.getSectionIndex(player.position().y);
            if (si >= 0 && si < (int)secs.size()) {
                sf::Color col = getSectionColor(secs[si].name);
                if (si > 0) {
                    float secTop = secs[si].startY;
                    float secAbove = secs[si - 1].startY;
                    float height = secTop - secAbove;
                    if (height > 0.f) {
                        float t = std::clamp((secTop - player.position().y) / height, 0.f, 1.f);
                        bgTarget = lerpColor(col, getSectionColor(secs[si - 1].name), t * 0.5f);
                    } else
                        bgTarget = col;
                } else
                    bgTarget = col;
            }
            bgCurrent = lerpColor(bgCurrent, bgTarget, std::min(1.f, dt * 2.f));
        }

        // Draw — world space
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

        // Draw — screen space
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

        if (runMgr.timerRunning()) {
            const auto& mods = g_artifacts.mods();
            if (mods.countdownTimer) {
                float timeLeft = std::max(0.f, mods.countdownSeconds - runMgr.runTimer());
                int mins = (int)(timeLeft / 60.f);
                float secs = timeLeft - mins * 60.f;
                if (timeLeft < 30.f) {
                    float f = std::fmod(timeLeft, 0.5f) < 0.25f ? 255.f : 180.f;
                    hudText.setFillColor(sf::Color((uint8_t)f, 60, 60));
                } else {
                    hudText.setFillColor(sf::Color(210, 210, 210));
                }
                hud << "   Time: " << mins << ":" << std::fixed << std::setprecision(2)
                    << std::setw(5) << std::setfill('0') << secs << " \u2B07";
            } else {
                hudText.setFillColor(sf::Color(210, 210, 210));
                int mins = (int)(runMgr.runTimer() / 60.f);
                float secs = runMgr.runTimer() - mins * 60.f;
                hud << "   Time: " << mins << ":" << std::fixed << std::setprecision(2)
                    << std::setw(5) << std::setfill('0') << secs;
            }
        } else {
            hudText.setFillColor(sf::Color(210, 210, 210));
        }

        if (runMgr.inHub()) hud << "   [E] Interact";
        hud << "   [R] Restart";
        hudText.setString(hud.str());
        hudText.setPosition({10.f, 10.f});
        window.draw(hudText);

        // NPC prompt
        if (runMgr.inHub() && !hubUI.isOpen()) {
            int zi = npcs.getNearbyNpc(player.position());
            if (zi >= 0) {
                std::string npcPrompt = "[E] Open " + npcs.nameAt(zi);
                if (npcs.typeAt(zi) == NpcType::TimeKeeper && g_achievements.hasUnclaimed())
                    npcPrompt += "  \u2605 New!";
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
        if (runMgr.runEndTimer() > 0.f) {
            uint8_t a = (uint8_t)(std::min(1.f, runMgr.runEndTimer() / 0.5f) * 255.f);
            runEndText.setFillColor(sf::Color(200, 160, 255, a));
            runEndText.setOutlineColor(sf::Color(0, 0, 0, a));
            window.draw(runEndText);
        }

        if (state == GameState::Won && g_achievements.hasUnclaimed()) window.draw(achievNotif);

        announcer.draw(window);
        hubUI.draw(window);

        if (state == GameState::Won) {
            auto b = winText.getLocalBounds();
            winText.setPosition({(float)WIN_W / 2.f - b.size.x / 2.f, (float)WIN_H / 2.f - 40.f});
            window.draw(winText);
        }

        if (g_juice.flashAlpha() > 0.f) {
            sf::RectangleShape flash({(float)WIN_W, (float)WIN_H});
            flash.setFillColor(sf::Color(255, 255, 255, (uint8_t)(g_juice.flashAlpha() * 255.f)));
            window.draw(flash);
        }

        window.display();
    }

    runMgr.fullReset(state);
    return 0;
}