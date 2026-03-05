#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "Constants.hpp"
#include "level/LevelData.hpp"
#include "level/TileLoader.hpp"
#include "core/Player.hpp"
#include "core/Camera.hpp"
#include "ui/SectionAnnouncer.hpp"
#include "ui/CoinManager.hpp"
#include "ui/CosmeticsManager.hpp"
#include "ui/Npc.hpp"
#include "ui/HubUI.hpp"
#include "ui/SplashScreen.hpp"
#include "systems/SaveData.hpp"
#include "systems/DiscoveryTracker.hpp"

static const std::string SAVE_PATH = "save.dat";

int main() {
    sf::RenderWindow window(sf::VideoMode({WIN_W, WIN_H}), "Arcane Journey", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    font.openFromFile("assets/fonts/DejaVuSans.ttf")

    // --- Load level ---
    LevelData level;
    float worldW = 0.f;
    float worldH = TileLoader::load("assets/levels/level.txt", level, &worldW);

    // --- Construct managers ---
    Player           player;
    Camera           camera;
    SectionAnnouncer announcer(font);
    CoinManager      coins;
    coins.init(level.coinSpawns());
    CosmeticsManager cosmetics;
    NpcManager       npcs;
    HubUI            hubUI(font, coins, cosmetics);

    player.setWorldH(worldH);  player.setWorldW(worldW);
    camera.setWorldH(worldH);  camera.setWorldW(worldW);
    npcs.build(worldH);
    player.reset();

    // --- Load save ---
    SaveData save;
    if (save.load(SAVE_PATH)) {
        // Restore persistent coin total
        coins.setCollectedCount(save.totalCoins);

        // Restore discoveries
    g_discovery.fromBits(save.discoveries);

    // Restore best time
        if (save.bestTime > 0.f)
            hubUI.setBestTime(save.bestTime);

        // Restore cosmetics
        auto& items = const_cast<std::vector<Cosmetic>&>(cosmetics.items());
        for (int i = 0; i < (int)items.size() && i < (int)save.unlocked.size(); ++i) {
            items[i].unlocked = save.unlocked[i];
            items[i].equipped = save.equipped[i];
        }
    }

    // Helper: write save to disk
    auto doSave = [&]() {
        save.discoveries = g_discovery.toBits();
        save.totalCoins = coins.collectedCount();
        save.bestTime   = hubUI.bestTime();
        const auto& items = cosmetics.items();
        save.unlocked.resize(items.size());
        save.equipped.resize(items.size());
        for (int i = 0; i < (int)items.size(); ++i) {
            save.unlocked[i] = items[i].unlocked;
            save.equipped[i] = items[i].equipped;
        }
        save.save(SAVE_PATH);
    };

    announcer.resetSeen((int)level.sections().size());

    float foolThreshold = worldH;
    for (const auto& s : level.sections())
        if (s.name == "I - The Fool") { foolThreshold = s.startY; break; }

    GameState state        = GameState::Playing;
    bool      inHub        = true;
    bool      runActive    = false;
    float     runTimer     = 0.f;
    bool      timerRunning = false;
    float     runEndTimer  = 0.f;

    // --- HUD ---
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
    dashIndicator.setOutlineColor(sf::Color::Black);
    dashIndicator.setOutlineThickness(1.f);

    auto doReset = [&]() {
        // Save before resetting so persistent data isn't lost
        doSave();
        player.reset();
        coins.reset();
        // Restore persistent coin total after reset clears it
        coins.setCollectedCount(save.totalCoins);
        level.resetDynamic();
        announcer.resetSeen((int)level.sections().size());
        state        = GameState::Playing;
        inHub        = true;
        runActive    = false;
        runTimer     = 0.f;
        timerRunning = false;
        runEndTimer  = 0.f;
        hubUI.close();
    };

    // -----------------------------------------------------------------------
    // Splash screen — runs in its own mini-loop, fades into the game
    // -----------------------------------------------------------------------
    {
        SplashScreen splash(font);
        sf::Clock splashClock;
        while (window.isOpen() && !splash.isDone()) {
            float sdt = splashClock.restart().asSeconds();
            if (sdt > 0.05f) sdt = 0.05f;
            while (const std::optional ev = window.pollEvent()) {
                if (ev->is<sf::Event::Closed>()) { window.close(); return 0; }
                splash.handleInput(*ev);
            }
            splash.update(sdt);
            window.clear();
            splash.draw(window);
            window.display();
        }
    }

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) { doSave(); window.close(); }
            if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->code == sf::Keyboard::Key::R) {
                    doReset();
                } else if (hubUI.isOpen()) {
                    hubUI.handleInput(k->code);
                    doSave(); // cosmetics/shop changes persist immediately
                } else if (k->code == sf::Keyboard::Key::E && inHub) {
                    int zi = npcs.getNearbyNpc(player.position());
                    if (zi >= 0) {
                        switch (npcs.typeAt(zi)) {
                            case NpcType::Shop:      hubUI.open(HubPanel::Shop);      break;
                            case NpcType::Cosmetics: hubUI.open(HubPanel::Cosmetics); break;
                            case NpcType::TimeKeeper:  hubUI.open(HubPanel::TimeKeeper);  break;
                        case NpcType::Lore:        hubUI.open(HubPanel::Lore);       break;
                        case NpcType::WiseMan:     hubUI.open(HubPanel::WiseMan);    break;
                        }
                    }
                } else if (k->code == sf::Keyboard::Key::Escape) {
                    hubUI.close();
                }
            }
        }

        if (!hubUI.isOpen() && state == GameState::Playing) {
            player.update(dt, level.platforms(), state);
            level.update(dt);
            level.resolvePlayer(player, dt, state);
            coins.update(player.position());

            float py = player.position().y;

            if (inHub && py < foolThreshold) {
                inHub        = false;
                runActive    = true;
                timerRunning = true;
                runTimer     = 0.f;
                coins.resetRun();
            }

            if (runActive && py >= foolThreshold) {
                runActive    = false;
                timerRunning = false;
                inHub        = true;
                runEndTimer  = 3.5f;
                level.resetDynamic();
                doSave(); // save at end of each run

                int mins = (int)(runTimer / 60.f);
                float secs = runTimer - mins * 60.f;
                std::ostringstream ss;
                ss << "Run ended — " << mins << ":"
                   << std::fixed << std::setprecision(2)
                   << std::setw(5) << std::setfill('0') << secs;
                runEndText.setString(ss.str());
                auto rb = runEndText.getLocalBounds();
                runEndText.setOrigin({rb.position.x + rb.size.x / 2.f,
                                      rb.position.y + rb.size.y / 2.f});
                runEndText.setPosition({(float)WIN_W / 2.f, (float)WIN_H - 100.f});
            }

            if (timerRunning) runTimer  += dt;
            if (runEndTimer  > 0.f)      runEndTimer -= dt;

            int sec = level.getSectionIndex(py);
            if (sec >= 0 && !announcer.hasSeen(sec)) {
                announcer.markSeen(sec);
                announcer.trigger(level.sections()[sec].name);
            }

            if (state == GameState::Won) {
                timerRunning = false;
                runActive    = false;
                hubUI.setBestTime(runTimer);
                doSave(); // save on win
            }
        }

        announcer.update(dt);
    hubUI.update(dt);
        camera.update(player.position());

        camera.apply(window);
        window.clear(sf::Color(18, 8, 38));
        level.draw(window);
        npcs.draw(window, font);
        coins.draw(window);
        player.draw(window);

        camera.applyDefault(window);

        float progress = std::clamp(1.f - (player.position().y / worldH), 0.f, 1.f);
        std::ostringstream hud;
        hud << "Height: " << (int)(progress * 100) << "%"
            << "   Coins: " << coins.collectedCount() << "/" << coins.totalCoins();
        if (timerRunning) {
            int mins = (int)(runTimer / 60.f);
            float secs = runTimer - mins * 60.f;
            hud << "   Time: " << mins << ":"
                << std::fixed << std::setprecision(2)
                << std::setw(5) << std::setfill('0') << secs;
        }
        if (inHub) hud << "   [E] Interact";
        hud << "   [R] Restart";
        hudText.setString(hud.str());
        hudText.setPosition({10.f, 10.f});
        window.draw(hudText);

        dashIndicator.setPosition({10.f, 36.f});
        window.draw(dashIndicator);

        if (inHub && !hubUI.isOpen()) {
            int zi = npcs.getNearbyNpc(player.position());
            if (zi >= 0) {
                promptText.setString("[E] Open " + npcs.nameAt(zi));
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

        if (runEndTimer > 0.f) {
            uint8_t a = (uint8_t)(std::min(1.f, runEndTimer / 0.5f) * 255.f);
            runEndText.setFillColor(sf::Color(200, 160, 255, a));
            runEndText.setOutlineColor(sf::Color(0, 0, 0, a));
            window.draw(runEndText);
        }

        announcer.draw(window);
        hubUI.draw(window);

        if (state == GameState::Won) {
            auto b = winText.getLocalBounds();
            winText.setPosition({(float)WIN_W / 2.f - b.size.x / 2.f,
                                 (float)WIN_H / 2.f - 40.f});
            window.draw(winText);
        }

        window.display();
    }
    return 0;
}