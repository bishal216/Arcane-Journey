#include "ui/HubUI.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "Constants.hpp"
#include "systems/AchievementManager.hpp"
#include "systems/ArtifactManager.hpp"
#include "systems/DiscoveryTracker.hpp"
#include "ui/PlatformDemo.hpp"

// ---------------------------------------------------------------------------
// Static content
// ---------------------------------------------------------------------------

const std::vector<std::pair<std::string, std::string>> HubUI::LORE = {
    {"Training Grounds", "A place of gentle beginnings. The world above waits."},
    {"I - The Fool", "Step forward with nothing but courage. The journey starts."},
    {"II - The Magician", "All tools are yours. Learn to wield them with intent."},
    {"III - The High Priestess", "Patience reveals what haste conceals. Move deliberately."},
    {"IV - The Empress", "Abundance flows here. Rest, breathe, gather strength."},
    {"V - The Emperor", "Order and structure. Every platform placed with purpose."},
    {"VI - The Hierophant", "Follow the ancient path upward, step by step."},
    {"VII - The Lovers", "Two paths diverge. Both lead onward. Choose wisely."},
    {"VIII - The Chariot", "Speed and will. Hesitation is the only enemy here."},
    {"IX - Strength", "Endurance, not force. Ten small steps beat one great leap."},
    {"X - The Hermit", "Solitude and vast silence. Each platform an island."},
    {"XI - Wheel of Fortune", "Chaos reigns. Adapt or fall. The wheel turns for all."},
    {"XII - Justice", "Balance in all things. The world is symmetrical here."},
    {"XIII - The Hanged Man", "Surrender to the fall. A great drop precedes the climb."},
    {"XIV - Death", "Brutal and honest. Precision is survival."},
    {"XV - Temperance", "Flow like water. Ropes and ladders are your friends."},
    {"XVI - The Devil", "Deception lurks. Gaps are wider than they appear."},
    {"XVII - The Tower", "Wall jump or be swallowed. The tower demands mastery."},
    {"XVIII - The Star", "Hope after ruin. Wide platforms, generous spacing."},
    {"XIX - The Moon", "Nothing is as it seems. Trust your instincts."},
    {"XX - The Sun", "Joy and relief. A warm breather before the end."},
    {"XXI - Judgement", "All skills tested one final time. Rise to the call."},
    {"XXII - The World", "Completion. The top is within reach. Ascend."},
};

const std::vector<std::pair<std::string, std::string>> HubUI::WISDOM = {
    {"Static Platform  [=]", "Solid ground. Won't move, won't break. The bedrock of the world."},
    {"Moving Horiz.    [~]", "Rides left and right. Stand on it and it carries you — use that."},
    {"Moving Vert.     [^]", "Rises and falls. Time your jump for when it's on the way up."},
    {"Crumbling        [@]", "Shakes when you land, then crumbles. Don't linger."},
    {"Bounce Pad       [*]", "Launches you upward on contact. Aim before you land."},
    {"Disappearing     [?]", "Fades in and out on a timer. Watch the rhythm before committing."},
    {"One-Way          [-]", "Pass through from below, solid from above. A shortcut if you know."},
    {"Hatch            [H]", "Swings open when you land. Step off quickly or fall through."},
    {"Rope             [R]", "Hold Up/Down near it to grab and climb. Jump to dismount."},
    {"Ladder           [L]", "Same as rope but sturdier. Up/Down to climb, jump to leave."},
    {"Teleport         [T]",
     "Stand still for half a second and you'll vanish — reappear elsewhere."},
    {"Coin             [C]", "Collect these. Spend them on artifacts at the Merchant."},
    {"Goal             [G]", "The golden platform at the very top. Reach it to win the run."},
};

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

HubUI::HubUI(const sf::Font& font, CoinManager& coins) : m_font(font), m_coins(coins) {}

// ---------------------------------------------------------------------------
// Open / close
// ---------------------------------------------------------------------------

void HubUI::open(HubPanel panel) {
    m_prevCursor = -1;
    m_panel = panel;
    m_cursor = 0;
}

void HubUI::close() { m_panel = HubPanel::None; }

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------

