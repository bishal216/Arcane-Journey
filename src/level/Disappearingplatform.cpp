#include "systems/DiscoveryTracker.hpp"
#include "level/DisappearingPlatform.hpp"
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// DisappearingPlatform
// ---------------------------------------------------------------------------

DisappearingPlatform::DisappearingPlatform(float x, float y, float w, float h,
                                           sf::Color col,
                                           float visTime, float hidTime,
                                           float fade, float offset)
    : baseColor(col), visibleTime(visTime), hiddenTime(hidTime),
      fadeTime(fade), phaseOffset(offset)
{
    shape.setSize({w, h});
    shape.setPosition({x, y});
    shape.setFillColor(col);
    shape.setOutlineColor(sf::Color(255, 255, 255, 80));
    shape.setOutlineThickness(1.5f);

    // Apply phase offset — skip ahead in the cycle
    timer = offset;
    // Advance state to match offset
    float cycle = visTime + fade + hidTime + fade;
    float t = std::fmod(offset, cycle);
    if      (t < visTime)            { state = DisappearState::Visible;   timer = t; }
    else if (t < visTime + fade)     { state = DisappearState::FadingOut; timer = t - visTime; }
    else if (t < visTime + fade + hidTime) { state = DisappearState::Hidden; timer = t - visTime - fade; }
    else                             { state = DisappearState::FadingIn;  timer = t - visTime - fade - hidTime; }
}

void DisappearingPlatform::update(float dt) {
    timer += dt;

    switch (state) {
        case DisappearState::Visible:
            shape.setFillColor(baseColor);
            shape.setOutlineColor(sf::Color(255, 255, 255, 80));
            if (timer >= visibleTime) {
                state = DisappearState::FadingOut;
                timer = 0.f;
            }
            break;

        case DisappearState::FadingOut: {
            float t = 1.f - (timer / fadeTime);
            uint8_t a = (uint8_t)(std::clamp(t, 0.f, 1.f) * 255.f);
            shape.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, a));
            shape.setOutlineColor(sf::Color(255, 255, 255, (uint8_t)(a * 0.3f)));
            if (timer >= fadeTime) {
                state = DisappearState::Hidden;
                timer = 0.f;
            }
            break;
        }

        case DisappearState::Hidden:
            if (timer >= hiddenTime) {
                state = DisappearState::FadingIn;
                timer = 0.f;
            }
            break;

        case DisappearState::FadingIn: {
            float t = timer / fadeTime;
            uint8_t a = (uint8_t)(std::clamp(t, 0.f, 1.f) * 255.f);
            shape.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, a));
            shape.setOutlineColor(sf::Color(255, 255, 255, (uint8_t)(a * 0.3f)));
            if (timer >= fadeTime) {
                state = DisappearState::Visible;
                timer = 0.f;
                shape.setFillColor(baseColor);
            }
            break;
        }
    }
}

void DisappearingPlatform::draw(sf::RenderWindow& window) const {
    if (state != DisappearState::Hidden)
        window.draw(shape);
}

// ---------------------------------------------------------------------------
// DisappearingPlatformManager
// ---------------------------------------------------------------------------

void DisappearingPlatformManager::add(float x, float y, float w, float h,
                                      sf::Color col,
                                      float visTime, float hidTime,
                                      float fade, float offset)
{
    m_platforms.emplace_back(x, y, w, h, col, visTime, hidTime, fade, offset);
}

void DisappearingPlatformManager::update(float dt) {
    for (auto& p : m_platforms)
        p.update(dt);
}

void DisappearingPlatformManager::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_platforms)
        p.draw(window);
}

void DisappearingPlatformManager::resolvePlayer(sf::Vector2f& playerPos,
                                                sf::Vector2f& playerVel,
                                                sf::FloatRect playerRect,
                                                bool& onGround,
                                                bool& dashAvail,
                                                int&  jumpsLeft)
{
    for (const auto& p : m_platforms) {
        if (!p.isSolid()) continue;

        sf::FloatRect pr = p.bounds();
        if (!playerRect.findIntersection(pr)) continue;

        float playerBottom = playerRect.position.y + playerRect.size.y;
        float playerTop    = playerRect.position.y;
        float platTop      = pr.position.y;
        float platBottom   = pr.position.y + pr.size.y;
        float playerLeft   = playerRect.position.x;
        float playerRight  = playerRect.position.x + playerRect.size.x;
        float platLeft     = pr.position.x;
        float platRight    = pr.position.x + pr.size.x;

        float overlapTop    = playerBottom - platTop;
        float overlapBottom = platBottom   - playerTop;
        float overlapLeft   = playerRight  - platLeft;
        float overlapRight  = platRight    - playerLeft;

        float minOverlap = std::min({overlapTop, overlapBottom,
                                     overlapLeft, overlapRight});

        if (minOverlap == overlapTop && playerVel.y >= 0.f) {
            playerPos.y = platTop - playerRect.size.y / 2.f;
            playerVel.y = 0.f;
            onGround    = true;
            g_discovery.discover(PlatType::Disappearing);
            dashAvail   = true;
            jumpsLeft   = 1;
        } else if (minOverlap == overlapBottom && playerVel.y < 0.f) {
            playerPos.y = platBottom + playerRect.size.y / 2.f;
            playerVel.y = 0.f;
        } else if (minOverlap == overlapLeft) {
            playerPos.x = platLeft - playerRect.size.x / 2.f;
            playerVel.x = 0.f;
        } else if (minOverlap == overlapRight) {
            playerPos.x = platRight + playerRect.size.x / 2.f;
            playerVel.x = 0.f;
        }

        playerRect.position = {playerPos.x - playerRect.size.x / 2.f,
                               playerPos.y - playerRect.size.y / 2.f};
    }
}