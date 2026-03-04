#include "ui/SplashScreen.hpp"
#include "Constants.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

static float randf(float lo, float hi) {
    return lo + (hi - lo) * (std::rand() % 1000) / 999.f;
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

SplashScreen::SplashScreen(const sf::Font& font)
    : m_title (font, "ARCANE JOURNEY", 64)
    , m_subtitle(font, "A journey through the tarot", 22)
    , m_prompt(font, "Press any key to begin", 20)
{
    // Title
    m_title.setFillColor(sf::Color(220, 200, 255));
    m_title.setOutlineColor(sf::Color(80, 40, 140));
    m_title.setOutlineThickness(3.f);
    auto tb = m_title.getLocalBounds();
    m_title.setOrigin({tb.position.x + tb.size.x / 2.f,
                       tb.position.y + tb.size.y / 2.f});
    m_title.setPosition({WIN_W / 2.f, WIN_H / 2.f - 60.f});

    // Subtitle
    m_subtitle.setFillColor(sf::Color(160, 130, 210));
    m_subtitle.setOutlineColor(sf::Color(40, 20, 80));
    m_subtitle.setOutlineThickness(1.5f);
    auto sb = m_subtitle.getLocalBounds();
    m_subtitle.setOrigin({sb.position.x + sb.size.x / 2.f,
                          sb.position.y + sb.size.y / 2.f});
    m_subtitle.setPosition({WIN_W / 2.f, WIN_H / 2.f + 20.f});

    // Prompt
    m_prompt.setFillColor(sf::Color(200, 180, 240));
    m_prompt.setOutlineColor(sf::Color(40, 20, 80));
    m_prompt.setOutlineThickness(1.f);
    auto pb = m_prompt.getLocalBounds();
    m_prompt.setOrigin({pb.position.x + pb.size.x / 2.f,
                        pb.position.y + pb.size.y / 2.f});
    m_prompt.setPosition({WIN_W / 2.f, WIN_H / 2.f + 120.f});

    // Overlay
    m_overlay.setSize({(float)WIN_W, (float)WIN_H});
    m_overlay.setFillColor(sf::Color(0, 0, 0, 255));

    buildStars();
    buildPlatforms();
    buildSigil();
}

// ---------------------------------------------------------------------------
// Build helpers
// ---------------------------------------------------------------------------

void SplashScreen::buildStars() {
    std::srand(42);
    m_stars.resize(120);
    for (auto& s : m_stars) {
        s.pos        = {randf(0.f, (float)WIN_W), randf(0.f, (float)WIN_H)};
        s.speed      = randf(4.f, 18.f);
        s.size       = randf(1.f, 3.f);
        s.brightness = randf(0.4f, 1.f);
    }
}

void SplashScreen::buildPlatforms() {
    m_plats = {
        {{120.f, WIN_H / 2.f + 160.f}, 0.0f, 0.6f, sf::Color( 80, 190,  80), {96.f,  14.f}},
        {{780.f, WIN_H / 2.f + 140.f}, 1.2f, 0.8f, sf::Color(120, 210, 255), {80.f,  14.f}},
        {{ 60.f, WIN_H / 2.f + 200.f}, 2.5f, 0.5f, sf::Color(230, 210,  90), {64.f,  14.f}},
        {{860.f, WIN_H / 2.f + 180.f}, 0.8f, 0.7f, sf::Color(140,  80, 200), {72.f,  14.f}},
        {{300.f, WIN_H / 2.f + 220.f}, 3.1f, 0.4f, sf::Color( 80, 220, 120), {88.f,  14.f}},
        {{650.f, WIN_H / 2.f + 210.f}, 1.8f, 0.9f, sf::Color(160, 100,  50), {80.f,  14.f}},
    };
}

void SplashScreen::buildSigil() {
    float cx = WIN_W / 2.f;
    float cy = WIN_H / 2.f - 180.f;
    float r  = 48.f;

    // Outer ring
    m_sigilRing.setRadius(r);
    m_sigilRing.setOrigin({r, r});
    m_sigilRing.setPosition({cx, cy});
    m_sigilRing.setFillColor(sf::Color::Transparent);
    m_sigilRing.setOutlineColor(sf::Color(160, 100, 220));
    m_sigilRing.setOutlineThickness(1.5f);

    // Inner ring
    m_sigilInner.setRadius(r * 0.35f);
    m_sigilInner.setOrigin({r * 0.35f, r * 0.35f});
    m_sigilInner.setPosition({cx, cy});
    m_sigilInner.setFillColor(sf::Color(80, 40, 120, 120));
    m_sigilInner.setOutlineColor(sf::Color(200, 140, 255));
    m_sigilInner.setOutlineThickness(1.f);

    // Inscribed geometry — a pentagram-like set of lines connecting 7 points
    const int N = 7;
    std::vector<sf::Vector2f> pts(N);
    for (int i = 0; i < N; ++i) {
        float angle = i * 2.f * 3.14159f / N - 3.14159f / 2.f;
        pts[i] = {cx + r * std::cos(angle), cy + r * std::sin(angle)};
    }

    // Connect every point to every other for dense sigil look
    m_sigilLines = sf::VertexArray(sf::PrimitiveType::Lines);
    sf::Color lineCol(140, 80, 200, 180);
    // Skip adjacent connections (those are the ring) — connect skip-2 and skip-3
    for (int i = 0; i < N; ++i) {
        for (int skip : {2, 3}) {
            int j = (i + skip) % N;
            if (j > i) {  // avoid duplicates
                m_sigilLines.append({pts[i], lineCol});
                m_sigilLines.append({pts[j], lineCol});
            }
        }
    }

    // Add spokes to centre
    sf::Color spokeCol(120, 60, 180, 100);
    for (int i = 0; i < N; i += 2) {
        m_sigilLines.append({{cx, cy}, spokeCol});
        m_sigilLines.append({pts[i], spokeCol});
    }
}

// ---------------------------------------------------------------------------
// Runtime
// ---------------------------------------------------------------------------

sf::Color SplashScreen::withAlpha(sf::Color c, float a) const {
    c.a = (uint8_t)(std::clamp(a, 0.f, 1.f) * 255.f);
    return c;
}

void SplashScreen::handleInput(const sf::Event& ev) {
    if (m_state != SplashState::Idle) return;
    bool pressed = ev.is<sf::Event::KeyPressed>() ||
                   ev.is<sf::Event::MouseButtonPressed>();
    if (pressed) {
        m_anyKey = true;
        m_state  = SplashState::FadeOut;
        m_timer  = 0.f;
    }
}

void SplashScreen::update(float dt) {
    m_timer += dt;
    m_pulse += dt;

    // Drift stars upward, wrap at top
    for (auto& s : m_stars) {
        s.pos.y -= s.speed * dt;
        if (s.pos.y < -4.f)
            s.pos.y = (float)WIN_H + 2.f;
    }

    switch (m_state) {
        case SplashState::FadeIn:
            m_alpha = std::min(m_timer / FADEIN_TIME, 1.f);
            if (m_timer >= FADEIN_TIME) {
                m_state = SplashState::Idle;
                m_timer = 0.f;
                m_alpha = 1.f;
            }
            break;

        case SplashState::Idle:
            m_alpha = 1.f;
            break;

        case SplashState::FadeOut:
            m_alpha = 1.f - std::min(m_timer / FADEOUT_TIME, 1.f);
            if (m_timer >= FADEOUT_TIME)
                m_state = SplashState::Done;
            break;

        case SplashState::Done:
            break;
    }
}

void SplashScreen::draw(sf::RenderWindow& window) const {
    float a = m_alpha;

    // Background
    sf::RectangleShape bg({(float)WIN_W, (float)WIN_H});
    bg.setFillColor(sf::Color(12, 6, 28));
    window.draw(bg);

    // Stars
    for (const auto& s : m_stars) {
        uint8_t bright = (uint8_t)(s.brightness * a * 255.f);
        sf::CircleShape star(s.size);
        star.setFillColor(sf::Color(bright, bright, (uint8_t)(bright * 0.85f + 40)));
        star.setPosition(s.pos);
        window.draw(star);
    }

    // Floating platforms
    float t = m_pulse;
    for (const auto& p : m_plats) {
        float bob = std::sin(t * p.speed + p.phase) * 8.f;
        sf::RectangleShape plat(p.size);
        plat.setPosition({p.origin.x, p.origin.y + bob});
        sf::Color col = withAlpha(p.color, a * 0.7f);
        plat.setFillColor(col);
        plat.setOutlineColor(withAlpha(sf::Color::White, a * 0.15f));
        plat.setOutlineThickness(1.f);
        window.draw(plat);
    }

    // Sigil — tint by alpha
    auto drawSigil = [&]() {
        // Rings
        sf::CircleShape ring  = m_sigilRing;
        sf::CircleShape inner = m_sigilInner;
        ring.setOutlineColor(withAlpha(sf::Color(160, 100, 220), a));
        inner.setFillColor(withAlpha(sf::Color(80, 40, 120), a * 0.5f));
        inner.setOutlineColor(withAlpha(sf::Color(200, 140, 255), a));

        // Rotate slowly
        float angle = m_pulse * 8.f;
        ring.setRotation(sf::degrees(angle));
        inner.setRotation(sf::degrees(-angle * 1.5f));
        window.draw(ring);
        window.draw(inner);

        // Lines — rebuild with current alpha
        sf::VertexArray lines = m_sigilLines;
        for (size_t i = 0; i < lines.getVertexCount(); ++i) {
            sf::Color c = lines[i].color;
            c.a = (uint8_t)(c.a * a);
            lines[i].color = c;
        }
        window.draw(lines);
    };
    drawSigil();

    // Title
    sf::Text title   = m_title;
    sf::Text sub     = m_subtitle;
    sf::Text prompt  = m_prompt;

    title.setFillColor(withAlpha(sf::Color(220, 200, 255), a));
    title.setOutlineColor(withAlpha(sf::Color(80, 40, 140), a));
    sub.setFillColor(withAlpha(sf::Color(160, 130, 210), a));
    sub.setOutlineColor(withAlpha(sf::Color(40, 20, 80), a));

    // Prompt pulsing opacity
    float promptA = m_state == SplashState::Idle
                    ? a * (0.5f + 0.5f * std::sin(m_pulse * 3.f))
                    : 0.f;
    prompt.setFillColor(withAlpha(sf::Color(200, 180, 240), promptA));
    prompt.setOutlineColor(withAlpha(sf::Color(40, 20, 80), promptA));

    window.draw(title);
    window.draw(sub);
    window.draw(prompt);

    // Black overlay for fade in/out
    if (a < 1.f) {
        sf::RectangleShape overlay({(float)WIN_W, (float)WIN_H});
        overlay.setFillColor(sf::Color(0, 0, 0, (uint8_t)((1.f - a) * 255.f)));
        window.draw(overlay);
    }
}