void HubUI::handleInput(sf::Keyboard::Key key) {
    if (m_panel == HubPanel::None) return;

    // Determine item count for this panel
    int itemCount = 0;
    if (m_panel == HubPanel::Shop) {
        itemCount = (int)g_artifacts.artifacts().size();
    } else if (m_panel == HubPanel::Cosmetics) {
        for (const auto& a : g_artifacts.artifacts())
            if (a.owned) ++itemCount;
    } else if (m_panel == HubPanel::Lore) {
        itemCount = (int)LORE.size();
    } else if (m_panel == HubPanel::WiseMan) {
        itemCount = (int)WISDOM.size();
    } else if (m_panel == HubPanel::TimeKeeper) {
        itemCount = (int)g_achievements.achievements().size();
    }

    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W) {
        if (itemCount > 0) m_cursor = (m_cursor - 1 + itemCount) % itemCount;
    } else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S) {
        if (itemCount > 0) m_cursor = (m_cursor + 1) % itemCount;
    } else if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::E) {
        if (m_panel == HubPanel::Shop) {
            int coins = m_coins.collectedCount();
            if (g_artifacts.buy(m_cursor, coins)) m_coins.setCollectedCount(coins);
        } else if (m_panel == HubPanel::Cosmetics) {
            std::vector<int> ownedIds;
            for (const auto& a : g_artifacts.artifacts())
                if (a.owned) ownedIds.push_back(a.id);
            if (m_cursor < (int)ownedIds.size()) {
                int id = ownedIds[m_cursor];
                const auto& arts = g_artifacts.artifacts();
                bool eq = arts[id].equipped;
                if (eq)
                    g_artifacts.unequip(id);
                else
                    g_artifacts.equip(id);
            }
        } else if (m_panel == HubPanel::TimeKeeper) {
            // Claim reward from selected achievement
            const auto& achievs = g_achievements.achievements();
            if (m_cursor < (int)achievs.size()) {
                const Achievement& a = achievs[m_cursor];
                if (a.completed && !a.claimed) {
                    int rewardId = g_achievements.claimNext();
                    if (rewardId >= 100) g_artifacts.grantEyeCosmetic(rewardId);
                }
            }
        } else if (m_panel == HubPanel::WiseMan) {
            if (g_discovery.isDiscovered((PlatType)m_cursor)) m_demo.play((PlatType)m_cursor);
        }
    } else if (key == sf::Keyboard::Key::Escape) {
        close();
    }
}

// ---------------------------------------------------------------------------
// Update
// ---------------------------------------------------------------------------

