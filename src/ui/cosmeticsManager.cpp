#include "CosmeticsManager.hpp"

CosmeticsManager::CosmeticsManager() {
    // Player colors
    m_items.push_back({"Crimson",     "color", 10, false, false, sf::Color(220,  60,  60)});
    m_items.push_back({"Emerald",     "color", 10, false, false, sf::Color( 60, 200, 100)});
    m_items.push_back({"Gold",        "color", 15, false, false, sf::Color(255, 200,  40)});
    m_items.push_back({"Void",        "color", 20, false, false, sf::Color( 80,  40, 160)});
    m_items.push_back({"Ghost",       "color", 25, false, false, sf::Color(200, 200, 220)});

    // Dash effects
    m_items.push_back({"Flame Trail", "dash",  20, false, false, sf::Color(255, 120,  30)});
    m_items.push_back({"Ice Burst",   "dash",  20, false, false, sf::Color(100, 200, 255)});
    m_items.push_back({"Shadow Step", "dash",  30, false, false, sf::Color( 60,  20,  80)});

    // Skins
    m_items.push_back({"The Fool",    "skin",  15, false, false, sf::Color(255, 240, 180)});
    m_items.push_back({"The Tower",   "skin",  30, false, false, sf::Color(140, 100,  60)});
    m_items.push_back({"The Star",    "skin",  40, false, false, sf::Color(180, 180, 255)});
    m_items.push_back({"The World",   "skin",  60, false, false, sf::Color(100, 220, 180)});
}

bool CosmeticsManager::tryUnlock(int index, int& playerCoins) {
    if (index < 0 || index >= (int)m_items.size()) return false;
    auto& item = m_items[index];
    if (item.unlocked) return false;
    if (playerCoins < item.cost) return false;
    playerCoins -= item.cost;
    item.unlocked = true;
    return true;
}

void CosmeticsManager::equip(int index) {
    if (index < 0 || index >= (int)m_items.size()) return;
    if (!m_items[index].unlocked) return;
    std::string cat = m_items[index].category;
    // Unequip others in same category
    for (auto& item : m_items)
        if (item.category == cat) item.equipped = false;
    m_items[index].equipped = true;
}

sf::Color CosmeticsManager::activePlayerColor() const {
    for (const auto& item : m_items)
        if (item.category == "color" && item.equipped)
            return item.previewColor;
    return sf::Color(100, 180, 255); // default blue
}

sf::Color CosmeticsManager::activeDashColor() const {
    for (const auto& item : m_items)
        if (item.category == "dash" && item.equipped)
            return item.previewColor;
    return sf::Color(255, 120, 50); // default orange
}