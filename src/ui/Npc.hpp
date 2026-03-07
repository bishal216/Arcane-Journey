#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class NpcType { Shop, Cosmetics, TimeKeeper, Lore, WiseMan };

struct Npc {
    NpcType type;
    std::string name;
    sf::Vector2f pos;
    sf::Color bodyColor;
    sf::Color headColor;

    sf::RectangleShape body;
    sf::CircleShape head;

    float interactRadius = 90.f;

    // Dialogue
    std::vector<std::string> lines;
    int lineIndex = 0;        // which line is showing
    float lineTimer = 0.f;    // time on current line
    float revealTimer = 0.f;  // typewriter progress
    int revealChars = 0;      // chars revealed so far
    bool playerNearby = false;
};

class NpcManager {
   public:
    void build(float worldH);

    int getNearbyNpc(sf::Vector2f playerPos) const;
    NpcType typeAt(int index) const { return m_npcs[index].type; }
    std::string nameAt(int index) const { return m_npcs[index].name; }

    void update(float dt, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window, const sf::Font& font) const;

   private:
    std::vector<Npc> m_npcs;
    sf::Clock m_bobClock;

    void addNpc(float x, float floorY, NpcType type, const std::string& name, sf::Color bodyCol,
                sf::Color headCol, std::vector<std::string> lines);
};