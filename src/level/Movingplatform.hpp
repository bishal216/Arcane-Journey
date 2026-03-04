#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct MovingPlatform {
    sf::RectangleShape shape;

    sf::Vector2f pointA;    // one end of travel
    sf::Vector2f pointB;    // other end of travel
    float        speed;
    sf::Vector2f velocity;  // current velocity (derived, used by player carry)
    sf::Vector2f prevPos;   // position before this frame's move — used by carry fix

    // Internal
    sf::Vector2f m_dir;     // normalised direction toward current target
    bool         m_towardB = true;
    bool isVertical = false;   // true if primary movement axis is Y

    MovingPlatform(float x, float y, float w, float h,
                   sf::Vector2f a, sf::Vector2f b,
                   float spd,
                   sf::Color col = sf::Color(80, 160, 220));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
};

class MovingPlatformManager {
public:
    void add(float x, float y, float w, float h,
             sf::Vector2f a, sf::Vector2f b,
             float speed,
             sf::Color col = sf::Color(80, 160, 220));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    // Resolve player collision. Carries player if standing on top.
    // Returns true if player is on any moving platform this frame.
    bool resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel,
                       sf::FloatRect playerRect, bool& onGround, float dt);

    const std::vector<MovingPlatform>& platforms() const { return m_platforms; }

private:
    std::vector<MovingPlatform> m_platforms;
};