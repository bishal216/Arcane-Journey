#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "systems/DiscoveryTracker.hpp"
#include "systems/ArtifactManager.hpp"

// ---------------------------------------------------------------------------
// FanPlatform — constant upward force applied while player is inside bounds
// Creates updrafts that let player hover or gain height
// ---------------------------------------------------------------------------

struct FanPlatform {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Color baseColor;
    float strength;      // px/s² upward acceleration
    float particleTimer = 0.f;

    static constexpr float MAX_STRENGTH = 1200.f;

    FanPlatform(float x, float y, float w, float h, float str = 600.f,
                sf::Color col = sf::Color(SKY.r, SKY.g, SKY.b, 160))
        : origin({x, y}), baseColor(col), strength(std::clamp(str, 0.f, MAX_STRENGTH)) {
        shape.setSize({w, h});
        shape.setPosition({x, y});
        shape.setFillColor(col);
        shape.setOutlineColor(sf::Color(SKY.r, SKY.g, SKY.b, 100));
        shape.setOutlineThickness(1.f);
    }

    void update(float dt) {
        particleTimer += dt;
        // Gentle pulsing
        uint8_t a = (uint8_t)(baseColor.a * (0.6f + 0.4f * std::sin(particleTimer * 3.f)));
        shape.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, a));
    }

    void draw(sf::RenderWindow& window) const { window.draw(shape); }

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
    bool contains(const sf::FloatRect& rect) const { return bounds().findIntersection(rect); }

    // Apply upward force to player velocity
    void applyForce(sf::Vector2f& playerVel, float dt) const {
        playerVel.y -= strength * dt;  // Negative = upward
    }
};

class FanPlatformManager {
public:
    void add(float x, float y, float w, float h, float strength = 600.f,
             sf::Color col = sf::Color(SKY.r, SKY.g, SKY.b, 160));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void reset();

    // Call from LevelData::resolvePlayer — applies force if player overlaps
    void applyFan(sf::Vector2f& playerVel, const sf::FloatRect& playerRect, float dt);

    const std::vector<FanPlatform>& platforms() const { return m_platforms; }

private:
    std::vector<FanPlatform> m_platforms;
};

extern FanPlatformManager g_fanPlatforms;