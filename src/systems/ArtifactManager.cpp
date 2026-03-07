#include "systems/ArtifactManager.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "Constants.hpp"

ArtifactManager g_artifacts;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

float ArtifactManager::highestMult(float a, float b) {
    // The value further from 1.0 wins
    return (std::abs(a - 1.f) >= std::abs(b - 1.f)) ? a : b;
}

// ---------------------------------------------------------------------------
// Artifact list
// ---------------------------------------------------------------------------
void ArtifactManager::buildArtifactList() {
    // Lambda to make artifact construction concise
    auto make = [](int id, const std::string& name, const std::string& desc,
                   const std::string& flavour, int price, sf::Color col, CosmeticSlot slot,
                   ArtifactMod mod) -> Artifact {
        Artifact a;
        a.id = id;
        a.name = name;
        a.description = desc;
        a.quirkyFlavour = flavour;
        a.price = price;
        a.color = col;
        a.slot = slot;
        a.mod = mod;
        return a;
    };

    ArtifactMod m;

    // 0 — Winged Boots
    m = {};
    m.jumpMult = 1.4f;
    m.fallMult = 0.7f;
    m_artifacts.push_back(make(0, "Winged Boots", "Jump height +40%, fall speed -30%",
                               "Stolen from a courier who wasn't using them.", 12,
                               sf::Color(120, 220, 180), CosmeticSlot::Boots, m));

    // 1 — Lead Greaves
    m = {};
    m.jumpMult = 0.75f;
    m.dashSpeedMult = 1.5f;
    m_artifacts.push_back(make(1, "Lead Greaves", "Jump height -25%, but dash is much faster",
                               "Heavy, but stylish in a brutalist sort of way.", 10,
                               sf::Color(140, 140, 180), CosmeticSlot::Boots, m));

    // 2 — Overclock Gauntlet
    m = {};
    m.speedMult = 1.5f;
    m.doubleJump = false;
    m_artifacts.push_back(make(2, "Overclock Gauntlet", "Move speed +50%, no double jump",
                               "Hums ominously. Don't ask where it came from.", 15,
                               sf::Color(255, 180, 60), CosmeticSlot::Hat, m));

    // 3 — Ghost Cloak
    m = {};
    m.disappearMult = 2.f;
    m_artifacts.push_back(make(3, "Ghost Cloak",
                               "Disappearing platforms stay invisible twice as long",
                               "Surprisingly warm for something incorporeal.", 8,
                               sf::Color(160, 100, 220), CosmeticSlot::Cape, m));

    // 4 — Moon Crown
    m = {};
    m.gravityMult = 0.5f;
    m.fallMult = 0.5f;
    m_artifacts.push_back(make(4, "Moon Crown", "Moon gravity — floaty jumps, slow falls",
                               "Radiates a faint silver light. Also cold.", 18,
                               sf::Color(200, 200, 255), CosmeticSlot::Hat, m));

    // 5 — Devil's Pact
    m = {};
    m.glassCannon = true;
    m.coinMult = 2.f;
    m_artifacts.push_back(make(
        5, "Devil's Pact", "Coins worth 2x, but falling far resets you to hub",
        "The contract was very small print.", 20, sf::Color(220, 60, 60), CosmeticSlot::Aura, m));

    // 6 — Coin Halo
    m = {};
    m.coinMagnetRadius = 96.f;
    m.coinMult = 1.5f;
    m_artifacts.push_back(make(6, "Coin Halo", "Coins auto-collect within 3 tiles, worth 1.5x",
                               "Greed made visible. Literally.", 14, sf::Color(255, 215, 0),
                               CosmeticSlot::Aura, m));

    // 7 — Slick Boots
    m = {};
    m.frictionMult = 0.05f;
    m.speedMult = 1.3f;
    m_artifacts.push_back(make(7, "Slick Boots", "Ice physics — no friction, but +30% speed",
                               "Leave an icy trail wherever you go.", 12, sf::Color(180, 240, 255),
                               CosmeticSlot::Boots, m));

    // 8 — Bone Wings
    m = {};
    m.jumpMult = 1.5f;
    m.doubleJump = false;
    m_artifacts.push_back(make(8, "Bone Wings", "Jump height +50%, no double jump",
                               "Clatter loudly. Very aerodynamic somehow.", 16,
                               sf::Color(230, 220, 200), CosmeticSlot::Wings, m));

    // 9 — Haunted Mirror
    m = {};
    m.haunted = true;
    m_artifacts.push_back(make(9, "Haunted Mirror", "A ghost mirrors your inputs 1 second behind",
                               "You didn't buy it. It just appeared.", 22, sf::Color(140, 180, 220),
                               CosmeticSlot::Cape, m));

    // 10 — Bouncy Boots
    m = {};
    m.bouncy = true;
    m.jumpMult = 1.2f;
    m_artifacts.push_back(make(10, "Bouncy Boots", "Every landing triggers a small bounce",
                               "Annoyingly fun. Funly annoying.", 10, sf::Color(255, 160, 80),
                               CosmeticSlot::Boots, m));

    // 11 — Purist's Band
    m = {};
    m.dashEnabled = false;
    m.doubleJump = false;
    m.wallJump = false;
    m.speedMult = 1.4f;
    m_artifacts.push_back(make(11, "Purist's Band",
                               "No dash, double jump, or wall jump. +40% speed.",
                               "For those who find skill trees offensive.", 5,
                               sf::Color(220, 100, 100), CosmeticSlot::Hat, m));

    // -----------------------------------------------------------------------
    // Eye cosmetics — id 100-107, zero quirk effect, unlocked via achievements
    // -----------------------------------------------------------------------
    m = {};  // empty mods
    m_artifacts.push_back(make(100, "Spectral Shades", "Pure style. No effect.",
                               "Cool guys don't look at explosions.", 0, sf::Color(160, 160, 200),
                               CosmeticSlot::Eyes, m));

    m = {};
    m_artifacts.push_back(make(101, "Witch Goggles", "Pure style. No effect.",
                               "Brewed in a cauldron. Worn ironically.", 0, sf::Color(80, 200, 80),
                               CosmeticSlot::Eyes, m));

    m = {};
    m_artifacts.push_back(make(102, "Star Eyes", "Pure style. No effect.",
                               "You reached the top. Let your eyes say it.", 0,
                               sf::Color(255, 230, 80), CosmeticSlot::Eyes, m));

    m = {};
    m_artifacts.push_back(make(103, "Gold Monocle", "Pure style. No effect.",
                               "One eye is all it takes to judge.", 0, sf::Color(255, 215, 0),
                               CosmeticSlot::Eyes, m));

    m = {};
    m_artifacts.push_back(make(104, "Demon Eyes", "Pure style. No effect.",
                               "You made a deal. These came with it.", 0, sf::Color(200, 30, 30),
                               CosmeticSlot::Eyes, m));

    m = {};
    m_artifacts.push_back(make(105, "Scholar Lens", "Pure style. No effect.",
                               "You read the fine print. Both of them.", 0, sf::Color(80, 140, 220),
                               CosmeticSlot::Eyes, m));

    m = {};
    m_artifacts.push_back(make(106, "Crown Visor", "Pure style. No effect.",
                               "Royalty doesn't squint.", 0, sf::Color(220, 150, 30),
                               CosmeticSlot::Eyes, m));

    m = {};
    m_artifacts.push_back(make(107, "Void Gaze", "Pure style. No effect.",
                               "You looked into the abyss. It blinked first.", 0,
                               sf::Color(200, 180, 255), CosmeticSlot::Eyes, m));
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
ArtifactManager::ArtifactManager() { buildArtifactList(); }

// ---------------------------------------------------------------------------
// Shop
// ---------------------------------------------------------------------------
bool ArtifactManager::canAfford(int id, int coins) const {
    for (const auto& a : m_artifacts)
        if (a.id == id) return !a.owned && coins >= a.price;
    return false;
}

bool ArtifactManager::buy(int id, int& coins) {
    for (auto& a : m_artifacts) {
        if (a.id == id && !a.owned && coins >= a.price) {
            coins -= a.price;
            a.owned = true;
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Loadout
// ---------------------------------------------------------------------------
int ArtifactManager::equippedCount() const {
    int n = 0;
    for (const auto& a : m_artifacts)
        if (a.equipped) ++n;
    return n;
}

bool ArtifactManager::equip(int id) {
    // Find the artifact we want to equip
    Artifact* target = nullptr;
    for (auto& a : m_artifacts)
        if (a.id == id && a.owned) {
            target = &a;
            break;
        }
    if (!target) return false;

    // Unequip any existing artifact in the same slot
    for (auto& a : m_artifacts)
        if (a.equipped && a.slot == target->slot && a.id != id) a.equipped = false;

    target->equipped = true;
    rebuildMods();
    return true;
}

void ArtifactManager::unequip(int id) {
    for (auto& a : m_artifacts) {
        if (a.id == id) {
            a.equipped = false;
            rebuildMods();
            return;
        }
    }
}

void ArtifactManager::grantEyeCosmetic(int id) {
    for (auto& a : m_artifacts)
        if (a.id == id) {
            a.owned = true;
            return;
        }
}

// ---------------------------------------------------------------------------
// Rebuild mods — highest value wins for numerics, OR for bools
// ---------------------------------------------------------------------------
void ArtifactManager::rebuildMods() {
    m_mods = ActiveMods{};
    for (const auto& a : m_artifacts) {
        if (!a.equipped) continue;
        const ArtifactMod& m = a.mod;

        m_mods.jumpMult = highestMult(m_mods.jumpMult, m.jumpMult);
        m_mods.fallMult = highestMult(m_mods.fallMult, m.fallMult);
        m_mods.speedMult = highestMult(m_mods.speedMult, m.speedMult);
        m_mods.dashSpeedMult = highestMult(m_mods.dashSpeedMult, m.dashSpeedMult);
        m_mods.gravityMult = highestMult(m_mods.gravityMult, m.gravityMult);
        m_mods.frictionMult = highestMult(m_mods.frictionMult, m.frictionMult);
        m_mods.crumbleSpeedMult = highestMult(m_mods.crumbleSpeedMult, m.crumbleSpeedMult);
        m_mods.platformSpeedMult = highestMult(m_mods.platformSpeedMult, m.platformSpeedMult);
        m_mods.disappearMult = highestMult(m_mods.disappearMult, m.disappearMult);
        m_mods.coinMult = highestMult(m_mods.coinMult, m.coinMult);
        m_mods.sizeScale = highestMult(m_mods.sizeScale, m.sizeScale);
        m_mods.coinMagnetRadius = std::max(m_mods.coinMagnetRadius, m.coinMagnetRadius);

        // Bools — restrictions: AND (all must allow it)
        m_mods.dashEnabled = m_mods.dashEnabled && m.dashEnabled;
        m_mods.doubleJump = m_mods.doubleJump && m.doubleJump;
        m_mods.wallJump = m_mods.wallJump && m.wallJump;
        m_mods.coinsSpawn = m_mods.coinsSpawn && m.coinsSpawn;

        // Bools — effects: OR (any artifact can trigger)
        m_mods.inputMirrored = m_mods.inputMirrored || m.inputMirrored;
        m_mods.glassCannon = m_mods.glassCannon || m.glassCannon;
        m_mods.bouncy = m_mods.bouncy || m.bouncy;
        m_mods.haunted = m_mods.haunted || m.haunted;
        m_mods.loseCoinsOnFail = m_mods.loseCoinsOnFail || m.loseCoinsOnFail;
        m_mods.countdownTimer = m_mods.countdownTimer || m.countdownTimer;

        // Cosmetic slots — store array index for direct lookup in draw calls
        int idx = (int)(&a - m_artifacts.data());
        switch (a.slot) {
            case CosmeticSlot::Hat:
                m_mods.hatArtifact = idx;
                break;
            case CosmeticSlot::Boots:
                m_mods.bootsArtifact = idx;
                break;
            case CosmeticSlot::Cape:
                m_mods.capeArtifact = idx;
                break;
            case CosmeticSlot::Aura:
                m_mods.auraArtifact = idx;
                break;
            case CosmeticSlot::Eyes:
                m_mods.eyesArtifact = idx;
                break;
            case CosmeticSlot::Wings:
                m_mods.wingsArtifact = idx;
                break;
            default:
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// Serialisation
// ---------------------------------------------------------------------------
std::vector<bool> ArtifactManager::ownedBits() const {
    std::vector<bool> bits;
    for (const auto& a : m_artifacts) bits.push_back(a.owned);
    return bits;
}

std::vector<bool> ArtifactManager::equippedBits() const {
    std::vector<bool> bits;
    for (const auto& a : m_artifacts) bits.push_back(a.equipped);
    return bits;
}

void ArtifactManager::fromBits(const std::vector<bool>& owned, const std::vector<bool>& equipped) {
    for (int i = 0; i < (int)m_artifacts.size(); ++i) {
        if (i < (int)owned.size()) m_artifacts[i].owned = owned[i];
        if (i < (int)equipped.size()) m_artifacts[i].equipped = equipped[i];
    }
    rebuildMods();
}

// ---------------------------------------------------------------------------
// Draw card helper
// ---------------------------------------------------------------------------
void ArtifactManager::drawCard(sf::RenderWindow& window, const sf::Font& font, const Artifact& a,
                               sf::Vector2f pos, bool hovered, bool showPrice, int coins) const {
    const float W = 190.f, H = 200.f;
    float yOff = hovered ? -6.f : 0.f;

    // Background
    sf::RectangleShape card({W, H});
    card.setPosition({pos.x, pos.y + yOff});
    bool locked = !a.owned;
    card.setFillColor(locked ? sf::Color(20, 10, 35, 200) : sf::Color(30, 15, 55, 220));
    card.setOutlineColor(hovered ? a.color
                                 : (locked ? sf::Color(60, 50, 80) : sf::Color(80, 60, 120)));
    card.setOutlineThickness(hovered ? 3.f : 1.5f);
    window.draw(card);

    // Color bar
    sf::RectangleShape bar({W, 5.f});
    bar.setPosition({pos.x, pos.y + yOff});
    bar.setFillColor(locked ? sf::Color(60, 50, 80) : a.color);
    window.draw(bar);

    // Lock / equipped indicator
    std::string badge = a.equipped ? "[ON]" : (locked ? "[?]" : "[OFF]");
    sf::Color badgeCol = a.equipped
                             ? sf::Color(120, 255, 120)
                             : (locked ? sf::Color(120, 100, 160) : sf::Color(160, 140, 200));
    sf::Text badgeT(font, badge, 14);
    badgeT.setFillColor(badgeCol);
    badgeT.setPosition({pos.x + W - 46.f, pos.y + yOff + 8.f});
    window.draw(badgeT);

    // Name
    sf::Text nameT(font, locked ? "???" : a.name, 18);
    nameT.setFillColor(locked ? sf::Color(100, 90, 130) : sf::Color(230, 220, 255));
    nameT.setOutlineColor(sf::Color::Black);
    nameT.setOutlineThickness(1.f);
    nameT.setPosition({pos.x + 10.f, pos.y + yOff + 16.f});
    window.draw(nameT);

    if (!locked) {
        // Description
        sf::Text descT(font, a.description, 13);
        descT.setFillColor(sf::Color(170, 160, 200));
        descT.setPosition({pos.x + 10.f, pos.y + yOff + 44.f});
        // Simple word wrap
        std::string wrapped, word;
        float lineW = 0.f;
        for (char c : a.description + ' ') {
            if (c == ' ') {
                sf::Text tmp(font, word, 13);
                float ww = tmp.getLocalBounds().size.x;
                if (lineW + ww > W - 20.f && lineW > 0.f) {
                    wrapped += '\n';
                    lineW = 0.f;
                }
                wrapped += word + ' ';
                lineW += ww + 6.f;
                word.clear();
            } else
                word += c;
        }
        descT.setString(wrapped);
        window.draw(descT);

        // Flavour
        sf::Text flavT(font, a.quirkyFlavour, 12);
        flavT.setFillColor(sf::Color(120, 110, 160));
        flavT.setStyle(sf::Text::Italic);
        flavT.setPosition({pos.x + 10.f, pos.y + yOff + H - 40.f});
        window.draw(flavT);
    }

    // Price / owned
    if (showPrice) {
        std::string priceStr = a.owned ? "Owned"
                                       : (coins >= a.price ? std::to_string(a.price) + " C"
                                                           : std::to_string(a.price) + " C");
        sf::Color priceCol =
            a.owned ? sf::Color(120, 200, 120)
                    : (coins >= a.price ? sf::Color(255, 215, 0) : sf::Color(180, 80, 80));
        sf::Text priceT(font, priceStr, 15);
        priceT.setFillColor(priceCol);
        priceT.setOutlineColor(sf::Color::Black);
        priceT.setOutlineThickness(1.f);
        priceT.setPosition({pos.x + 10.f, pos.y + yOff + H - 26.f});
        window.draw(priceT);
    }
}

// ---------------------------------------------------------------------------
// Draw shop (Merchant)
// ---------------------------------------------------------------------------
void ArtifactManager::drawShop(sf::RenderWindow& window, const sf::Font& font, int coins,
                               int& cursor) const {
    // -----------------------------------------------------------------------
    // Full-screen overlay
    // -----------------------------------------------------------------------
    sf::RectangleShape bg({(float)WIN_W, (float)WIN_H});
    bg.setFillColor(sf::Color(8, 4, 20, 240));
    window.draw(bg);

    // Panel outline
    sf::RectangleShape panel({(float)WIN_W - 40.f, (float)WIN_H - 40.f});
    panel.setPosition({20.f, 20.f});
    panel.setFillColor(sf::Color(18, 10, 38, 255));
    panel.setOutlineColor(sf::Color(100, 70, 180));
    panel.setOutlineThickness(2.f);
    window.draw(panel);

    // -----------------------------------------------------------------------
    // Header
    // -----------------------------------------------------------------------
    sf::Text title(font, "Merchant  —  Artifacts", 28);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2.f);
    title.setPosition({36.f, 30.f});
    window.draw(title);

    sf::Text coinT(font, "Coins: " + std::to_string(coins), 18);
    coinT.setFillColor(sf::Color(255, 215, 0));
    auto ctb = coinT.getLocalBounds();
    coinT.setPosition({WIN_W - 36.f - ctb.size.x, 34.f});
    window.draw(coinT);

    sf::Text hint(font, "[W/S] Browse   [E/Enter] Buy   [Esc] Close", 13);
    hint.setFillColor(sf::Color(100, 90, 140));
    hint.setPosition({36.f, WIN_H - 36.f});
    window.draw(hint);

    // Divider under header
    sf::RectangleShape div({(float)WIN_W - 80.f, 1.f});
    div.setPosition({40.f, 68.f});
    div.setFillColor(sf::Color(80, 60, 120));
    window.draw(div);

    // Vertical divider between list and detail
    const float SPLIT = 300.f;
    sf::RectangleShape vdiv({1.f, (float)WIN_H - 120.f});
    vdiv.setPosition({40.f + SPLIT, 74.f});
    vdiv.setFillColor(sf::Color(80, 60, 120));
    window.draw(vdiv);

    // -----------------------------------------------------------------------
    // Left column — scrollable artifact list
    // -----------------------------------------------------------------------
    const float listX = 48.f;
    const float listY = 80.f;
    const float rowH = 34.f;
    const int visible = 12;
    int start = std::max(0, cursor - visible / 2);

    for (int i = start; i < std::min(start + visible, (int)m_artifacts.size()); ++i) {
        const Artifact& a = m_artifacts[i];
        bool sel = (i == cursor);
        bool owned = a.owned;
        bool afford = !owned && coins >= a.price;

        float ry = listY + (i - start) * rowH;

        // Row highlight
        if (sel) {
            sf::RectangleShape hl({SPLIT - 16.f, rowH - 4.f});
            hl.setPosition({listX - 4.f, ry});
            hl.setFillColor(sf::Color(60, 40, 100, 200));
            hl.setOutlineColor(a.color);
            hl.setOutlineThickness(1.f);
            window.draw(hl);
        }

        // Color dot
        sf::CircleShape dot(6.f);
        dot.setOrigin({6.f, 6.f});
        dot.setPosition({listX + 10.f, ry + rowH / 2.f});
        dot.setFillColor(owned ? a.color : sf::Color(a.color.r, a.color.g, a.color.b, 80));
        window.draw(dot);

        // Name
        sf::Color nameCol = owned    ? sf::Color(200, 255, 200)
                            : afford ? sf::Color(220, 220, 220)
                                     : sf::Color(120, 110, 140);
        sf::Text nameT(font, a.name, 16);
        nameT.setFillColor(nameCol);
        nameT.setPosition({listX + 24.f, ry + 6.f});
        window.draw(nameT);

        // Price / owned tag on right of row
        std::string tag = owned ? "Owned" : std::to_string(a.price) + " C";
        sf::Color tagCol = owned    ? sf::Color(100, 200, 100)
                           : afford ? sf::Color(255, 215, 0)
                                    : sf::Color(180, 80, 80);
        sf::Text tagT(font, tag, 14);
        tagT.setFillColor(tagCol);
        auto tagB = tagT.getLocalBounds();
        tagT.setPosition({40.f + SPLIT - 8.f - tagB.size.x, ry + 8.f});
        window.draw(tagT);

        // Selection arrow
        if (sel) {
            sf::Text arrow(font, ">", 16);
            arrow.setFillColor(a.color);
            arrow.setPosition({listX - 2.f, ry + 6.f});
            window.draw(arrow);
        }
    }

    // -----------------------------------------------------------------------
    // Right column — detail panel
    // -----------------------------------------------------------------------
    if (cursor >= (int)m_artifacts.size()) return;
    const Artifact& sel = m_artifacts[cursor];

    const float detX = 40.f + SPLIT + 20.f;
    const float detY = 80.f;
    const float detW = WIN_W - detX - 30.f;

    // ---- Artifact preview (rendered SFML shapes) ----
    const float previewCX = detX + detW / 2.f;
    const float previewCY = detY + 120.f;
    const float bodyW = 44.f, bodyH = 64.f, headR = 22.f;

    // Aura
    if (sel.slot == CosmeticSlot::Aura) {
        sf::CircleShape aura(bodyW * 1.4f);
        aura.setOrigin({bodyW * 1.4f, bodyW * 1.4f});
        aura.setPosition({previewCX, previewCY});
        aura.setFillColor(sf::Color(sel.color.r, sel.color.g, sel.color.b, 40));
        aura.setOutlineColor(sf::Color(sel.color.r, sel.color.g, sel.color.b, 160));
        aura.setOutlineThickness(3.f);
        window.draw(aura);
    }

    // Cape (behind body)
    if (sel.slot == CosmeticSlot::Cape) {
        sf::RectangleShape cape({bodyW * 1.6f, bodyH * 1.1f});
        cape.setOrigin({bodyW * 1.6f / 2.f, bodyH * 1.1f / 2.f});
        cape.setPosition({previewCX + 6.f, previewCY});
        cape.setFillColor(sf::Color(sel.color.r, sel.color.g, sel.color.b, 180));
        cape.setOutlineColor(sf::Color(sel.color.r, sel.color.g, sel.color.b, 80));
        cape.setOutlineThickness(1.f);
        window.draw(cape);
    }

    // Wings (behind body)
    if (sel.slot == CosmeticSlot::Wings) {
        for (int side : {-1, 1}) {
            sf::ConvexShape wing;
            wing.setPointCount(3);
            float wx = previewCX + side * bodyW * 0.5f;
            float wy = previewCY - bodyH * 0.2f;
            wing.setPoint(0, {wx, wy});
            wing.setPoint(1, {wx + side * bodyW * 1.4f, wy - bodyH * 0.5f});
            wing.setPoint(2, {wx + side * bodyW * 1.f, wy + bodyH * 0.6f});
            wing.setFillColor(sf::Color(sel.color.r, sel.color.g, sel.color.b, 200));
            wing.setOutlineColor(sf::Color(255, 255, 255, 40));
            wing.setOutlineThickness(1.f);
            window.draw(wing);
        }
    }

    // Body
    sf::RectangleShape body({bodyW, bodyH});
    body.setOrigin({bodyW / 2.f, bodyH / 2.f});
    body.setPosition({previewCX, previewCY});
    body.setFillColor(sf::Color(100, 180, 255));
    body.setOutlineColor(sf::Color(255, 255, 255, 60));
    body.setOutlineThickness(1.f);
    window.draw(body);

    // Head
    sf::CircleShape head(headR);
    head.setOrigin({headR, headR});
    head.setPosition({previewCX, previewCY - bodyH / 2.f - headR});
    head.setFillColor(sf::Color(100, 180, 255));
    head.setOutlineColor(sf::Color(255, 255, 255, 60));
    head.setOutlineThickness(1.f);
    window.draw(head);

    // Eyes
    for (int side : {-1, 1}) {
        sf::CircleShape eye(4.f);
        eye.setOrigin({4.f, 4.f});
        eye.setFillColor(sf::Color::White);
        eye.setPosition({previewCX + side * 7.f, previewCY - bodyH / 2.f - headR * 0.5f});
        window.draw(eye);
    }

    // Boots (below body)
    if (sel.slot == CosmeticSlot::Boots) {
        sf::RectangleShape boots({bodyW * 1.2f, bodyH * 0.22f});
        boots.setOrigin({bodyW * 1.2f / 2.f, 0.f});
        boots.setPosition({previewCX, previewCY + bodyH / 2.f - 2.f});
        boots.setFillColor(sel.color);
        boots.setOutlineColor(sf::Color(255, 255, 255, 60));
        boots.setOutlineThickness(1.f);
        window.draw(boots);
    }

    // Hat (above head)
    if (sel.slot == CosmeticSlot::Hat) {
        float hatY = previewCY - bodyH / 2.f - headR * 2.f;
        sf::RectangleShape hat({bodyW * 1.3f, headR * 0.8f});
        hat.setOrigin({bodyW * 1.3f / 2.f, headR * 0.8f});
        hat.setPosition({previewCX, hatY});
        hat.setFillColor(sel.color);
        hat.setOutlineColor(sf::Color(255, 255, 255, 60));
        hat.setOutlineThickness(1.f);
        window.draw(hat);
        sf::RectangleShape brim({bodyW * 1.8f, headR * 0.22f});
        brim.setOrigin({bodyW * 1.8f / 2.f, 0.f});
        brim.setPosition({previewCX, hatY});
        brim.setFillColor(sel.color);
        window.draw(brim);
    }

    // ---- Divider under preview ----
    sf::RectangleShape hdiv({detW, 1.f});
    hdiv.setPosition({detX, detY + 240.f});
    hdiv.setFillColor(sf::Color(80, 60, 120));
    window.draw(hdiv);

    // ---- Text info ----
    float ty = detY + 252.f;
    const float lineH = 26.f;

    // Name
    sf::Text nameT(font, sel.name, 22);
    nameT.setFillColor(sel.color);
    nameT.setOutlineColor(sf::Color::Black);
    nameT.setOutlineThickness(1.5f);
    nameT.setPosition({detX, ty});
    window.draw(nameT);
    ty += lineH + 6.f;

    // Cosmetic slot
    std::string slotStr;
    switch (sel.slot) {
        case CosmeticSlot::Hat:
            slotStr = "Hat";
            break;
        case CosmeticSlot::Boots:
            slotStr = "Boots";
            break;
        case CosmeticSlot::Cape:
            slotStr = "Cape";
            break;
        case CosmeticSlot::Aura:
            slotStr = "Aura";
            break;
        case CosmeticSlot::Wings:
            slotStr = "Wings";
            break;
        case CosmeticSlot::Eyes:
            slotStr = "Eyes";
            break;
        default:
            slotStr = "None";
            break;
    }
    sf::Text slotT(font, "Slot: " + slotStr, 14);
    slotT.setFillColor(sf::Color(160, 140, 200));
    slotT.setPosition({detX, ty});
    window.draw(slotT);
    ty += lineH;

    // Quirk effect
    sf::Text effectT(font, sel.description, 15);
    effectT.setFillColor(sf::Color(200, 240, 200));
    effectT.setPosition({detX, ty});
    // Word wrap
    std::string wrapped, word;
    float lineW = 0.f;
    for (char c : sel.description + ' ') {
        if (c == ' ') {
            sf::Text tmp(font, word, 15);
            float ww = tmp.getLocalBounds().size.x;
            if (lineW + ww > detW && lineW > 0.f) {
                wrapped += '\n';
                lineW = 0.f;
            }
            wrapped += word + ' ';
            lineW += ww + 6.f;
            word.clear();
        } else
            word += c;
    }
    effectT.setString(wrapped);
    window.draw(effectT);
    // Count newlines to advance ty
    int newlines = (int)std::count(wrapped.begin(), wrapped.end(), '\n');
    ty += lineH * (1 + newlines) + 4.f;

    // Flavour text
    sf::Text flavT(font, sel.quirkyFlavour, 13);
    flavT.setFillColor(sf::Color(110, 100, 150));
    flavT.setStyle(sf::Text::Italic);
    flavT.setPosition({detX, ty});
    window.draw(flavT);
    ty += lineH + 8.f;

    // Price / owned
    bool owned = sel.owned;
    bool afford = !owned && coins >= sel.price;
    std::string priceStr = owned ? "Already owned" : std::to_string(sel.price) + " coins";
    sf::Color priceCol = owned    ? sf::Color(100, 200, 100)
                         : afford ? sf::Color(255, 215, 0)
                                  : sf::Color(200, 80, 80);
    sf::Text priceT(font, priceStr, 16);
    priceT.setFillColor(priceCol);
    priceT.setOutlineColor(sf::Color::Black);
    priceT.setOutlineThickness(1.f);
    priceT.setPosition({detX, ty});
    window.draw(priceT);

    // Buy prompt
    if (!owned) {
        std::string prompt = afford ? "[E] Buy" : "Not enough coins";
        sf::Text promptT(font, prompt, 14);
        promptT.setFillColor(afford ? sf::Color(180, 255, 180) : sf::Color(160, 80, 80));
        promptT.setPosition({detX, ty + lineH});
        window.draw(promptT);
    }
}

// ---------------------------------------------------------------------------
// Draw tailor (Loadout)
// ---------------------------------------------------------------------------
void ArtifactManager::drawTailor(sf::RenderWindow& window, const sf::Font& font,
                                 int& cursor) const {
    // -----------------------------------------------------------------------
    // Background + panel
    // -----------------------------------------------------------------------
    sf::RectangleShape bg({(float)WIN_W, (float)WIN_H});
    bg.setFillColor(sf::Color(8, 4, 20, 240));
    window.draw(bg);

    sf::RectangleShape panel({(float)WIN_W - 40.f, (float)WIN_H - 40.f});
    panel.setPosition({20.f, 20.f});
    panel.setFillColor(sf::Color(18, 10, 38, 255));
    panel.setOutlineColor(sf::Color(100, 70, 180));
    panel.setOutlineThickness(2.f);
    window.draw(panel);

    // -----------------------------------------------------------------------
    // Header
    // -----------------------------------------------------------------------
    sf::Text title(font, "Tailor  —  Loadout", 28);
    title.setFillColor(sf::Color(160, 100, 220));
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2.f);
    title.setPosition({36.f, 30.f});
    window.draw(title);

    sf::Text hint(font, "[W/S] Browse   [E/Enter] Equip/Unequip   [Esc] Close", 13);
    hint.setFillColor(sf::Color(100, 90, 140));
    hint.setPosition({36.f, WIN_H - 36.f});
    window.draw(hint);

    // Dividers
    sf::RectangleShape hdiv({(float)WIN_W - 80.f, 1.f});
    hdiv.setPosition({40.f, 68.f});
    hdiv.setFillColor(sf::Color(80, 60, 120));
    window.draw(hdiv);

    const float SPLIT = 300.f;
    sf::RectangleShape vdiv({1.f, (float)WIN_H - 120.f});
    vdiv.setPosition({40.f + SPLIT, 74.f});
    vdiv.setFillColor(sf::Color(80, 60, 120));
    window.draw(vdiv);

    // -----------------------------------------------------------------------
    // Left column — owned artifact list
    // -----------------------------------------------------------------------
    std::vector<const Artifact*> owned;
    for (const auto& a : m_artifacts)
        if (a.owned) owned.push_back(&a);

    if (owned.empty()) {
        sf::Text empty(font, "No artifacts owned.\nVisit the Merchant first.", 18);
        empty.setFillColor(sf::Color(120, 100, 160));
        empty.setPosition({48.f, 100.f});
        window.draw(empty);
    } else {
        const float listX = 48.f;
        const float listY = 80.f;
        const float rowH = 34.f;
        const int visible = 12;
        int start = std::max(0, cursor - visible / 2);

        for (int i = start; i < std::min(start + visible, (int)owned.size()); ++i) {
            const Artifact& a = *owned[i];
            bool sel = (i == cursor);
            float ry = listY + (i - start) * rowH;

            if (sel) {
                sf::RectangleShape hl({SPLIT - 16.f, rowH - 4.f});
                hl.setPosition({listX - 4.f, ry});
                hl.setFillColor(sf::Color(60, 40, 100, 200));
                hl.setOutlineColor(a.color);
                hl.setOutlineThickness(1.f);
                window.draw(hl);
            }

            // Equipped indicator dot
            sf::CircleShape dot(6.f);
            dot.setOrigin({6.f, 6.f});
            dot.setPosition({listX + 10.f, ry + rowH / 2.f});
            dot.setFillColor(a.equipped ? a.color : sf::Color(a.color.r, a.color.g, a.color.b, 60));
            window.draw(dot);

            sf::Text nameT(font, a.name, 16);
            nameT.setFillColor(a.equipped ? sf::Color(255, 255, 200) : sf::Color(180, 170, 210));
            nameT.setPosition({listX + 24.f, ry + 6.f});
            window.draw(nameT);

            // Slot tag
            std::string slotStr;
            switch (a.slot) {
                case CosmeticSlot::Hat:
                    slotStr = "Hat";
                    break;
                case CosmeticSlot::Boots:
                    slotStr = "Boots";
                    break;
                case CosmeticSlot::Cape:
                    slotStr = "Cape";
                    break;
                case CosmeticSlot::Aura:
                    slotStr = "Aura";
                    break;
                case CosmeticSlot::Wings:
                    slotStr = "Wings";
                    break;
                case CosmeticSlot::Eyes:
                    slotStr = "Eyes";
                    break;
                default:
                    slotStr = "—";
                    break;
            }
            sf::Text slotT(font, slotStr, 13);
            slotT.setFillColor(sf::Color(120, 100, 160));
            auto sb = slotT.getLocalBounds();
            slotT.setPosition({40.f + SPLIT - 8.f - sb.size.x, ry + 9.f});
            window.draw(slotT);

            if (sel) {
                sf::Text arrow(font, ">", 16);
                arrow.setFillColor(a.color);
                arrow.setPosition({listX - 2.f, ry + 6.f});
                window.draw(arrow);
            }
        }
    }

    // -----------------------------------------------------------------------
    // Right column — player preview + slot assignments
    // -----------------------------------------------------------------------
    const float detX = 40.f + SPLIT + 20.f;
    const float detY = 80.f;
    const float detW = WIN_W - detX - 30.f;

    // ---- Player preview with all equipped cosmetics ----
    const float previewCX = detX + detW / 2.f;
    const float previewCY = detY + 130.f;
    const float bodyW = 44.f, bodyH = 64.f, headR = 22.f;

    // Cape
    if (m_mods.capeArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.capeArtifact];
        sf::RectangleShape cape({bodyW * 1.6f, bodyH * 1.1f});
        cape.setOrigin({bodyW * 1.6f / 2.f, bodyH * 1.1f / 2.f});
        cape.setPosition({previewCX + 8.f, previewCY});
        cape.setFillColor(sf::Color(a.color.r, a.color.g, a.color.b, 180));
        cape.setOutlineColor(sf::Color(a.color.r, a.color.g, a.color.b, 80));
        cape.setOutlineThickness(1.f);
        window.draw(cape);
    }

    // Aura
    if (m_mods.auraArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.auraArtifact];
        sf::CircleShape aura(bodyW * 1.4f);
        aura.setOrigin({bodyW * 1.4f, bodyW * 1.4f});
        aura.setPosition({previewCX, previewCY});
        aura.setFillColor(sf::Color(a.color.r, a.color.g, a.color.b, 35));
        aura.setOutlineColor(sf::Color(a.color.r, a.color.g, a.color.b, 150));
        aura.setOutlineThickness(3.f);
        window.draw(aura);
    }

    // Wings
    if (m_mods.wingsArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.wingsArtifact];
        for (int side : {-1, 1}) {
            sf::ConvexShape wing;
            wing.setPointCount(3);
            float wx = previewCX + side * bodyW * 0.5f;
            float wy = previewCY - bodyH * 0.2f;
            wing.setPoint(0, {wx, wy});
            wing.setPoint(1, {wx + side * bodyW * 1.4f, wy - bodyH * 0.5f});
            wing.setPoint(2, {wx + side * bodyW * 1.f, wy + bodyH * 0.6f});
            wing.setFillColor(sf::Color(a.color.r, a.color.g, a.color.b, 200));
            wing.setOutlineColor(sf::Color(255, 255, 255, 40));
            wing.setOutlineThickness(1.f);
            window.draw(wing);
        }
    }

    // Body
    sf::RectangleShape body({bodyW, bodyH});
    body.setOrigin({bodyW / 2.f, bodyH / 2.f});
    body.setPosition({previewCX, previewCY});
    body.setFillColor(sf::Color(100, 180, 255));
    body.setOutlineColor(sf::Color(255, 255, 255, 60));
    body.setOutlineThickness(1.f);
    window.draw(body);

    // Head
    sf::CircleShape head(headR);
    head.setOrigin({headR, headR});
    head.setPosition({previewCX, previewCY - bodyH / 2.f - headR});
    head.setFillColor(sf::Color(100, 180, 255));
    head.setOutlineColor(sf::Color(255, 255, 255, 60));
    head.setOutlineThickness(1.f);
    window.draw(head);

    // Eyes
    for (int side : {-1, 1}) {
        sf::CircleShape eye(4.f);
        eye.setOrigin({4.f, 4.f});
        eye.setFillColor(sf::Color::White);
        eye.setPosition({previewCX + side * 7.f, previewCY - bodyH / 2.f - headR * 0.5f});
        window.draw(eye);
    }

    // Boots
    if (m_mods.bootsArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.bootsArtifact];
        sf::RectangleShape boots({bodyW * 1.2f, bodyH * 0.22f});
        boots.setOrigin({bodyW * 1.2f / 2.f, 0.f});
        boots.setPosition({previewCX, previewCY + bodyH / 2.f - 2.f});
        boots.setFillColor(a.color);
        boots.setOutlineColor(sf::Color(255, 255, 255, 60));
        boots.setOutlineThickness(1.f);
        window.draw(boots);
    }

    // Hat
    if (m_mods.hatArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.hatArtifact];
        float hatY = previewCY - bodyH / 2.f - headR * 2.f;
        sf::RectangleShape hat({bodyW * 1.3f, headR * 0.8f});
        hat.setOrigin({bodyW * 1.3f / 2.f, headR * 0.8f});
        hat.setPosition({previewCX, hatY});
        hat.setFillColor(a.color);
        hat.setOutlineColor(sf::Color(255, 255, 255, 60));
        hat.setOutlineThickness(1.f);
        window.draw(hat);
        sf::RectangleShape brim({bodyW * 1.8f, headR * 0.22f});
        brim.setOrigin({bodyW * 1.8f / 2.f, 0.f});
        brim.setPosition({previewCX, hatY});
        brim.setFillColor(a.color);
        window.draw(brim);
    }

    // ---- Divider under preview ----
    sf::RectangleShape pdiv({detW, 1.f});
    pdiv.setPosition({detX, detY + 260.f});
    pdiv.setFillColor(sf::Color(80, 60, 120));
    window.draw(pdiv);

    // ---- Slot assignments list ----
    struct SlotDef {
        CosmeticSlot slot;
        std::string label;
        int idx;
    };
    const SlotDef slotDefs[] = {
        {CosmeticSlot::Hat, "Hat", m_mods.hatArtifact},
        {CosmeticSlot::Boots, "Boots", m_mods.bootsArtifact},
        {CosmeticSlot::Cape, "Cape", m_mods.capeArtifact},
        {CosmeticSlot::Aura, "Aura", m_mods.auraArtifact},
        {CosmeticSlot::Wings, "Wings", m_mods.wingsArtifact},
        {CosmeticSlot::Eyes, "Eyes", m_mods.eyesArtifact},
    };

    float sy = detY + 272.f;
    const float slotRowH = 28.f;

    for (const auto& sd : slotDefs) {
        sf::Text slotLabel(font, sd.label + ":", 15);
        slotLabel.setFillColor(sf::Color(140, 120, 180));
        slotLabel.setPosition({detX, sy});
        window.draw(slotLabel);

        std::string equippedName = "—";
        sf::Color equippedCol = sf::Color(80, 70, 100);
        if (sd.idx >= 0 && sd.idx < (int)m_artifacts.size()) {
            equippedName = m_artifacts[sd.idx].name;
            equippedCol = m_artifacts[sd.idx].color;
        }
        sf::Text equippedT(font, equippedName, 15);
        equippedT.setFillColor(equippedCol);
        equippedT.setPosition({detX + 70.f, sy});
        window.draw(equippedT);

        sy += slotRowH;
    }

    // ---- Equip/unequip prompt for selected artifact ----
    if (!owned.empty() && cursor < (int)owned.size()) {
        const Artifact& sel = *owned[cursor];
        bool isEquipped = sel.equipped;
        std::string action = isEquipped ? "[E] Unequip " + sel.name : "[E] Equip " + sel.name;
        sf::Color actionCol = isEquipped ? sf::Color(220, 100, 100) : sf::Color(120, 220, 120);
        sf::Text actionT(font, action, 15);
        actionT.setFillColor(actionCol);
        actionT.setOutlineColor(sf::Color::Black);
        actionT.setOutlineThickness(1.f);
        actionT.setPosition({detX, WIN_H - 60.f});
        window.draw(actionT);

        // Slot conflict note
        if (!isEquipped) {
            for (const auto& a : m_artifacts) {
                if (a.equipped && a.slot == sel.slot && a.id != sel.id) {
                    sf::Text conflict(font, "Replaces: " + a.name, 13);
                    conflict.setFillColor(sf::Color(180, 140, 80));
                    conflict.setPosition({detX, WIN_H - 40.f});
                    window.draw(conflict);
                    break;
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Draw cosmetics on player
// ---------------------------------------------------------------------------
void ArtifactManager::drawCosmetics(sf::RenderWindow& window, sf::Vector2f pos, float scaleX,
                                    float scaleY, bool facingLeft) const {
    const float BW = 22.f * scaleX;
    const float BH = 32.f * scaleY;
    const float HR = 11.f * ((scaleX + scaleY) / 2.f);

    // --- Cape (behind player, drawn first) ---
    if (m_mods.capeArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.capeArtifact];
        float capeW = BW * 1.4f, capeH = BH * 1.1f;
        float capeX = facingLeft ? pos.x + BW * 0.3f : pos.x - BW * 0.3f;
        sf::RectangleShape cape({capeW, capeH});
        cape.setOrigin({capeW / 2.f, capeH / 2.f});
        cape.setPosition({capeX, pos.y});
        cape.setFillColor(sf::Color(a.color.r, a.color.g, a.color.b, 180));
        cape.setOutlineColor(sf::Color(a.color.r, a.color.g, a.color.b, 80));
        cape.setOutlineThickness(1.f);
        window.draw(cape);
    }

    // --- Aura (glow ring) ---
    if (m_mods.auraArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.auraArtifact];
        sf::CircleShape aura(BW * 1.2f);
        aura.setOrigin({BW * 1.2f, BW * 1.2f});
        aura.setPosition(pos);
        aura.setFillColor(sf::Color(a.color.r, a.color.g, a.color.b, 40));
        aura.setOutlineColor(sf::Color(a.color.r, a.color.g, a.color.b, 120));
        aura.setOutlineThickness(2.f);
        window.draw(aura);
    }

    // --- Wings ---
    if (m_mods.wingsArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.wingsArtifact];
        for (int side : {-1, 1}) {
            sf::ConvexShape wing;
            wing.setPointCount(3);
            float wx = pos.x + side * BW * 0.5f;
            float wy = pos.y - BH * 0.2f;
            wing.setPoint(0, {wx, wy});
            wing.setPoint(1, {wx + side * BW * 1.2f, wy - BH * 0.4f});
            wing.setPoint(2, {wx + side * BW * 0.8f, wy + BH * 0.5f});
            wing.setFillColor(sf::Color(a.color.r, a.color.g, a.color.b, 200));
            wing.setOutlineColor(sf::Color(255, 255, 255, 60));
            wing.setOutlineThickness(1.f);
            window.draw(wing);
        }
    }

    // --- Boots (drawn below body) ---
    if (m_mods.bootsArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.bootsArtifact];
        sf::RectangleShape boot({BW * 1.1f, BH * 0.25f});
        boot.setOrigin({BW * 1.1f / 2.f, 0.f});
        boot.setPosition({pos.x, pos.y + BH * 0.5f - 2.f});
        boot.setFillColor(a.color);
        boot.setOutlineColor(sf::Color(255, 255, 255, 60));
        boot.setOutlineThickness(1.f);
        window.draw(boot);
    }

    // --- Hat (drawn above head) ---
    if (m_mods.hatArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.hatArtifact];
        float headTopY = pos.y - BH * 0.5f - HR * 2.f;
        sf::RectangleShape hat({BW * 1.3f, HR * 0.7f});
        hat.setOrigin({BW * 1.3f / 2.f, HR * 0.7f});
        hat.setPosition({pos.x, headTopY});
        hat.setFillColor(a.color);
        hat.setOutlineColor(sf::Color(255, 255, 255, 60));
        hat.setOutlineThickness(1.f);
        window.draw(hat);
        // Brim
        sf::RectangleShape brim({BW * 1.7f, HR * 0.2f});
        brim.setOrigin({BW * 1.7f / 2.f, 0.f});
        brim.setPosition({pos.x, headTopY});
        brim.setFillColor(a.color);
        window.draw(brim);
    }

    // --- Eyes cosmetic (drawn over head eyes, on top of everything) ---
    if (m_mods.eyesArtifact >= 0) {
        const Artifact& a = m_artifacts[m_mods.eyesArtifact];
        float eyeY = pos.y - BH * 0.5f - HR * 0.5f;

        for (int side : {-1, 1}) {
            float ex = pos.x + side * 7.f * ((BW / 22.f));

            switch (a.id) {
                case 100: {  // Spectral Shades
                    sf::RectangleShape lens({BW * 0.55f, BH * 0.08f});
                    lens.setOrigin({BW * 0.55f / 2.f, BH * 0.04f});
                    lens.setPosition({ex, eyeY});
                    lens.setFillColor(sf::Color(20, 20, 40, 220));
                    lens.setOutlineColor(sf::Color(180, 180, 220));
                    lens.setOutlineThickness(0.8f);
                    window.draw(lens);
                    break;
                }
                case 101: {  // Witch Goggles
                    sf::CircleShape g(HR * 0.28f);
                    g.setOrigin({HR * 0.28f, HR * 0.28f});
                    g.setPosition({ex, eyeY});
                    g.setFillColor(sf::Color(60, 180, 60, 200));
                    g.setOutlineColor(sf::Color(100, 220, 100));
                    g.setOutlineThickness(0.8f);
                    window.draw(g);
                    break;
                }
                case 102: {  // Star Eyes
                    sf::CircleShape star(HR * 0.28f, 4);
                    star.setOrigin({HR * 0.28f, HR * 0.28f});
                    star.setRotation(sf::degrees(45.f));
                    star.setPosition({ex, eyeY});
                    star.setFillColor(sf::Color(255, 230, 80));
                    window.draw(star);
                    break;
                }
                case 103: {  // Gold Monocle — one side circle, other plain
                    if (side == 1) {
                        sf::CircleShape mono(HR * 0.38f);
                        mono.setOrigin({HR * 0.38f, HR * 0.38f});
                        mono.setPosition({ex, eyeY});
                        mono.setFillColor(sf::Color(0, 0, 0, 0));
                        mono.setOutlineColor(sf::Color(255, 215, 0));
                        mono.setOutlineThickness(1.2f);
                        window.draw(mono);
                    } else {
                        sf::CircleShape eye(HR * 0.22f);
                        eye.setOrigin({HR * 0.22f, HR * 0.22f});
                        eye.setFillColor(sf::Color::White);
                        eye.setPosition({ex, eyeY});
                        window.draw(eye);
                    }
                    break;
                }
                case 104: {  // Demon Eyes
                    sf::CircleShape outer(HR * 0.28f);
                    outer.setOrigin({HR * 0.28f, HR * 0.28f});
                    outer.setPosition({ex, eyeY});
                    outer.setFillColor(sf::Color(200, 30, 30));
                    window.draw(outer);
                    sf::RectangleShape slit({BW * 0.08f, HR * 0.5f});
                    slit.setOrigin({BW * 0.04f, HR * 0.25f});
                    slit.setPosition({ex, eyeY});
                    slit.setFillColor(sf::Color(10, 0, 0));
                    window.draw(slit);
                    break;
                }
                case 105: {  // Scholar Lens
                    sf::CircleShape lens(HR * 0.28f);
                    lens.setOrigin({HR * 0.28f, HR * 0.28f});
                    lens.setPosition({ex, eyeY});
                    lens.setFillColor(sf::Color(80, 140, 220, 180));
                    lens.setOutlineColor(sf::Color(140, 180, 255));
                    lens.setOutlineThickness(0.8f);
                    window.draw(lens);
                    break;
                }
                case 106: {  // Crown Visor — single wide bar across both eyes
                    if (side == -1) {
                        sf::RectangleShape visor({BW * 1.1f, BH * 0.08f});
                        visor.setOrigin({0.f, BH * 0.04f});
                        visor.setPosition({pos.x - BW * 0.55f, eyeY});
                        visor.setFillColor(sf::Color(220, 150, 30));
                        visor.setOutlineColor(sf::Color(255, 200, 80));
                        visor.setOutlineThickness(0.8f);
                        window.draw(visor);
                    }
                    break;
                }
                case 107: {  // Void Gaze
                    sf::CircleShape outer(HR * 0.28f);
                    outer.setOrigin({HR * 0.28f, HR * 0.28f});
                    outer.setPosition({ex, eyeY});
                    outer.setFillColor(sf::Color(5, 5, 15));
                    outer.setOutlineColor(sf::Color(200, 180, 255));
                    outer.setOutlineThickness(1.f);
                    window.draw(outer);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Haunted ghost
// ---------------------------------------------------------------------------
void ArtifactManager::updateGhost(float dt, sf::Vector2f playerPos) {
    if (!m_mods.haunted) return;
    m_ghostHistory.push_back({playerPos, GHOST_DELAY});
    for (auto& f : m_ghostHistory) f.timer -= dt;
    for (int i = (int)m_ghostHistory.size() - 1; i >= 0; --i) {
        if (m_ghostHistory[i].timer <= 0.f) {
            m_ghostPos = m_ghostHistory[i].pos;
            m_ghostHistory.erase(m_ghostHistory.begin(), m_ghostHistory.begin() + i + 1);
            break;
        }
    }
}

void ArtifactManager::drawGhost(sf::RenderWindow& window) const {
    if (!m_mods.haunted || m_ghostPos.x < -9000.f) return;
    sf::RectangleShape body({22.f, 32.f});
    body.setOrigin({11.f, 16.f});
    body.setPosition(m_ghostPos);
    body.setFillColor(sf::Color(140, 180, 220, 80));
    body.setOutlineColor(sf::Color(180, 220, 255, 60));
    body.setOutlineThickness(1.f);
    window.draw(body);
    sf::CircleShape head(11.f);
    head.setOrigin({11.f, 11.f});
    head.setPosition({m_ghostPos.x, m_ghostPos.y - 27.f});
    head.setFillColor(sf::Color(160, 200, 240, 80));
    window.draw(head);
}

// ---------------------------------------------------------------------------
// Draw HUD badges — one per equipped artifact, top-right corner
// ---------------------------------------------------------------------------
void ArtifactManager::drawBadges(sf::RenderWindow& window, const sf::Font& font) const {
    float x = (float)WIN_W - 190.f;
    float y = 10.f;
    for (const auto& a : m_artifacts) {
        if (!a.equipped) continue;
        sf::RectangleShape badge({180.f, 24.f});
        badge.setPosition({x, y});
        badge.setFillColor(sf::Color(20, 10, 40, 200));
        badge.setOutlineColor(a.color);
        badge.setOutlineThickness(1.5f);
        window.draw(badge);

        sf::Text label(font, a.name, 14);
        label.setFillColor(a.color);
        label.setPosition({x + 6.f, y + 4.f});
        window.draw(label);

        y += 28.f;
    }
}