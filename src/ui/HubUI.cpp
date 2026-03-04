#include "core/DiscoveryTracker.hpp"
#include "ui/HubUI.hpp"
#include "Constants.hpp"
#include <iomanip>
#include <sstream>

// ---------------------------------------------------------------------------
// Static content
// ---------------------------------------------------------------------------

const std::vector<std::pair<std::string,std::string>> HubUI::LORE = {
    {"Training Grounds",        "A place of gentle beginnings. The world above waits."},
    {"I - The Fool",            "Step forward with nothing but courage. The journey starts."},
    {"II - The Magician",       "All tools are yours. Learn to wield them with intent."},
    {"III - The High Priestess","Patience reveals what haste conceals. Move deliberately."},
    {"IV - The Empress",        "Abundance flows here. Rest, breathe, gather strength."},
    {"V - The Emperor",         "Order and structure. Every platform placed with purpose."},
    {"VI - The Hierophant",     "Follow the ancient path upward, step by step."},
    {"VII - The Lovers",        "Two paths diverge. Both lead onward. Choose wisely."},
    {"VIII - The Chariot",      "Speed and will. Hesitation is the only enemy here."},
    {"IX - Strength",           "Endurance, not force. Ten small steps beat one great leap."},
    {"X - The Hermit",          "Solitude and vast silence. Each platform an island."},
    {"XI - Wheel of Fortune",   "Chaos reigns. Adapt or fall. The wheel turns for all."},
    {"XII - Justice",           "Balance in all things. The world is symmetrical here."},
    {"XIII - The Hanged Man",   "Surrender to the fall. A great drop precedes the climb."},
    {"XIV - Death",             "Brutal and honest. Precision is survival."},
    {"XV - Temperance",         "Flow like water. Ropes and ladders are your friends."},
    {"XVI - The Devil",         "Deception lurks. Gaps are wider than they appear."},
    {"XVII - The Tower",        "Wall jump or be swallowed. The tower demands mastery."},
    {"XVIII - The Star",        "Hope after ruin. Wide platforms, generous spacing."},
    {"XIX - The Moon",          "Nothing is as it seems. Trust your instincts."},
    {"XX - The Sun",            "Joy and relief. A warm breather before the end."},
    {"XXI - Judgement",         "All skills tested one final time. Rise to the call."},
    {"XXII - The World",        "Completion. The top is within reach. Ascend."},
};

const std::vector<std::pair<std::string,std::string>> HubUI::WISDOM = {
    {"Static Platform  [=]",    "Solid ground. Won't move, won't break. The bedrock of the world."},
    {"Moving Horiz.    [~]",    "Rides left and right. Stand on it and it carries you — use that."},
    {"Moving Vert.     [^]",    "Rises and falls. Time your jump for when it's on the way up."},
    {"Crumbling        [@]",    "Shakes when you land, then crumbles. Don't linger."},
    {"Bounce Pad       [*]",    "Launches you upward on contact. Aim before you land."},
    {"Disappearing     [?]",    "Fades in and out on a timer. Watch the rhythm before committing."},
    {"One-Way          [-]",    "Pass through from below, solid from above. A shortcut if you know."},
    {"Hatch            [H]",    "Swings open when you land. Step off quickly or fall through."},
    {"Rope             [R]",    "Hold Up/Down near it to grab and climb. Jump to dismount."},
    {"Ladder           [L]",    "Same as rope but sturdier. Up/Down to climb, jump to leave."},
    {"Teleport         [T]",    "Stand still for half a second and you'll vanish — and reappear elsewhere."},
    {"Coin             [C]",    "Collect these. Spend them on upgrades and cosmetics with me."},
    {"Goal             [G]",    "The golden platform at the very top. Reach it to win the run."},
};

// ---------------------------------------------------------------------------
// HubUI
// ---------------------------------------------------------------------------

HubUI::HubUI(const sf::Font& font, CoinManager& coins, CosmeticsManager& cosmetics)
    : m_font(font), m_coins(coins), m_cosmetics(cosmetics) {}

void HubUI::open(HubPanel panel) {
    m_panel  = panel;
    m_cursor = 0;
}

void HubUI::close() { m_panel = HubPanel::None; }

