#include "ui/Npc.hpp"

#include <algorithm>
#include <cmath>

#include "Constants.hpp"

static constexpr float BODY_W = 24.f;
static constexpr float BODY_H = 36.f;
static constexpr float HEAD_R = 14.f;
static constexpr float BOB_AMP = 4.f;
static constexpr float BOB_SPD = 1.8f;
static constexpr float CHARS_PER_SEC = 28.f;  // typewriter speed
static constexpr float LINE_HOLD = 3.2f;      // seconds per line after reveal

void NpcManager::addNpc(float x, float floorY, NpcType type, const std::string& name,
                        sf::Color bodyCol, sf::Color headCol, std::vector<std::string> lines) {
    Npc npc;
    npc.type = type;
    npc.name = name;
    npc.pos = {x, floorY};
    npc.bodyColor = bodyCol;
    npc.headColor = headCol;
    npc.lines = std::move(lines);

    npc.body.setSize({BODY_W, BODY_H});
    npc.body.setOrigin({BODY_W / 2.f, BODY_H});
    npc.body.setPosition({x, floorY});
    npc.body.setFillColor(bodyCol);
    npc.body.setOutlineColor(sf::Color(255, 255, 255, 60));
    npc.body.setOutlineThickness(1.f);

    npc.head.setRadius(HEAD_R);
    npc.head.setOrigin({HEAD_R, HEAD_R});
    npc.head.setPosition({x, floorY - BODY_H - HEAD_R});
    npc.head.setFillColor(headCol);
    npc.head.setOutlineColor(sf::Color(255, 255, 255, 80));
    npc.head.setOutlineThickness(1.f);

    m_npcs.push_back(npc);
}

void NpcManager::build(float worldH) {
    m_npcs.clear();
    float floorY = worldH - TILEF;

    addNpc(120.f, floorY, NpcType::Shop, "Merchant", sf::Color(200, 160, 40),
           sf::Color(230, 190, 80),
           {
               "Fine artifacts, fresh from the ruins.",
               "Gold talks. Everything else walks.",
               "I don't make the prices. The tower does.",
               "Every artifact has a story. Yours is expensive.",
               "Buy something. My familiar needs feeding.",
           });

    addNpc(280.f, floorY, NpcType::Cosmetics, "Tailor", sf::Color(160, 60, 200),
           sf::Color(200, 120, 240),
           {
               "Style is half the climb.",
               "One slot per type. I have standards.",
               "You can't wear two hats. Physically or metaphorically.",
               "The tower sees what you wear. Dress accordingly.",
               "A well-dressed climber is a confident climber.",
           });

    addNpc(512.f, floorY, NpcType::TimeKeeper, "Chronicler", sf::Color(40, 160, 200),
           sf::Color(80, 200, 240),
           {
               "I remember every run. Every fall. Every coin.",
               "Speed records only count without artifacts.",
               "Complete my challenges. Claim your reward.",
               "The tower has seen a thousand climbers. I've watched them all.",
               "Time is the only currency that can't be earned back.",
           });

    addNpc(740.f, floorY, NpcType::Lore, "Seer", sf::Color(180, 60, 60), sf::Color(220, 100, 100),
           {
               "The cards don't lie. Climbers do.",
               "Each floor has a name. Names have power.",
               "You carry more than a body up that tower.",
               "The fool begins. The world ends. That's the whole story.",
               "I've read your future. I won't spoil it.",
           });

    addNpc(900.f, floorY, NpcType::WiseMan, "Sage", sf::Color(60, 160, 80),
           sf::Color(100, 210, 130),
           {
               "Every platform teaches something. Pay attention.",
               "The crumbling ones build character. Briefly.",
               "Bouncing is not falling. Repeat that to yourself.",
               "Disappearing platforms exist. So do bad decisions.",
               "A rope climbed is a fall avoided.",
           });
}

int NpcManager::getNearbyNpc(sf::Vector2f playerPos) const {
    for (int i = 0; i < (int)m_npcs.size(); ++i) {
        float dx = playerPos.x - m_npcs[i].pos.x;
        float dy = playerPos.y - m_npcs[i].pos.y;
        if (std::sqrt(dx * dx + dy * dy) < m_npcs[i].interactRadius) return i;
    }
    return -1;
}

void NpcManager::update(float dt, sf::Vector2f playerPos) {
    for (auto& npc : m_npcs) {
        float dx = playerPos.x - npc.pos.x;
        float dy = playerPos.y - npc.pos.y;
        bool near = std::sqrt(dx * dx + dy * dy) < npc.interactRadius;

        if (!near) {
            npc.playerNearby = false;
            continue;
        }

        // Just became nearby — reset to start of current line with typewriter
        if (!npc.playerNearby) {
            npc.playerNearby = true;
            npc.revealTimer = 0.f;
            npc.revealChars = 0;
            npc.lineTimer = 0.f;
        }

        npc.playerNearby = true;

        if (npc.lines.empty()) continue;
        const std::string& line = npc.lines[npc.lineIndex];

        // Typewriter reveal
        npc.revealTimer += dt;
        npc.revealChars = std::min((int)(npc.revealTimer * CHARS_PER_SEC), (int)line.size());

        // Advance to next line after hold
        if (npc.revealChars >= (int)line.size()) {
            npc.lineTimer += dt;
            if (npc.lineTimer >= LINE_HOLD) {
                npc.lineIndex = (npc.lineIndex + 1) % (int)npc.lines.size();
                npc.revealTimer = 0.f;
                npc.revealChars = 0;
                npc.lineTimer = 0.f;
            }
        }
    }
}

