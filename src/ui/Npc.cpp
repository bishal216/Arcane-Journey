#include "ui/Npc.hpp"

#include <cmath>

#include "Constants.hpp"

static constexpr float BODY_W = 24.f;
static constexpr float BODY_H = 36.f;
static constexpr float HEAD_R = 14.f;
static constexpr float BOB_AMP = 4.f;  // gentle floating bob
static constexpr float BOB_SPD = 1.8f;

void NpcManager::addNpc(float x, float floorY, NpcType type, const std::string& name,
                        sf::Color bodyCol, sf::Color headCol) {
    Npc npc;
    npc.type = type;
    npc.name = name;
    npc.pos = {x, floorY};
    npc.bodyColor = bodyCol;
    npc.headColor = headCol;

    // Body
    npc.body.setSize({BODY_W, BODY_H});
    npc.body.setOrigin({BODY_W / 2.f, BODY_H});
    npc.body.setPosition({x, floorY});
    npc.body.setFillColor(bodyCol);
    npc.body.setOutlineColor(sf::Color(255, 255, 255, 60));
    npc.body.setOutlineThickness(1.f);

    // Head
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
    float floorY = worldH - TILEF;  // one tile above absolute bottom

    // Five NPCs spread across the hub width
    addNpc(120.f, floorY, NpcType::Shop, "Merchant", sf::Color(200, 160, 40),
           sf::Color(230, 190, 80));
    addNpc(280.f, floorY, NpcType::Cosmetics, "Tailor", sf::Color(160, 60, 200),
           sf::Color(200, 120, 240));
    addNpc(512.f, floorY, NpcType::TimeKeeper, "Chronicler", sf::Color(40, 160, 200),
           sf::Color(80, 200, 240));
    addNpc(740.f, floorY, NpcType::Lore, "Seer", sf::Color(180, 60, 60), sf::Color(220, 100, 100));
    addNpc(900.f, floorY, NpcType::WiseMan, "Sage", sf::Color(60, 160, 80),
           sf::Color(100, 210, 130));
}

int NpcManager::getNearbyNpc(sf::Vector2f playerPos) const {
    for (int i = 0; i < (int)m_npcs.size(); ++i) {
        float dx = playerPos.x - m_npcs[i].pos.x;
        float dy = playerPos.y - m_npcs[i].pos.y;
        if (std::sqrt(dx * dx + dy * dy) < m_npcs[i].interactRadius) return i;
    }
    return -1;
}

void NpcManager::draw(sf::RenderWindow& window, const sf::Font& font) const {
    float t = m_bobClock.getElapsedTime().asSeconds();

    for (const auto& npc : m_npcs) {
        float bob = std::sin(t * BOB_SPD + npc.pos.x * 0.01f) * BOB_AMP;

        // Draw body and head with bob offset
        sf::RectangleShape body = npc.body;
        sf::CircleShape head = npc.head;
        body.move({0.f, bob});
        head.move({0.f, bob});
        window.draw(body);
        window.draw(head);

        // Eyes (two small dots on head)
        sf::CircleShape eye(2.5f);
        eye.setFillColor(sf::Color(20, 10, 30));
        eye.setOrigin({2.5f, 2.5f});
        eye.setPosition({npc.pos.x - 5.f, npc.pos.y - BODY_H - HEAD_R * 0.6f + bob});
        window.draw(eye);
        eye.setPosition({npc.pos.x + 5.f, npc.pos.y - BODY_H - HEAD_R * 0.6f + bob});
        window.draw(eye);

        // Name tag above head
        sf::Text nameText(font, npc.name, 13);
        nameText.setFillColor(sf::Color(240, 240, 240));
        nameText.setOutlineColor(sf::Color::Black);
        nameText.setOutlineThickness(1.f);
        auto nb = nameText.getLocalBounds();
        nameText.setOrigin({nb.position.x + nb.size.x / 2.f, nb.position.y + nb.size.y});
        nameText.setPosition({npc.pos.x, npc.pos.y - BODY_H - HEAD_R * 2.f - 8.f + bob});
        window.draw(nameText);
    }
}
