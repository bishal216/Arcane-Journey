#pragma once
#include <SFML/Graphics.hpp>

class Camera {
public:
    Camera();
    void setWorldH(float h) { m_worldH = h; }
    void setWorldW(float w) { m_worldW = w; }
    void update(sf::Vector2f target);
    void apply(sf::RenderWindow& window) const;
    void applyDefault(sf::RenderWindow& window) const;
    float top() const;

private:
    sf::View m_view;
    float    m_worldH = 2704.f;
    float    m_worldW = 1024.f;
};