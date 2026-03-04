#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "core/Player.hpp"

struct BouncePad {
    sf::RectangleShape shape;
    sf::Vector2f       origin;
    sf::Vector2f       size;
    float              bounceForce;

    // Squash animation
    float animTimer  = 0.f;
    bool  animActive = false;
    static constexpr float ANIM_DURATION = 0.3f;

    BouncePad(float x, float y, float w, float h,
              float force = 900.f,
              sf::Color col = sf::Color(80, 220, 120));

    void trigger();
    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
};

class BouncePadManager {
public:
    void add(float x, float y, float w, float h,
             float force = 900.f,
             sf::Color col = sf::Color(80, 220, 120));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    // Launches player upward if they land on a bounce pad.
    // Does NOT cost a jump or dash — it's a free launch.
    void resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel,
                       sf::FloatRect playerRect, bool& onGround);

private:
    std::vector<BouncePad> m_pads;
};