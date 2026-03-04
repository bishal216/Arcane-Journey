#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct Cosmetic {
    std::string name;
    std::string category;   // "color", "dash", "skin"
    int         cost;
    bool        unlocked = false;
    bool        equipped = false;
    sf::Color   previewColor = sf::Color::White;
};

class CosmeticsManager {
public:
    CosmeticsManager();

    bool tryUnlock(int index, int& playerCoins);
    void equip(int index);

    const std::vector<Cosmetic>& items() const { return m_items; }

    // Returns the color of the currently equipped "color" cosmetic,
    // or the default player color if none equipped
    sf::Color activePlayerColor() const;
    sf::Color activeDashColor()   const;

private:
    std::vector<Cosmetic> m_items;
};