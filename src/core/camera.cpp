#include "core/Camera.hpp"
#include "Constants.hpp"
#include <algorithm>

Camera::Camera()
    : m_view(sf::FloatRect({0.f, 0.f}, {(float)WIN_W, (float)WIN_H}))
{}

void Camera::update(sf::Vector2f target) {
    // Horizontal: follow player but clamp to world width
    float camX = target.x - WIN_W * 0.5f;
    camX = std::clamp(camX, 0.f, std::max(0.f, m_worldW - (float)WIN_W));

    // Vertical: follow player but clamp to world height
    float camY = target.y - WIN_H * 0.5f;
    camY = std::clamp(camY, 0.f, std::max(0.f, m_worldH - (float)WIN_H));

    m_view.setCenter({camX + WIN_W / 2.f, camY + WIN_H / 2.f});
}

void Camera::apply(sf::RenderWindow& window) const {
    window.setView(m_view);
}

void Camera::applyDefault(sf::RenderWindow& window) const {
    window.setView(window.getDefaultView());
}

float Camera::top() const {
    return m_view.getCenter().y - WIN_H / 2.f;
}