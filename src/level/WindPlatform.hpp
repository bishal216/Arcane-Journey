#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "systems/DiscoveryTracker.hpp"
#include "systems/ArtifactManager.hpp"

// ---------------------------------------------------------------------------
// WindPlatform — constant horizontal force applied while player is inside bounds
// Direction: +1 = right, -1 = left
// ---------------------------------------------------------------------------

struct WindPlatform {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Color baseColor;
    float strength;      // px/s² acceleration applied to player
    int direction;       // +1 = right, -1 = left
    float pulseTimer = 0.f;

    static constexpr float VISUAL_W = 48.f;  // visual width indicator
    static constexpr float MAX_STRENGTH = 800.f;

    WindPlatform(float x, float y, float w, float h, int dir, float str = 400.f,
                 sf::Color col = sf::Color(SKY.r, SKY.g, SKY.b, 180))
        : origin({x, y}), baseColor(col), strength(std::clamp(str, 0.f, MAX_STRENGTH)), direction(dir) {
        shape.setSize({w, h});
        shape.setPosition({x, y});
        shape.setFillColor(col);
        shape.setOutlineColor(sf::Color(SKY.r, SKY.g, SKY.b, 120));
        shape.setOutlineThickness(1.f);
    }

    void update(float dt) {
        pulseTimer += dt;
        // Pulsing alpha to show active wind
        uint8_t a = (uint8_t)(baseColor.a * (0.7f + 0.3f * std::sin(pulseTimer * 4.f)));
        shape.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, a));
    }

    void draw(sf::RenderWindow& window) const { window.draw(shape); }

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
    bool contains(const sf::FloatRect& rect) const { return bounds().findIntersection(rect); }

    // Apply wind force to player velocity
    void applyForce(sf::Vector2f& playerVel, float dt) const {
        playerVel.x += direction * strength * dt;
    }
};

class WindPlatformManager {
public:
    // dir: +1 = right, -1 = left
    void add(float x, float y, float w, float h, int dir, float strength = 400.f,
             sf::Color col = sf::Color(SKY.r, SKY.g, SKY.b, 180));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void reset();

    // Call from LevelData::resolvePlayer — applies force if player overlaps
    void applyWind(sf::Vector2f& playerVel, const sf::FloatRect& playerRect, float dt);

    const std::vector<WindPlatform>& platforms() const { return m_platforms; }

private:
    std::vector<WindPlatform> m_platforms;
};

extern WindPlatformManager g_windPlatforms;