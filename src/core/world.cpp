#include <algorithm>
#include "core/World.hpp"

void World::addPlat(float x, float y, float w, float h, sf::Color col, bool goal) {
    Platform p;
    p.shape.setSize({w, h});
    p.shape.setPosition({x, y});
    p.shape.setFillColor(col);
    p.shape.setOutlineColor(sf::Color(255, 255, 255, 30));
    p.shape.setOutlineThickness(1.f);
    p.isGoal = goal;
    m_platforms.push_back(p);
}

void World::addCoin(float cx, float cy) {
    m_coinSpawns.push_back({cx, cy});
}

void World::addSection(float worldY, const std::string& name) {
    m_sections.push_back({worldY, name});
}

void World::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_platforms)
        window.draw(p.shape);
}

int World::getSectionIndex(float worldY) const {
    int idx = -1;
    for (int i = 0; i < (int)m_sections.size(); ++i)
        if (worldY <= m_sections[i].startY)
            idx = i;
    return idx;
}

void World::sortSections() {
    std::sort(m_sections.begin(), m_sections.end(),
              [](const Section& a, const Section& b){ return a.startY > b.startY; });
}