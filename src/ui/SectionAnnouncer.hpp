#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class SectionAnnouncer {
public:
    SectionAnnouncer(const sf::Font& font);

    void trigger(const std::string& name);  // call when entering a new section
    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void reset();

private:
    sf::Text  m_text;
    float     m_timer    = 0.f;
    float     m_fadeIn   = 0.8f;
    float     m_hold     = 1.4f;
    float     m_fadeOut  = 0.8f;
    float     m_total    = 0.f;  // fadeIn + hold + fadeOut
    bool      m_active   = false;

    std::vector<bool> m_seen; // track which sections have been shown this run
    int m_lastSection = -1;

public:
    int  lastSection() const { return m_lastSection; }
    void setLastSection(int i) { m_lastSection = i; }
    bool hasSeen(int i) const;
    void markSeen(int i);
    void resetSeen(int count);
};