void NpcManager::draw(sf::RenderWindow& window, const sf::Font& font) const {
    float t = m_bobClock.getElapsedTime().asSeconds();

    for (const auto& npc : m_npcs) {
        float bob = std::sin(t * BOB_SPD + npc.pos.x * 0.01f) * BOB_AMP;

        // Body + head
        sf::RectangleShape body = npc.body;
        sf::CircleShape head = npc.head;
        body.move({0.f, bob});
        head.move({0.f, bob});
        window.draw(body);
        window.draw(head);

        // Eyes
        sf::CircleShape eye(2.5f);
        eye.setFillColor(sf::Color(20, 10, 30));
        eye.setOrigin({2.5f, 2.5f});
        eye.setPosition({npc.pos.x - 5.f, npc.pos.y - BODY_H - HEAD_R * 0.6f + bob});
        window.draw(eye);
        eye.setPosition({npc.pos.x + 5.f, npc.pos.y - BODY_H - HEAD_R * 0.6f + bob});
        window.draw(eye);

        // Name tag
        sf::Text nameText(font, npc.name, 13);
        nameText.setFillColor(sf::Color(240, 240, 240));
        nameText.setOutlineColor(sf::Color::Black);
        nameText.setOutlineThickness(1.f);
        auto nb = nameText.getLocalBounds();
        nameText.setOrigin({nb.position.x + nb.size.x / 2.f, nb.position.y + nb.size.y});
        nameText.setPosition({npc.pos.x, npc.pos.y - BODY_H - HEAD_R * 2.f - 8.f + bob});
        window.draw(nameText);

        // Speech bubble — only when player is nearby and there's something to say
        if (!npc.playerNearby || npc.lines.empty()) continue;

        const std::string& fullLine = npc.lines[npc.lineIndex];
        std::string visible = fullLine.substr(0, npc.revealChars);

        // Measure text to size bubble
        sf::Text bubbleText(font, visible, 14);
        bubbleText.setFillColor(sf::Color(240, 235, 220));
        auto tb = bubbleText.getLocalBounds();
        float tw = std::max(tb.size.x, 10.f);
        float th = tb.size.y;

        float padX = 10.f, padY = 7.f;
        float bw = tw + padX * 2.f;
        float bh = th + padY * 2.f;
        float bx = npc.pos.x - bw / 2.f;
        float by = npc.pos.y - BODY_H - HEAD_R * 2.f - 18.f - bh + bob;

        // Bubble background
        sf::RectangleShape bubble({bw, bh});
        bubble.setPosition({bx, by});
        bubble.setFillColor(sf::Color(20, 12, 35, 230));
        bubble.setOutlineColor(npc.headColor);
        bubble.setOutlineThickness(1.5f);
        window.draw(bubble);

        // Tail (small triangle pointing down toward head)
        sf::ConvexShape tail;
        tail.setPointCount(3);
        tail.setPoint(0, {npc.pos.x - 6.f, by + bh});
        tail.setPoint(1, {npc.pos.x + 6.f, by + bh});
        tail.setPoint(2, {npc.pos.x, by + bh + 8.f});
        tail.setFillColor(sf::Color(20, 12, 35, 230));
        window.draw(tail);

        // Tail border lines
        sf::Vertex tailBorder[] = {
            sf::Vertex{{npc.pos.x - 6.f, by + bh}, npc.headColor},
            sf::Vertex{{npc.pos.x, by + bh + 8.f}, npc.headColor},
            sf::Vertex{{npc.pos.x + 6.f, by + bh}, npc.headColor},
        };
        window.draw(tailBorder, 3, sf::PrimitiveType::LineStrip);

        // Text inside bubble
        bubbleText.setPosition({bx + padX, by + padY});
        window.draw(bubbleText);

        // Blinking cursor while typing
        if (npc.revealChars < (int)fullLine.size()) {
            float blink = std::fmod(m_bobClock.getElapsedTime().asSeconds(), 0.6f);
            if (blink < 0.3f) {
                sf::Text cursor(font, "|", 14);
                cursor.setFillColor(
                    sf::Color(npc.headColor.r, npc.headColor.g, npc.headColor.b, 200));
                cursor.setPosition({bx + padX + tw + 2.f, by + padY});
                window.draw(cursor);
            }
        }
    }
}