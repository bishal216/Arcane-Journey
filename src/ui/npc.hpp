#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class NpcType { Shop, Cosmetics, TimeKeeper, Lore, WiseMan };

struct Npc {
    NpcType      type;
    std::string  name;
    sf::Vector2f pos;       // feet position
    sf::Color    bodyColor;
    sf::Color    headColor;

    // Drawn shapes (built once in constructor)
    sf::RectangleShape body;
    sf::CircleShape    head;
    sf::RectangleShape nameTag;

    float interactRadius = 80.f;
};

class NpcManager {
public:
    void build(float worldH);   // call after TileLoader::load

    // Returns type of NPC player is near, or -1
    int  getNearbyNpc(sf::Vector2f playerPos) const;
    NpcType typeAt(int index) const { return m_npcs[index].type; }
    std::string nameAt(int index) const { return m_npcs[index].name; }

    void draw(sf::RenderWindow& window, const sf::Font& font) const;

private:
    std::vector<Npc> m_npcs;
    sf::Clock        m_bobClock;
    const sf::Font*  m_font = nullptr;

    void addNpc(float x, float floorY, NpcType type,
                const std::string& name,
                sf::Color bodyCol, sf::Color headCol);
};