void HubUI::handleInput(sf::Keyboard::Key key) {
    if (m_panel == HubPanel::None) return;

    int itemCount = 0;
    if      (m_panel == HubPanel::Shop || m_panel == HubPanel::Cosmetics)
        itemCount = (int)m_cosmetics.items().size();
    else if (m_panel == HubPanel::Lore)
        itemCount = (int)LORE.size();
    else if (m_panel == HubPanel::WiseMan)
        itemCount = (int)WISDOM.size();

    if (key == sf::Keyboard::Key::Up   || key == sf::Keyboard::Key::W)
        { if (itemCount > 0) m_cursor = (m_cursor - 1 + itemCount) % itemCount; }
    else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S)
        { if (itemCount > 0) m_cursor = (m_cursor + 1) % itemCount; }
    else if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::E) {
        if (m_panel == HubPanel::Shop) {
            int coins = m_coins.collectedCount();
            if (m_cosmetics.tryUnlock(m_cursor, coins))
                m_coins.setCollectedCount(coins);
        } else if (m_panel == HubPanel::Cosmetics) {
            m_cosmetics.equip(m_cursor);
        }
    } else if (key == sf::Keyboard::Key::Escape) {
        close();
    }
}

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

    auto hint = makeText("[Esc] Close   [W/S] Scroll   [E] Select", 13,
                         sf::Color(130, 130, 150));
    auto hb = hint.getLocalBounds();
    hint.setOrigin({hb.position.x + hb.size.x / 2.f, 0.f});
    hint.setPosition({WIN_W / 2.f, WIN_H / 2.f + 195.f});
    w.draw(hint);
}

void HubUI::drawShop(sf::RenderWindow& w) {
    drawBackground(w, "Merchant");

    auto coinLabel = makeText("Coins: " + std::to_string(m_coins.collectedCount()),
                              20, sf::Color(255, 210, 40));
    coinLabel.setPosition({WIN_W / 2.f - 290.f, WIN_H / 2.f - 175.f});
    w.draw(coinLabel);

    const auto& items = m_cosmetics.items();
    float startY = WIN_H / 2.f - 145.f;

    for (int i = 0; i < (int)items.size(); i++) {
        const auto& item = items[i];
        bool selected = (i == m_cursor);
        if (selected) {
            sf::RectangleShape hl({580.f, 26.f});
            hl.setPosition({WIN_W / 2.f - 290.f, startY + i * 30.f - 2.f});
            hl.setFillColor(sf::Color(80, 50, 130, 180));
            w.draw(hl);
        }
        sf::RectangleShape swatch({16.f, 16.f});
        swatch.setFillColor(item.previewColor);
        swatch.setPosition({WIN_W / 2.f - 285.f, startY + i * 30.f + 2.f});
        w.draw(swatch);

        sf::Color nameCol = item.unlocked ? sf::Color(100, 220, 100)
                          : selected      ? sf::Color(255, 255, 255)
                                          : sf::Color(180, 180, 180);
        auto name = makeText(item.name + "  [" + item.category + "]", 18, nameCol);
        name.setPosition({WIN_W / 2.f - 260.f, startY + i * 30.f});
        w.draw(name);

        std::string costStr = item.unlocked ? "Owned"
                            : std::to_string(item.cost) + " coins";
        sf::Color costCol = item.unlocked ? sf::Color(100, 200, 100)
                          : m_coins.collectedCount() >= item.cost
                            ? sf::Color(255, 210, 40) : sf::Color(180, 80, 80);
        auto cost = makeText(costStr, 18, costCol);
        auto cb = cost.getLocalBounds();
        cost.setPosition({WIN_W / 2.f + 290.f - cb.size.x, startY + i * 30.f});
        w.draw(cost);
    }
}

void HubUI::drawCosmetics(sf::RenderWindow& w) {
    drawBackground(w, "Tailor");

    const auto& items = m_cosmetics.items();
    float startY = WIN_H / 2.f - 160.f;
    bool anyUnlocked = false;

    for (int i = 0; i < (int)items.size(); i++) {
        const auto& item = items[i];
        if (!item.unlocked) continue;
        anyUnlocked = true;
        bool selected = (i == m_cursor);
        if (selected) {
            sf::RectangleShape hl({580.f, 26.f});
            hl.setPosition({WIN_W / 2.f - 290.f, startY + i * 30.f - 2.f});
            hl.setFillColor(sf::Color(80, 50, 130, 180));
            w.draw(hl);
        }
        sf::RectangleShape swatch({16.f, 16.f});
        swatch.setFillColor(item.previewColor);
        swatch.setPosition({WIN_W / 2.f - 285.f, startY + i * 30.f + 2.f});
        w.draw(swatch);

        sf::Color col = item.equipped ? sf::Color(255, 215, 0) : sf::Color(200, 200, 200);
        auto name = makeText(item.name + "  [" + item.category + "]"
                             + (item.equipped ? "  <<" : ""), 18, col);
        name.setPosition({WIN_W / 2.f - 260.f, startY + i * 30.f});
        w.draw(name);
    }
    if (!anyUnlocked) {
        auto msg = makeText("Nothing unlocked yet.\nVisit the Merchant first.",
                            20, sf::Color(160, 140, 180));
        auto mb = msg.getLocalBounds();
        msg.setOrigin({mb.position.x + mb.size.x / 2.f, mb.position.y + mb.size.y / 2.f});
        msg.setPosition({WIN_W / 2.f, WIN_H / 2.f});
        w.draw(msg);
    }
}

