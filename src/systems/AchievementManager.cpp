#include "systems/AchievementManager.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "Constants.hpp"
#include "systems/ArtifactManager.hpp"

AchievementManager g_achievements;

// ---------------------------------------------------------------------------
// Artifact ID reference (matches buildArtifactList order in ArtifactManager)
// ---------------------------------------------------------------------------
// 0  Winged Boots       1  Lead Greaves     2  Overclock Gauntlet
// 3  Ghost Cloak        4  Moon Crown       5  Devil's Pact
// 6  Coin Halo          7  Slick Boots      8  Bone Wings
// 9  Haunted Mirror    10  Bouncy Boots    11  Purist's Band
//
// Eye cosmetic IDs (100+) — purely visual, no quirks
// 100 Spectral Shades   101 Witch Goggles   102 Star Eyes
// 103 Gold Monocle      104 Demon Eyes      105 Scholar Lens
// 106 Crown Visor       107 Void Gaze

// ---------------------------------------------------------------------------
// Build
// ---------------------------------------------------------------------------
AchievementManager::AchievementManager() { buildList(); }

void AchievementManager::buildList() {
    m_achievements.clear();
    int id = 0;

    auto add = [&](std::string name, std::string desc, std::string flavour, AchievCondition cond,
                   int a1, int a2, float tl, int reward, std::function<bool(const RunResult&)> fn) {
        Achievement a;
        a.id = id++;
        a.name = name;
        a.description = desc;
        a.flavour = flavour;
        a.condition = cond;
        a.artifactId1 = a1;
        a.artifactId2 = a2;
        a.timeLimit = tl;
        a.rewardArtifactId = reward;
        a.check = fn;
        m_achievements.push_back(a);
    };

    // 0 — Speedrunner: under 3 minutes
    add("Speedrunner", "Complete a run in under 3:00.", "Time is a river. You swam upstream.",
        AchievCondition::UnderTime, -1, -1, 180.f, 100,
        [](const RunResult& r) { return r.time < 180.f; });

    // 1 — Ghost Run: no dash used
    add("Ghost Run", "Complete a run without using dash.",
        "You moved like smoke. Nothing held you.", AchievCondition::NoDash, -1, -1, 0.f, 101,
        [](const RunResult& r) { return !r.dashUsed; });

    // 2 — Coin Hoarder: collect all coins
    add("Coin Hoarder", "Collect every coin in a single run.",
        "Not a single mote of gold escaped you.", AchievCondition::AllCoins, -1, -1, 0.f, 102,
        [](const RunResult& r) { return r.totalCoins > 0 && r.coinsCollected >= r.totalCoins; });

    // 3 — Devil's Bargain: complete with Devil's Pact
    add("Devil's Bargain", "Complete a run wearing the Devil's Pact.",
        "You signed in blood. The tower accepted.", AchievCondition::DevilsPact, 5, -1, 0.f, 103,
        [](const RunResult& r) {
            return std::find(r.equippedIds.begin(), r.equippedIds.end(), 5) != r.equippedIds.end();
        });

    // 4 — Wing and a Prayer: complete with Bone Wings
    add("Wing and a Prayer", "Complete a run wearing Bone Wings.",
        "Featherless flight. Impossible, yet here you are.", AchievCondition::SpecificArtifact, 8,
        -1, 0.f, 104, [](const RunResult& r) {
            return std::find(r.equippedIds.begin(), r.equippedIds.end(), 8) != r.equippedIds.end();
        });

    // 5 — Purist: complete with Purist's Band
    add("The Purist", "Complete a run wearing the Purist's Band.",
        "No tricks. No shortcuts. Just you and the climb.", AchievCondition::SpecificArtifact, 11,
        -1, 0.f, 105, [](const RunResult& r) {
            return std::find(r.equippedIds.begin(), r.equippedIds.end(), 11) != r.equippedIds.end();
        });

    // 6 — Moonwalker: complete with Moon Crown + Slick Boots
    add("Moonwalker", "Complete a run wearing Moon Crown and Slick Boots together.",
        "Floaty. Slidey. Somehow, victorious.", AchievCondition::ArtifactCombo, 4, 7, 0.f, 106,
        [](const RunResult& r) {
            bool hasMoon =
                std::find(r.equippedIds.begin(), r.equippedIds.end(), 4) != r.equippedIds.end();
            bool hasSlick =
                std::find(r.equippedIds.begin(), r.equippedIds.end(), 7) != r.equippedIds.end();
            return hasMoon && hasSlick;
        });

    // 7 — Haunted Speed: complete with Haunted Mirror + Overclock Gauntlet
    add("Haunted Speed", "Complete a run wearing Haunted Mirror and Overclock Gauntlet.",
        "Two of you. Both fast. One slightly behind.", AchievCondition::ArtifactCombo, 9, 2, 0.f,
        107, [](const RunResult& r) {
            bool hasHaunted =
                std::find(r.equippedIds.begin(), r.equippedIds.end(), 9) != r.equippedIds.end();
            bool hasOverclock =
                std::find(r.equippedIds.begin(), r.equippedIds.end(), 2) != r.equippedIds.end();
            return hasHaunted && hasOverclock;
        });
}

