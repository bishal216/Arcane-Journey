#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct Platform {
    sf::RectangleShape shape;
    bool isGoal = false;
};

struct Section {
    float       startY;
    std::string name;
};

struct CoinSpawn {
    float cx, cy;
};

// World is now a pure data container — no hardcoded geometry.
// All platforms, sections, and coins are populated by TileLoader.
class World {
public:
    void draw(sf::RenderWindow& window) const;

    void addPlat(float x, float y, float w, float h,
                 sf::Color col = sf::Color(80, 190, 80), bool goal = false);
    void addCoin   (float cx, float cy);
    void addSection(float worldY, const std::string& name);
    void sortSections();

    const std::vector<Platform>&  platforms()  const { return m_platforms; }
    const std::vector<Section>&   sections()   const { return m_sections; }
    const std::vector<CoinSpawn>& coinSpawns() const { return m_coinSpawns; }
    int getSectionIndex(float worldY) const;

private:
    std::vector<Platform>  m_platforms;
    std::vector<Section>   m_sections;
    std::vector<CoinSpawn> m_coinSpawns;
};