#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class DisappearState {
    Visible,     // solid, can be stood on
    FadingOut,   // becoming transparent (not yet gone)
    Hidden,      // invisible, no collision
    FadingIn     // reappearing
};

struct DisappearingPlatform {
    sf::RectangleShape shape;
    sf::Color          baseColor;
    DisappearState     state     = DisappearState::Visible;
    float              timer     = 0.f;
    float              visibleTime;   // how long it stays solid
    float              hiddenTime;    // how long it stays gone
    float              fadeTime;      // fade in/out duration
    float              phaseOffset;  // stagger so not all blink at once

    static constexpr float DEFAULT_VISIBLE = 2.0f;
    static constexpr float DEFAULT_HIDDEN  = 1.5f;
    static constexpr float DEFAULT_FADE    = 0.4f;

    DisappearingPlatform(float x, float y, float w, float h,
                         sf::Color col         = sf::Color(140, 80, 200),
                         float visTime         = DEFAULT_VISIBLE,
                         float hidTime         = DEFAULT_HIDDEN,
                         float fade            = DEFAULT_FADE,
                         float offset          = 0.f);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    bool isSolid() const { return state == DisappearState::Visible ||
                                  state == DisappearState::FadingOut; }
    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
};

class DisappearingPlatformManager {
public:
    // offset staggers start phase so platforms don't all vanish simultaneously
    void add(float x, float y, float w, float h,
             sf::Color col    = sf::Color(140, 80, 200),
             float visTime    = DisappearingPlatform::DEFAULT_VISIBLE,
             float hidTime    = DisappearingPlatform::DEFAULT_HIDDEN,
             float fade       = DisappearingPlatform::DEFAULT_FADE,
             float offset     = 0.f);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    void resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel,
                       sf::FloatRect playerRect, bool& onGround,
                       bool& dashAvail, int& jumpsLeft);

private:
    std::vector<DisappearingPlatform> m_platforms;
};