// ---------------------------------------------------------------------------
// Run complete — check all incomplete achievements
// ---------------------------------------------------------------------------
void AchievementManager::onRunComplete(const RunResult& result) {
    for (auto& a : m_achievements) {
        if (a.completed) continue;
        if (a.check && a.check(result)) {
            a.completed = true;
            a.notified = true;
        }
    }
}

// ---------------------------------------------------------------------------
// Claim next unclaimed completed achievement — returns reward artifact id
// ---------------------------------------------------------------------------
int AchievementManager::claimNext() {
    for (auto& a : m_achievements) {
        if (a.completed && !a.claimed) {
            a.claimed = true;
            a.notified = false;
            return a.rewardArtifactId;
        }
    }
    return -1;
}

bool AchievementManager::hasUnclaimed() const {
    for (const auto& a : m_achievements)
        if (a.completed && !a.claimed) return true;
    return false;
}

// ---------------------------------------------------------------------------
// Serialisation
// ---------------------------------------------------------------------------
std::vector<bool> AchievementManager::completedBits() const {
    std::vector<bool> v;
    for (const auto& a : m_achievements) v.push_back(a.completed);
    return v;
}

std::vector<bool> AchievementManager::claimedBits() const {
    std::vector<bool> v;
    for (const auto& a : m_achievements) v.push_back(a.claimed);
    return v;
}

void AchievementManager::restoreBits(const std::vector<bool>& completed,
                                     const std::vector<bool>& claimed) {
    for (int i = 0; i < (int)m_achievements.size(); ++i) {
        if (i < (int)completed.size()) m_achievements[i].completed = completed[i];
        if (i < (int)claimed.size()) m_achievements[i].claimed = claimed[i];
    }
}

// ---------------------------------------------------------------------------
// Chronicler UI
// ---------------------------------------------------------------------------
static std::string fmtTime(float t) {
    int mins = (int)(t / 60.f);
    float secs = t - mins * 60.f;
    std::ostringstream ss;
    ss << mins << ":" << std::fixed << std::setprecision(2) << std::setw(5) << std::setfill('0')
       << secs;
    return ss.str();
}

