#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "core/Player.hpp"

// ---------------------------------------------------------------------------
// SinkingPlatform — sinks slowly under the player's weight, then floats
// back to its origin after the player steps off.
//
// Behaviour timeline:
//   Idle      → player lands → Sinking (descends at SINK_SPEED px/s)
//   Sinking   → player steps off OR reaches MAX_SINK → Rising
//   Rising    → ascends at RISE_SPEED px/s back to origin → Idle
// ---------------------------------------------------------------------------

enum class SinkState { Idle, Sinking, Rising };

struct SinkingPlatform {
    sf::RectangleShape shape;
    sf::Vector2f origin;  // spawn position (reset target)
    sf::Color baseColor;
    SinkState state = SinkState::Idle;
    float sinkOffset = 0.f;  // current Y offset from origin (positive = down)

    // Tunables
    static constexpr float SINK_SPEED = 55.f;  // px/s while sinking
    static constexpr float RISE_SPEED = 90.f;  // px/s while rising
    static constexpr float MAX_SINK = 80.f;    // px before forced return

    SinkingPlatform(float x, float y, float w, float h, sf::Color col);

    void update(float dt, bool playerOnTop);
    void draw(sf::RenderWindow& window) const;

    bool isSolid() const { return true; }
    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }

   private:
    void syncShape();
};

// ---------------------------------------------------------------------------
class SinkingPlatformManager {
   public:
    void add(float x, float y, float w, float h, sf::Color col);

    void update(float dt);  // no-op; real update driven by resolvePlayer
    void draw(sf::RenderWindow& window) const;
    void reset();

    // Returns true if player landed on a sinking platform this frame.
    bool resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                       bool& onGround, bool& dashAvail, int& jumpsLeft, float dt);

   private:
    std::vector<SinkingPlatform> m_platforms;
};