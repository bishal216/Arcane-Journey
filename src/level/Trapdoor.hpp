#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// One-way platform / trapdoor / hatch.
// - Player can jump UP through it (no collision from below)
// - Player lands ON it when falling down (collision from above only)
// - Optional hatch variant: swings open briefly after landing,
//   then snaps shut — player must move off before it closes.

enum class TrapdoorStyle {
    OneWay,  // silent, always-passable from below (classic one-way platform)
    Hatch    // visible door that swings open after landing, closes again
};

enum class HatchState {
    Closed,
    Opening,  // animating open
    Open,     // passable in both directions briefly
    Closing   // animating shut
};

struct Trapdoor {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Color baseColor;
    TrapdoorStyle style;

    // Hatch animation
    HatchState hatchState = HatchState::Closed;
    float hatchTimer = 0.f;
    float openAngle = 0.f;  // degrees, 0=closed 90=open

    static constexpr float OPEN_TIME = 0.18f;   // seconds to swing open
    static constexpr float STAY_TIME = 0.55f;   // seconds it stays open
    static constexpr float CLOSE_TIME = 0.25f;  // seconds to swing closed

    Trapdoor(float x, float y, float w, float h, TrapdoorStyle s, sf::Color col);

    // Call when player lands on top
    void onLand();

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    // True when solid from above (blocks downward passage)
    bool isSolidFromAbove() const;

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
};

class TrapdoorManager {
   public:
    void addOneWay(float x, float y, float w, float h, sf::Color col = sf::Color(120, 80, 40));
    void addHatch(float x, float y, float w, float h, sf::Color col = sf::Color(100, 70, 35));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void reset();  // close all hatches: only blocks player falling downward.
    // Returns true if player landed on something this frame.
    bool resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                       bool& onGround, bool& dashAvail, int& jumpsLeft);

   private:
    std::vector<Trapdoor> m_trapdoors;
};