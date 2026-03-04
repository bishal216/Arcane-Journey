#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include "systems/DiscoveryTracker.hpp"

// A scripted keyframe animation shown in the Sage panel preview.
// Each demo is a sequence of keyframes that move a character shape
// and optionally animate a platform shape.

struct DemoKeyframe {
    float duration;           // seconds to interpolate to this keyframe
    sf::Vector2f charPos;     // character centre position (in demo-local coords)
    sf::Vector2f platPos;     // platform position (demo-local)
    sf::Vector2f platSize;    // platform size
    sf::Color    platColor;
    sf::Color    charColor;
    std::string  label;       // shown during this keyframe (empty = keep previous)
};

class PlatformDemo {
public:
    // Panel origin + size in screen coords (set by HubUI before drawing)
    void setArea(sf::Vector2f origin, sf::Vector2f size);

    // Build demo for a given type.  Resets and starts playback.
    void play(PlatType type);

    void update(float dt);
    void draw(sf::RenderWindow& window, const sf::Font& font) const;

    bool isPlaying() const { return m_playing; }

private:
    sf::Vector2f m_origin;
    sf::Vector2f m_size;

    std::vector<DemoKeyframe> m_frames;
    int    m_frameIdx   = 0;
    float  m_frameTimer = 0.f;
    bool   m_playing    = false;
    std::string m_currentLabel;

    // Interpolated state
    sf::Vector2f m_charPos;
    sf::Vector2f m_platPos;
    sf::Vector2f m_platSize;
    sf::Color    m_platColor;
    sf::Color    m_charColor;

    // Extra shapes for complex demos (rope, ladder segments, coin, etc.)
    std::vector<sf::RectangleShape> m_extras;

    void buildFrames(PlatType type);
    void applyFrame(const DemoKeyframe& f);
    void lerpFrame(const DemoKeyframe& from, const DemoKeyframe& to, float t);

    static sf::Color lerpColor(sf::Color a, sf::Color b, float t);
    static sf::Vector2f lerpVec(sf::Vector2f a, sf::Vector2f b, float t);
};