#include "systems/Juice.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

JuiceManager g_juice;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static float randf(float lo, float hi) {
    return lo + (hi - lo) * (std::rand() % 1000) / 999.f;
}

static sf::Color withAlpha(sf::Color c, float a) {
    c.a = (uint8_t)(std::clamp(a, 0.f, 1.f) * 255.f);
    return c;
}

// ---------------------------------------------------------------------------
// ScreenShake
// ---------------------------------------------------------------------------

sf::Vector2f ScreenShake::offset() const {
    float t2 = trauma * trauma;
    return {
        maxOffset * t2 * randf(-1.f, 1.f),
        maxOffset * t2 * randf(-1.f, 1.f)
    };
}

// ---------------------------------------------------------------------------
// SquashStretch
// ---------------------------------------------------------------------------

void SquashStretch::update(float dt) {
    if (timer <= 0.f) {
        // Lerp back to 1,1
        scaleX += (1.f - scaleX) * dt * 14.f;
        scaleY += (1.f - scaleY) * dt * 14.f;
        if (std::abs(scaleX - 1.f) < 0.01f) scaleX = 1.f;
        if (std::abs(scaleY - 1.f) < 0.01f) scaleY = 1.f;
    } else {
        timer -= dt;
    }
}

// ---------------------------------------------------------------------------
// Particle spawners
// ---------------------------------------------------------------------------

void JuiceManager::spawnDust(sf::Vector2f pos, int count,
                              sf::Color col, float spreadX, float spreadY) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.pos     = pos;
        p.vel     = {randf(-spreadX, spreadX), randf(-spreadY, 0.f)};
        p.color   = col;
        p.life    = randf(0.15f, 0.35f);
        p.maxLife = p.life;
        p.size    = randf(3.f, 6.f);
        p.sizeEnd = 0.f;
        p.gravity = 60.f;
        m_particles.push_back(p);
    }
}

void JuiceManager::spawnSparks(sf::Vector2f pos, int count, sf::Color col) {
    for (int i = 0; i < count; ++i) {
        float angle = randf(0.f, 6.28f);
        float spd   = randf(40.f, 140.f);
        Particle p;
        p.pos     = pos;
        p.vel     = {std::cos(angle) * spd, std::sin(angle) * spd};
        p.color   = col;
        p.life    = randf(0.2f, 0.5f);
        p.maxLife = p.life;
        p.size    = randf(2.f, 4.f);
        p.sizeEnd = 0.f;
        p.gravity = 120.f;
        m_particles.push_back(p);
    }
}

void JuiceManager::spawnDebris(sf::Vector2f pos, int count, sf::Color col) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.pos     = pos;
        p.vel     = {randf(-80.f, 80.f), randf(-120.f, -20.f)};
        p.color   = col;
        p.life    = randf(0.3f, 0.6f);
        p.maxLife = p.life;
        p.size    = randf(3.f, 7.f);
        p.sizeEnd = 1.f;
        p.gravity = 200.f;
        m_particles.push_back(p);
    }
}

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------

void JuiceManager::onLand(sf::Vector2f pos, float speed) {
    float intensity = std::clamp(std::abs(speed) / 600.f, 0.f, 1.f);
    if (intensity < 0.05f) return;

    m_squash.squash();
    m_shake.add(intensity * 0.25f);
    spawnDust(pos, (int)(4 + intensity * 8), sf::Color(200, 190, 170), 40.f, 20.f);
}

void JuiceManager::onJump(sf::Vector2f pos) {
    m_squash.stretch();
    spawnDust(pos, 5, sf::Color(180, 180, 160), 30.f, 10.f);
}

void JuiceManager::onDash(sf::Vector2f pos, sf::Vector2f dir, sf::Color col) {
    m_shake.add(0.15f);
    m_dashing    = true;
    m_dashPos    = pos;
    m_dashCol    = col;
    m_dashTrailTimer = 0.f;

    // Burst of sparks in dash direction
    for (int i = 0; i < 8; ++i) {
        Particle p;
        p.pos   = pos;
        float scatter = randf(-60.f, 60.f);
        p.vel   = {-dir.x * randf(60.f, 140.f) + scatter * std::abs(dir.y),
                   -dir.y * randf(60.f, 140.f) + scatter * std::abs(dir.x)};
        p.color   = col;
        p.life    = randf(0.1f, 0.25f);
        p.maxLife = p.life;
        p.size    = randf(3.f, 6.f);
        p.sizeEnd = 0.f;
        p.gravity = 0.f;
        m_particles.push_back(p);
    }
}

