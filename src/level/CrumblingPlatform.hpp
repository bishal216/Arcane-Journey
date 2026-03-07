#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "core/Player.hpp"  // for GameState

enum class CrumbleState {
    Solid,      // normal, standable
    Shaking,    // crumbling soon (SHAKE_TIME seconds)
    Gone,       // broken, no collision (GONE_TIME seconds)
    Respawning  // fading back in, no collision (RESPAWN_TIME seconds)
};

struct CrumblingPlatform {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Color baseColor;
    CrumbleState state = CrumbleState::Solid;
    float timer = 0.f;
    float shakeOffset = 0.f;  // horizontal shake in pixels

    static constexpr float SHAKE_TIME = 0.4f;
    static constexpr float GONE_TIME = 1.8f;
    static constexpr float RESPAWN_TIME = 0.4f;

    CrumblingPlatform(float x, float y, float w, float h, sf::Color col = sf::Color(180, 120, 60));

    void onLand();
    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    bool isSolid() const { return state == CrumbleState::Solid || state == CrumbleState::Shaking; }
    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
};

class CrumblingPlatformManager {
   public:
    void add(float x, float y, float w, float h, sf::Color col = sf::Color(180, 120, 60));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void reset();

    // Returns true if player is on a crumbling platform this frame.
    bool resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                       bool& onGround, bool& dashAvail, int& jumpsLeft, GameState& state);

   private:
    std::vector<CrumblingPlatform> m_platforms;
};
