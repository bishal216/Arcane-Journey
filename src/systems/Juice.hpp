#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

// ---------------------------------------------------------------------------
// Particle — a single visual puff/spark/debris piece
// ---------------------------------------------------------------------------
struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Color    color;
    float        life;       // remaining seconds
    float        maxLife;
    float        size;
    float        sizeEnd;    // shrinks toward this
    float        gravity;    // pixels/s² applied to vel.y
};

// ---------------------------------------------------------------------------
// ScreenShake
// ---------------------------------------------------------------------------
struct ScreenShake {
    float trauma    = 0.f;   // 0..1, decays each frame
    float maxOffset = 8.f;
    sf::Vector2f offset() const;
    void add(float t) { trauma = std::min(trauma + t, 1.f); }
    void update(float dt) { trauma = std::max(0.f, trauma - dt * 3.5f); }
};

// ---------------------------------------------------------------------------
// Squash/Stretch — applied to player draw size
// ---------------------------------------------------------------------------
struct SquashStretch {
    float scaleX = 1.f;
    float scaleY = 1.f;
    float timer  = 0.f;

    void squash()  { scaleX = 1.4f; scaleY = 0.6f; timer = 0.12f; }
    void stretch() { scaleX = 0.7f; scaleY = 1.4f; timer = 0.10f; }
    void update(float dt);
};

// ---------------------------------------------------------------------------
// Afterimage — ghost trail for dash
// ---------------------------------------------------------------------------
struct Afterimage {
    sf::Vector2f pos;
    sf::Color    color;
    float        alpha;   // 0..1, fades to 0
    float        scaleX, scaleY;
};

// ---------------------------------------------------------------------------
// JuiceManager — global instance, call g_juice.onXxx() from managers
// ---------------------------------------------------------------------------
class JuiceManager {
public:
    // -- Events (call these from your existing managers) --
    void onLand      (sf::Vector2f pos, float speed);   // player landed
    void onJump      (sf::Vector2f pos);                // player jumped
    void onDash      (sf::Vector2f pos, sf::Vector2f dir, sf::Color col);
    void onWallSlide (sf::Vector2f pos, int wallDir);   // -1=left wall, 1=right
    void onBounce    (sf::Vector2f pos);                // bounce pad
    void onCrumble   (sf::Vector2f pos, sf::Color col); // crumbling platform
    void onTeleport  (sf::Vector2f pos);                // teleport flash
    void onCoinCollect(sf::Vector2f pos);
    void onGoal      (sf::Vector2f pos);                // win!

    // -- Per-frame --
    void update(float dt);
    void draw  (sf::RenderWindow& window) const;

    // -- Camera offset from shake --
    sf::Vector2f shakeOffset() const { return m_shake.offset(); }

    // -- Squash/stretch scale for player draw --
    float playerScaleX() const { return m_squash.scaleX; }
    float playerScaleY() const { return m_squash.scaleY; }

    // -- Afterimages (draw before player) --
    const std::vector<Afterimage>& afterimages() const { return m_afterimages; }

    // -- Teleport flash --
    float flashAlpha() const { return m_flashAlpha; }

private:
    std::vector<Particle>   m_particles;
    std::vector<Afterimage> m_afterimages;
    ScreenShake             m_shake;
    SquashStretch           m_squash;

    float m_flashAlpha   = 0.f;  // white screen flash (teleport/goal)
    float m_wallSlideTimer = 0.f;
    sf::Vector2f m_wallSlidePos;
    int   m_wallSlideDir = 0;

    // Timers for continuous effects
    float m_dashTrailTimer = 0.f;
    bool  m_dashing = false;
    sf::Vector2f m_dashPos;
    sf::Color    m_dashCol;

    void spawnDust  (sf::Vector2f pos, int count, sf::Color col, float spreadX, float spreadY);
    void spawnSparks(sf::Vector2f pos, int count, sf::Color col);
    void spawnDebris(sf::Vector2f pos, int count, sf::Color col);
};

extern JuiceManager g_juice;