void JuiceManager::onWallSlide(sf::Vector2f pos, int wallDir) {
    m_wallSlidePos = pos;
    m_wallSlideDir = wallDir;
    m_wallSlideTimer = 0.05f;  // keeps emitting while held
}

void JuiceManager::onBounce(sf::Vector2f pos) {
    m_squash.stretch();
    m_shake.add(0.3f);
    spawnSparks(pos, 10, sf::Color(80, 220, 120));
    spawnDust(pos, 6, sf::Color(160, 240, 160), 50.f, 30.f);
}

void JuiceManager::onCrumble(sf::Vector2f pos, sf::Color col) {
    spawnDebris(pos, 8, col);
    m_shake.add(0.1f);
}

void JuiceManager::onTeleport(sf::Vector2f pos) {
    m_flashAlpha = 0.6f;
    spawnSparks(pos, 16, sf::Color(180, 120, 255));
    m_shake.add(0.2f);
}

void JuiceManager::onCoinCollect(sf::Vector2f pos) {
    spawnSparks(pos, 6, sf::Color(255, 210, 40));
}

void JuiceManager::onGoal(sf::Vector2f pos) {
    m_flashAlpha = 1.f;
    m_shake.add(0.5f);
    spawnSparks(pos, 30, sf::Color(255, 215, 0));
    spawnDebris(pos, 20, sf::Color(255, 200, 50));
}

// ---------------------------------------------------------------------------
// Update
// ---------------------------------------------------------------------------

void JuiceManager::update(float dt) {
    m_shake.update(dt);
    m_squash.update(dt);

    // Flash decay
    m_flashAlpha = std::max(0.f, m_flashAlpha - dt * 4.f);

    // Wall slide sparks
    if (m_wallSlideTimer > 0.f) {
        m_wallSlideTimer -= dt;
        if (std::rand() % 3 == 0) {
            Particle p;
            p.pos   = m_wallSlidePos;
            p.vel   = {(float)-m_wallSlideDir * randf(20.f, 60.f), randf(-20.f, 40.f)};
            p.color = sf::Color(220, 200, 140);
            p.life  = randf(0.1f, 0.2f);
            p.maxLife = p.life;
            p.size  = randf(2.f, 4.f);
            p.sizeEnd = 0.f;
            p.gravity = 80.f;
            m_particles.push_back(p);
        }
    }

    // Dash afterimage trail
    if (m_dashing) {
        m_dashTrailTimer -= dt;
        if (m_dashTrailTimer <= 0.f) {
            m_dashTrailTimer = 0.04f;
            Afterimage a;
            a.pos    = m_dashPos;
            a.color  = m_dashCol;
            a.alpha  = 0.5f;
            a.scaleX = m_squash.scaleX;
            a.scaleY = m_squash.scaleY;
            m_afterimages.push_back(a);
        }
    }
    m_dashing = false;  // reset — caller sets via onDash each frame while dashing

    // Fade afterimages
    for (auto& a : m_afterimages) a.alpha -= dt * 4.f;
    m_afterimages.erase(
        std::remove_if(m_afterimages.begin(), m_afterimages.end(),
                       [](const Afterimage& a){ return a.alpha <= 0.f; }),
        m_afterimages.end());

    // Update particles
    for (auto& p : m_particles) {
        p.life  -= dt;
        p.vel.y += p.gravity * dt;
        p.pos   += p.vel * dt;
    }
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
                       [](const Particle& p){ return p.life <= 0.f; }),
        m_particles.end());
}

// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------

void JuiceManager::draw(sf::RenderWindow& window) const {
    // Particles
    for (const auto& p : m_particles) {
        float t    = p.life / p.maxLife;
        float size = p.sizeEnd + (p.size - p.sizeEnd) * t;
        if (size < 0.5f) continue;

        sf::CircleShape c(size);
        c.setOrigin({size, size});
        c.setPosition(p.pos);
        c.setFillColor(withAlpha(p.color, t));
        window.draw(c);
    }

    // Flash overlay
    if (m_flashAlpha > 0.f) {
        auto sz = window.getSize();
        sf::RectangleShape flash({(float)sz.x, (float)sz.y});
        flash.setFillColor(withAlpha(sf::Color::White, m_flashAlpha));
        window.draw(flash);
    }
}
