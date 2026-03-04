#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "CoinManager.hpp"
#include "CosmeticsManager.hpp"

enum class HubPanel { None, Shop, Cosmetics, TimeKeeper, Lore, WiseMan };

class HubUI {
public:
    HubUI(const sf::Font& font, CoinManager& coins, CosmeticsManager& cosmetics);

    void open(HubPanel panel);
    void close();
    bool     isOpen()        const { return m_panel != HubPanel::None; }
    HubPanel currentPanel()  const { return m_panel; }

    void handleInput(sf::Keyboard::Key key);
    void draw(sf::RenderWindow& window);

    void  setBestTime(float s) { if (m_bestTime < 0.f || s < m_bestTime) m_bestTime = s; }
    float bestTime()     const { return m_bestTime; }

private:
    const sf::Font&   m_font;
    CoinManager&      m_coins;
    CosmeticsManager& m_cosmetics;
    HubPanel          m_panel  = HubPanel::None;
    int               m_cursor = 0;
    float             m_bestTime = -1.f;

    // Lore pages — one per tarot section
    static const std::vector<std::pair<std::string,std::string>> LORE;
    // WiseMan entries — one per platform type
    static const std::vector<std::pair<std::string,std::string>> WISDOM;

    void drawBackground (sf::RenderWindow& w, const std::string& title);
    void drawShop       (sf::RenderWindow& w);
    void drawCosmetics  (sf::RenderWindow& w);
    void drawTimeKeeper (sf::RenderWindow& w);
    void drawLore       (sf::RenderWindow& w);
    void drawWiseMan    (sf::RenderWindow& w);

    sf::Text makeText(const std::string& str, unsigned size,
                      sf::Color col = sf::Color::White);
};