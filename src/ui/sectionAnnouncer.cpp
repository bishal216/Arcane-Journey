#include "ui/SectionAnnouncer.hpp"
#include "Constants.hpp"
#include <algorithm>

SectionAnnouncer::SectionAnnouncer(const sf::Font& font)
    : m_text(font, "", 36)
{
    m_text.setOutlineColor(sf::Color::Black);
    m_text.setOutlineThickness(2.5f);
    m_total = m_fadeIn + m_hold + m_fadeOut;
}

void SectionAnnouncer::trigger(const std::string& name) {
    m_text.setString(name);
    // Centre horizontally
    auto bounds = m_text.getLocalBounds();
    m_text.setOrigin({bounds.position.x + bounds.size.x / 2.f,
                      bounds.position.y + bounds.size.y / 2.f});
    m_text.setPosition({(float)WIN_W / 2.f, (float)WIN_H * 0.35f});
    m_timer  = 0.f;
    m_active = true;
}

void SectionAnnouncer::update(float dt) {
    if (!m_active) return;
    m_timer += dt;
    if (m_timer >= m_total) { m_active = false; return; }

    // Compute alpha
    float alpha = 1.f;
    if (m_timer < m_fadeIn)
        alpha = m_timer / m_fadeIn;
    else if (m_timer > m_fadeIn + m_hold)
        alpha = 1.f - (m_timer - m_fadeIn - m_hold) / m_fadeOut;

    uint8_t a = (uint8_t)(std::clamp(alpha, 0.f, 1.f) * 255.f);
    m_text.setFillColor(sf::Color(255, 220, 120, a));
    m_text.setOutlineColor(sf::Color(0, 0, 0, a));
}

void SectionAnnouncer::draw(sf::RenderWindow& window) const {
    if (m_active) window.draw(m_text);
}

void SectionAnnouncer::reset() {
    m_active      = false;
    m_timer       = 0.f;
    m_lastSection = -1;
    std::fill(m_seen.begin(), m_seen.end(), false);
}

bool SectionAnnouncer::hasSeen(int i) const {
    if (i < 0 || i >= (int)m_seen.size()) return false;
    return m_seen[i];
}

void SectionAnnouncer::markSeen(int i) {
    if (i >= 0 && i < (int)m_seen.size()) m_seen[i] = true;
}

void SectionAnnouncer::resetSeen(int count) {
    m_seen.assign(count, false);
}