void AchievementManager::drawChronicler(sf::RenderWindow& window, const sf::Font& font,
                                        float bestTime, int& cursor) const {
    // -----------------------------------------------------------------------
    // Background + panel
    // -----------------------------------------------------------------------
    sf::RectangleShape bg({(float)WIN_W, (float)WIN_H});
    bg.setFillColor(sf::Color(8, 4, 20, 240));
    window.draw(bg);

    sf::RectangleShape panel({(float)WIN_W - 40.f, (float)WIN_H - 40.f});
    panel.setPosition({20.f, 20.f});
    panel.setFillColor(sf::Color(18, 10, 38, 255));
    panel.setOutlineColor(sf::Color(80, 140, 200));
    panel.setOutlineThickness(2.f);
    window.draw(panel);

    // Header
    sf::Text title(font, "Chronicler  —  Achievements", 28);
    title.setFillColor(sf::Color(120, 200, 255));
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(2.f);
    title.setPosition({36.f, 30.f});
    window.draw(title);

    // Best time (pure / artifactless note)
    std::string timeStr = bestTime < 0.f ? "No pure run yet" : "Pure best: " + fmtTime(bestTime);
    sf::Text timeT(font, timeStr, 16);
    timeT.setFillColor(sf::Color(180, 220, 255));
    auto tb2 = timeT.getLocalBounds();
    timeT.setPosition({WIN_W - 36.f - tb2.size.x, 34.f});
    window.draw(timeT);

    sf::Text hint(font, "[W/S] Browse   [E] Claim reward   [Esc] Close", 13);
    hint.setFillColor(sf::Color(100, 90, 140));
    hint.setPosition({36.f, WIN_H - 36.f});
    window.draw(hint);

    // Dividers
    sf::RectangleShape hdiv({(float)WIN_W - 80.f, 1.f});
    hdiv.setPosition({40.f, 68.f});
    hdiv.setFillColor(sf::Color(60, 90, 130));
    window.draw(hdiv);

    const float SPLIT = 300.f;
    sf::RectangleShape vdiv({1.f, (float)WIN_H - 120.f});
    vdiv.setPosition({40.f + SPLIT, 74.f});
    vdiv.setFillColor(sf::Color(60, 90, 130));
    window.draw(vdiv);

    // -----------------------------------------------------------------------
    // Left — achievement list
    // -----------------------------------------------------------------------
    const float listX = 48.f;
    const float listY = 80.f;
    const float rowH = 36.f;
    const int visible = 11;
    int start = std::max(0, cursor - visible / 2);

    for (int i = start; i < std::min(start + visible, (int)m_achievements.size()); ++i) {
        const Achievement& a = m_achievements[i];
        bool sel = (i == cursor);
        float ry = listY + (i - start) * rowH;

        // Row bg
        if (sel) {
            sf::RectangleShape hl({SPLIT - 16.f, rowH - 4.f});
            hl.setPosition({listX - 4.f, ry});
            hl.setFillColor(sf::Color(30, 50, 80, 200));
            hl.setOutlineColor(sf::Color(80, 160, 220));
            hl.setOutlineThickness(1.f);
            window.draw(hl);
        }

        // Status icon
        sf::Text icon(font, a.claimed ? "*" : a.completed ? "!" : "o", 16);
        icon.setFillColor(a.claimed     ? sf::Color(255, 215, 0)
                          : a.completed ? sf::Color(120, 255, 120)
                                        : sf::Color(80, 70, 110));
        icon.setPosition({listX + 4.f, ry + 7.f});
        window.draw(icon);

        // Name
        sf::Color nameCol = a.claimed     ? sf::Color(180, 180, 140)
                            : a.completed ? sf::Color(200, 255, 200)
                                          : sf::Color(140, 130, 170);
        sf::Text nameT(font, a.name, 16);
        nameT.setFillColor(nameCol);
        nameT.setPosition({listX + 22.f, ry + 7.f});
        window.draw(nameT);

        // "NEW" badge for unclaimed
        if (a.completed && !a.claimed) {
            sf::Text badge(font, "NEW", 11);
            badge.setFillColor(sf::Color(255, 80, 80));
            auto bb = badge.getLocalBounds();
            badge.setPosition({40.f + SPLIT - 8.f - bb.size.x, ry + 10.f});
            window.draw(badge);
        }

        if (sel) {
            sf::Text arrow(font, ">", 16);
            arrow.setFillColor(sf::Color(80, 160, 220));
            arrow.setPosition({listX - 2.f, ry + 7.f});
            window.draw(arrow);
        }
    }

    // -----------------------------------------------------------------------
    // Right — detail panel
    // -----------------------------------------------------------------------
    if (cursor >= (int)m_achievements.size()) return;
    const Achievement& sel = m_achievements[cursor];

    const float detX = 40.f + SPLIT + 20.f;
    const float detY = 80.f;
    const float detW = WIN_W - detX - 30.f;

    // ---- Eye cosmetic preview ----
    const float previewCX = detX + detW / 2.f;
    const float previewCY = detY + 110.f;
    const float bodyW = 44.f, bodyH = 64.f, headR = 22.f;

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

    // Eyes — vary by reward id
    float eyeY = previewCY - bodyH / 2.f - headR * 0.5f;
    bool locked = !sel.completed;

    for (int side : {-1, 1}) {
        float ex = previewCX + side * 7.f;

        switch (sel.rewardArtifactId) {
            case 100: {  // Spectral Shades — rectangular dark lenses
                sf::RectangleShape lens({12.f, 5.f});
                lens.setOrigin({6.f, 2.5f});
                lens.setPosition({ex, eyeY});
                lens.setFillColor(locked ? sf::Color(60, 60, 80) : sf::Color(20, 20, 40, 220));
                lens.setOutlineColor(sf::Color(180, 180, 220));
                lens.setOutlineThickness(1.f);
                window.draw(lens);
                break;
            }
            case 101: {  // Witch Goggles — round green tinted
                sf::CircleShape goggle(5.5f);
                goggle.setOrigin({5.5f, 5.5f});
                goggle.setPosition({ex, eyeY});
                goggle.setFillColor(locked ? sf::Color(40, 60, 40) : sf::Color(60, 180, 60, 200));
                goggle.setOutlineColor(sf::Color(100, 200, 100));
                goggle.setOutlineThickness(1.5f);
                window.draw(goggle);
                break;
            }
            case 102: {  // Star Eyes — four-point star shape
                sf::CircleShape star(5.f, 4);
                star.setOrigin({5.f, 5.f});
                star.setRotation(sf::degrees(45.f));
                star.setPosition({ex, eyeY});
                star.setFillColor(locked ? sf::Color(80, 80, 40) : sf::Color(255, 230, 80));
                window.draw(star);
                break;
            }
            case 103: {  // Gold Monocle — single large circle, one side only
                if (side == 1) {
                    sf::CircleShape mono(7.f);
                    mono.setOrigin({7.f, 7.f});
                    mono.setPosition({ex, eyeY});
                    mono.setFillColor(sf::Color(0, 0, 0, 0));
                    mono.setOutlineColor(locked ? sf::Color(100, 80, 20) : sf::Color(255, 215, 0));
                    mono.setOutlineThickness(2.f);
                    window.draw(mono);
                } else {
                    sf::CircleShape eye(4.f);
                    eye.setOrigin({4.f, 4.f});
                    eye.setFillColor(sf::Color::White);
                    eye.setPosition({ex, eyeY});
                    window.draw(eye);
                }
                break;
            }
            case 104: {  // Demon Eyes — vertical slit pupil, red
                sf::CircleShape outer(5.f);
                outer.setOrigin({5.f, 5.f});
                outer.setPosition({ex, eyeY});
                outer.setFillColor(locked ? sf::Color(60, 20, 20) : sf::Color(200, 30, 30));
                window.draw(outer);
                sf::RectangleShape slit({2.f, 8.f});
                slit.setOrigin({1.f, 4.f});
                slit.setPosition({ex, eyeY});
                slit.setFillColor(sf::Color(10, 0, 0));
                window.draw(slit);
                break;
            }
            case 105: {  // Scholar Lens — round, blue-tinted half-moon
                sf::CircleShape lens(5.f);
                lens.setOrigin({5.f, 5.f});
                lens.setPosition({ex, eyeY});
                lens.setFillColor(locked ? sf::Color(20, 30, 60) : sf::Color(80, 140, 220, 180));
                lens.setOutlineColor(sf::Color(140, 180, 255));
                lens.setOutlineThickness(1.5f);
                window.draw(lens);
                break;
            }
            case 106: {  // Crown Visor — thin wide visor across both eyes
                if (side == -1) {
                    sf::RectangleShape visor({28.f, 5.f});
                    visor.setOrigin({0.f, 2.5f});
                    visor.setPosition({previewCX - 14.f, eyeY});
                    visor.setFillColor(locked ? sf::Color(60, 40, 20) : sf::Color(220, 150, 30));
                    visor.setOutlineColor(sf::Color(255, 200, 80));
                    visor.setOutlineThickness(1.f);
                    window.draw(visor);
                }
                break;
            }
            case 107: {  // Void Gaze — black with white ring
                sf::CircleShape outer(5.5f);
                outer.setOrigin({5.5f, 5.5f});
                outer.setPosition({ex, eyeY});
                outer.setFillColor(sf::Color(5, 5, 15));
                outer.setOutlineColor(locked ? sf::Color(60, 60, 80) : sf::Color(200, 180, 255));
                outer.setOutlineThickness(1.5f);
                window.draw(outer);
                break;
            }
            default: {
                sf::CircleShape eye(4.f);
                eye.setOrigin({4.f, 4.f});
                eye.setFillColor(sf::Color::White);
                eye.setPosition({ex, eyeY});
                window.draw(eye);
            }
        }
    }

    // Lock overlay on preview if not yet completed
    if (locked) {
        sf::Text lockT(font, "?", 36);
        lockT.setFillColor(sf::Color(80, 70, 110));
        auto lb = lockT.getLocalBounds();
        lockT.setOrigin({lb.position.x + lb.size.x / 2.f, lb.position.y + lb.size.y / 2.f});
        lockT.setPosition({previewCX, previewCY - bodyH / 2.f - headR * 0.5f});
        window.draw(lockT);
    }

    // ---- Divider ----
    sf::RectangleShape pdiv({detW, 1.f});
    pdiv.setPosition({detX, detY + 230.f});
    pdiv.setFillColor(sf::Color(60, 90, 130));
    window.draw(pdiv);

    // ---- Text info ----
    float ty = detY + 242.f;
    const float lineH = 26.f;

    // Achievement name
    sf::Color nameCol = sel.claimed     ? sf::Color(180, 180, 140)
                        : sel.completed ? sf::Color(180, 255, 180)
                                        : sf::Color(160, 150, 200);
    sf::Text nameT(font, sel.name, 22);
    nameT.setFillColor(nameCol);
    nameT.setOutlineColor(sf::Color::Black);
    nameT.setOutlineThickness(1.5f);
    nameT.setPosition({detX, ty});
    window.draw(nameT);
    ty += lineH + 4.f;

    // Condition description
    sf::Text condT(font, sel.description, 15);
    condT.setFillColor(sf::Color(180, 170, 210));
    condT.setPosition({detX, ty});
    window.draw(condT);
    ty += lineH + 2.f;

    // Reward name
    static const char* rewardNames[] = {"Spectral Shades", "Witch Goggles", "Star Eyes",
                                        "Gold Monocle",    "Demon Eyes",    "Scholar Lens",
                                        "Crown Visor",     "Void Gaze"};
    int ridx = sel.rewardArtifactId - 100;
    std::string rewardName = (ridx >= 0 && ridx < 8) ? rewardNames[ridx] : "Unknown";
    sf::Text rewardT(font, "Reward: " + rewardName + "  (Eyes)", 14);
    rewardT.setFillColor(sel.completed ? sf::Color(255, 215, 0) : sf::Color(80, 70, 100));
    rewardT.setPosition({detX, ty});
    window.draw(rewardT);
    ty += lineH + 4.f;

    // Flavour
    sf::Text flavT(font, "\"" + sel.flavour + "\"", 13);
    flavT.setFillColor(sf::Color(100, 90, 130));
    flavT.setStyle(sf::Text::Italic);
    flavT.setPosition({detX, ty});
    window.draw(flavT);
    ty += lineH + 8.f;

    // Status + claim prompt
    if (sel.claimed) {
        sf::Text done(font, "Reward claimed.", 15);
        done.setFillColor(sf::Color(140, 140, 110));
        done.setPosition({detX, ty});
        window.draw(done);
    } else if (sel.completed) {
        sf::Text claim(font, "[E] Claim reward", 16);
        claim.setFillColor(sf::Color(120, 255, 120));
        claim.setOutlineColor(sf::Color::Black);
        claim.setOutlineThickness(1.f);
        claim.setPosition({detX, ty});
        window.draw(claim);
    } else {
        sf::Text locked2(font, "Complete the challenge to unlock.", 14);
        locked2.setFillColor(sf::Color(90, 80, 120));
        locked2.setPosition({detX, ty});
        window.draw(locked2);
    }
}