void HubUI::drawTimeKeeper(sf::RenderWindow& w) {
    drawBackground(w, "Chronicler");

    std::string timeStr;
    if (m_bestTime < 0.f) {
        timeStr = "No completed run yet.\nClimb to the top and return!";
    } else {
        int mins = (int)(m_bestTime / 60.f);
        float secs = m_bestTime - mins * 60.f;
        std::ostringstream ss;
        ss << "Best Time:  " << mins << ":"
           << std::fixed << std::setprecision(2)
           << std::setw(5) << std::setfill('0') << secs;
        timeStr = ss.str();
    }
    auto t = makeText(timeStr, 28, sf::Color(255, 215, 0));
    auto b = t.getLocalBounds();
    t.setOrigin({b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f});
    t.setPosition({WIN_W / 2.f, WIN_H / 2.f});
    w.draw(t);
}

void HubUI::drawLore(sf::RenderWindow& w) {
    drawBackground(w, "Seer  —  Arcane Lore");

    // Scrollable list — show entry name on left, selected entry text below
    float listX  = WIN_W / 2.f - 290.f;
    float startY = WIN_H / 2.f - 175.f;
    int   visible = 10;
    int   start   = std::max(0, m_cursor - visible / 2);

    for (int i = start; i < std::min(start + visible, (int)LORE.size()); ++i) {
        bool sel = (i == m_cursor);
        sf::Color col = sel ? sf::Color(255, 215, 0) : sf::Color(180, 170, 200);
        auto entry = makeText(LORE[i].first, 17, col);
        entry.setPosition({listX, startY + (i - start) * 26.f});
        w.draw(entry);
    }

    // Selected lore text
    if (m_cursor < (int)LORE.size()) {
        auto body = makeText(LORE[m_cursor].second, 16, sf::Color(210, 200, 230));
        body.setPosition({listX, WIN_H / 2.f + 110.f});
        w.draw(body);
    }
}

void HubUI::drawWiseMan(sf::RenderWindow& w) {
    drawBackground(w, "Sage  —  Platform Guide");

    float listX  = WIN_W / 2.f - 290.f;
    float startY = WIN_H / 2.f - 175.f;
    int   visible = 10;
    int   start   = std::max(0, m_cursor - visible / 2);

    for (int i = start; i < std::min(start + visible, (int)WISDOM.size()); ++i) {
        bool sel  = (i == m_cursor);
        bool known = g_discovery.isDiscovered((PlatType)i);

        sf::Color col;
        if (!known)       col = sf::Color(80, 80, 90);       // undiscovered — dim
        else if (sel)     col = sf::Color(100, 240, 140);    // selected
        else              col = sf::Color(160, 190, 170);

        std::string label = known ? WISDOM[i].first : "???  [undiscovered]";
        auto entry = makeText(label, 17, col);
        entry.setPosition({listX, startY + (i - start) * 26.f});
        w.draw(entry);
    }

    if (m_cursor < (int)WISDOM.size()) {
        bool known = g_discovery.isDiscovered((PlatType)m_cursor);
        std::string body = known ? WISDOM[m_cursor].second
                                 : "Step on this platform to learn about it.";
        sf::Color   col  = known ? sf::Color(200, 230, 210) : sf::Color(100, 100, 110);
        auto t = makeText(body, 16, col);
        t.setPosition({listX, WIN_H / 2.f + 110.f});
        w.draw(t);
    }
}

void HubUI::draw(sf::RenderWindow& w) {
    if (m_panel == HubPanel::None) return;
    switch (m_panel) {
        case HubPanel::Shop:       drawShop(w);       break;
        case HubPanel::Cosmetics:  drawCosmetics(w);  break;
        case HubPanel::TimeKeeper: drawTimeKeeper(w); break;
        case HubPanel::Lore:       drawLore(w);       break;
        case HubPanel::WiseMan:    drawWiseMan(w);    break;
        default: break;
    }
}