void HubUI::update(float dt) {
    if (m_panel == HubPanel::WiseMan) {
        m_demo.update(dt);
        if (m_cursor != m_prevCursor) {
            m_prevCursor = m_cursor;
            if (g_discovery.isDiscovered((PlatType)m_cursor)) m_demo.play((PlatType)m_cursor);
        }
    }
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

sf::Text HubUI::makeText(const std::string& str, unsigned size, sf::Color col) {
    sf::Text t(m_font, str, size);
    t.setFillColor(col);
    t.setOutlineColor(sf::Color::Black);
    t.setOutlineThickness(1.5f);
    return t;
}

void HubUI::drawBackground(sf::RenderWindow& w, const std::string& title) {
    sf::RectangleShape overlay({(float)WIN_W, (float)WIN_H});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    w.draw(overlay);

    sf::RectangleShape panel({620.f, 440.f});
    panel.setOrigin({310.f, 220.f});
    panel.setPosition({WIN_W / 2.f, WIN_H / 2.f});
    panel.setFillColor(sf::Color(30, 20, 50, 240));
    panel.setOutlineColor(sf::Color(120, 80, 200));
    panel.setOutlineThickness(2.f);
    w.draw(panel);

    auto t = makeText(title, 26, sf::Color(255, 215, 0));
    auto b = t.getLocalBounds();
    t.setOrigin({b.position.x + b.size.x / 2.f, 0.f});
    t.setPosition({WIN_W / 2.f, WIN_H / 2.f - 210.f});
    w.draw(t);

    auto hint = makeText("[Esc] Close   [W/S] Scroll   [E] Select", 13, sf::Color(130, 130, 150));
    auto hb = hint.getLocalBounds();
    hint.setOrigin({hb.position.x + hb.size.x / 2.f, 0.f});
    hint.setPosition({WIN_W / 2.f, WIN_H / 2.f + 195.f});
    w.draw(hint);
}

// ---------------------------------------------------------------------------
// Draw: Shop (Merchant — buy artifacts)
// ---------------------------------------------------------------------------

void HubUI::drawShop(sf::RenderWindow& w) {
    g_artifacts.drawShop(w, m_font, m_coins.collectedCount(), m_cursor);
}

// ---------------------------------------------------------------------------
// Draw: Tailor (equip up to 3 owned artifacts)
// ---------------------------------------------------------------------------

void HubUI::drawTailor(sf::RenderWindow& w) { g_artifacts.drawTailor(w, m_font, m_cursor); }

// ---------------------------------------------------------------------------
// Draw: TimeKeeper
// ---------------------------------------------------------------------------

void HubUI::drawTimeKeeper(sf::RenderWindow& w) {
    g_achievements.drawChronicler(w, m_font, m_bestTime, m_cursor);
}

// ---------------------------------------------------------------------------
// Draw: Lore (Seer)
// ---------------------------------------------------------------------------

void HubUI::drawLore(sf::RenderWindow& w) {
    drawBackground(w, "Seer  —  Arcane Lore");

    float listX = WIN_W / 2.f - 290.f;
    float startY = WIN_H / 2.f - 175.f;
    int visible = 10;
    int start = std::max(0, m_cursor - visible / 2);

    for (int i = start; i < std::min(start + visible, (int)LORE.size()); ++i) {
        bool sel = (i == m_cursor);
        sf::Color col = sel ? sf::Color(255, 215, 0) : sf::Color(180, 170, 200);
        auto entry = makeText(LORE[i].first, 17, col);
        entry.setPosition({listX, startY + (i - start) * 26.f});
        w.draw(entry);
    }

    if (m_cursor < (int)LORE.size()) {
        auto body = makeText(LORE[m_cursor].second, 16, sf::Color(210, 200, 230));
        body.setPosition({listX, WIN_H / 2.f + 110.f});
        w.draw(body);
    }
}

// ---------------------------------------------------------------------------
// Draw: WiseMan (Sage — platform guide)
// ---------------------------------------------------------------------------

void HubUI::drawWiseMan(sf::RenderWindow& w) {
    drawBackground(w, "Sage  —  Platform Guide");

    float listX = WIN_W / 2.f - 295.f;
    float listY = WIN_H / 2.f - 175.f;
    int visible = 11;
    int start = std::max(0, m_cursor - visible / 2);

    for (int i = start; i < std::min(start + visible, (int)WISDOM.size()); ++i) {
        bool sel = (i == m_cursor);
        bool known = g_discovery.isDiscovered((PlatType)i);

        sf::Color col;
        if (!known)
            col = sf::Color(70, 70, 80);
        else if (sel)
            col = sf::Color(100, 240, 140);
        else
            col = sf::Color(160, 190, 170);

        std::string label = known ? WISDOM[i].first : "???  [undiscovered]";
        auto entry = makeText(label, 16, col);
        entry.setPosition({listX, listY + (i - start) * 28.f});
        w.draw(entry);

        if (sel) {
            auto arrow = makeText(">", 16, sf::Color(100, 240, 140));
            arrow.setPosition({listX - 16.f, listY + (i - start) * 28.f});
            w.draw(arrow);
        }
    }

    float panelX = WIN_W / 2.f - 10.f;
    float panelY = WIN_H / 2.f - 175.f;
    float panelW = 295.f;
    float panelH = 310.f;
    m_demo.setArea({panelX, panelY}, {panelW, panelH});

    bool known = m_cursor < (int)WISDOM.size() && g_discovery.isDiscovered((PlatType)m_cursor);

    if (known) {
        if (m_prevCursor == -1) {
            m_prevCursor = m_cursor;
            m_demo.play((PlatType)m_cursor);
        }
        m_demo.draw(w, m_font);
    } else {
        sf::RectangleShape bg({panelW, panelH});
        bg.setPosition({panelX, panelY});
        bg.setFillColor(sf::Color(20, 12, 40));
        bg.setOutlineColor(sf::Color(60, 40, 100));
        bg.setOutlineThickness(1.f);
        w.draw(bg);

        auto msg =
            makeText("Step on this\nplatform to\nunlock its entry.", 16, sf::Color(90, 80, 110));
        auto mb = msg.getLocalBounds();
        msg.setOrigin({mb.position.x + mb.size.x / 2.f, mb.position.y + mb.size.y / 2.f});
        msg.setPosition({panelX + panelW / 2.f, panelY + panelH / 2.f});
        w.draw(msg);
    }
}

// ---------------------------------------------------------------------------
// Draw dispatch
// ---------------------------------------------------------------------------

void HubUI::draw(sf::RenderWindow& w) {
    if (m_panel == HubPanel::None) return;
    switch (m_panel) {
        case HubPanel::Shop:
            drawShop(w);
            break;
        case HubPanel::Cosmetics:
            drawTailor(w);
            break;
        case HubPanel::TimeKeeper:
            drawTimeKeeper(w);
            break;
        case HubPanel::Lore:
            drawLore(w);
            break;
        case HubPanel::WiseMan:
            drawWiseMan(w);
            break;
        default:
            break;
    }
}