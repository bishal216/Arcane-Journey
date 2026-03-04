#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class SplashState { FadeIn, Idle, FadeOut, Done };

class SplashScreen {
public:
    SplashScreen(const sf::Font& font);

    void handleInput(const sf::Event& ev);
    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    bool isDone() const { return m_state == SplashState::Done; }

private:
    // State
    SplashState m_state     = SplashState::FadeIn;
    float       m_timer     = 0.f;
    float       m_alpha     = 0.f;   // 0..1 overall fade
    float       m_pulse     = 0.f;   // prompt pulse timer
    bool        m_anyKey    = false;

    // Stars
    struct Star {
        sf::Vector2f pos;
        float        speed;
        float        size;
        float        brightness;
    };
    std::vector<Star> m_stars;

    // Floating platforms
    struct FloatPlat {
        sf::Vector2f origin;
        float        phase;
        float        speed;
        sf::Color    color;
        sf::Vector2f size;
    };
    std::vector<FloatPlat> m_plats;

    // Sigil points (built once)
    sf::VertexArray m_sigilLines;
    sf::CircleShape m_sigilRing;
    sf::CircleShape m_sigilInner;

    // Text
    sf::Text m_title;
    sf::Text m_subtitle;
    sf::Text m_prompt;

    // Overlay for fade in/out
    sf::RectangleShape m_overlay;

    static constexpr float FADEIN_TIME  = 2.2f;
    static constexpr float IDLE_TIME    = 99.f;  // waits for key
    static constexpr float FADEOUT_TIME = 0.8f;

    void buildSigil();
    void buildStars();
    void buildPlatforms();
    sf::Color withAlpha(sf::Color c, float a) const;
};