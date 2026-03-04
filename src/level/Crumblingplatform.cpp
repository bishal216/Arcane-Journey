#include "core/DiscoveryTracker.hpp"
#include "CrumblingPlatform.hpp"
#include <cmath>
#include <algorithm>
#include <cstdlib>

// ---------------------------------------------------------------------------
// CrumblingPlatform
// ---------------------------------------------------------------------------

CrumblingPlatform::CrumblingPlatform(float x, float y, float w, float h, sf::Color col)
    : origin({x, y}), baseColor(col)
{
    shape.setSize({w, h});
    shape.setPosition({x, y});
    shape.setFillColor(col);
    shape.setOutlineColor(sf::Color(255, 255, 255, 80));
    shape.setOutlineThickness(1.f);
}

void CrumblingPlatform::onLand() {
    if (state == CrumbleState::Solid) {
        state = CrumbleState::Shaking;
        timer = 0.f;
    }
}

void CrumblingPlatform::update(float dt) {
    timer += dt;

    switch (state) {
        case CrumbleState::Solid:
            break;

        case CrumbleState::Shaking: {
            // Shake side to side with increasing intensity
            float intensity = (timer / SHAKE_TIME) * 4.f;
            shakeOffset = std::sin(timer * 60.f) * intensity;
            shape.setPosition({origin.x + shakeOffset, origin.y});

            // Flash orange as warning
            float t = timer / SHAKE_TIME;
            uint8_t r = (uint8_t)(baseColor.r + (255 - baseColor.r) * t);
            uint8_t g = (uint8_t)(baseColor.g * (1.f - t * 0.6f));
            uint8_t b = (uint8_t)(baseColor.b * (1.f - t * 0.8f));
            shape.setFillColor(sf::Color(r, g, b));

            if (timer >= SHAKE_TIME) {
                state = CrumbleState::Gone;
                timer = 0.f;
            }
            break;
        }

        case CrumbleState::Gone:
            // Invisible and non-solid
            if (timer >= GONE_TIME) {
                state = CrumbleState::Respawning;
                timer = 0.f;
                shape.setPosition(origin);
            }
            break;

        case CrumbleState::Respawning: {
            // Fade back in
            float t = timer / RESPAWN_TIME;
            uint8_t a = (uint8_t)(t * 255.f);
            shape.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, a));
            shape.setOutlineColor(sf::Color(255, 255, 255, (uint8_t)(a * 0.3f)));

            if (timer >= RESPAWN_TIME) {
                state = CrumbleState::Solid;
                timer = 0.f;
                shape.setFillColor(baseColor);
                shape.setOutlineColor(sf::Color(255, 255, 255, 80));
            }
            break;
        }
    }
}

void CrumblingPlatform::draw(sf::RenderWindow& window) const {
    if (state != CrumbleState::Gone)
        window.draw(shape);
}

// ---------------------------------------------------------------------------
// CrumblingPlatformManager
// ---------------------------------------------------------------------------

void CrumblingPlatformManager::add(float x, float y, float w, float h, sf::Color col) {
    m_platforms.emplace_back(x, y, w, h, col);
}

void CrumblingPlatformManager::update(float dt) {
    for (auto& p : m_platforms)
        p.update(dt);
}

void CrumblingPlatformManager::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_platforms)
        p.draw(window);
}

void CrumblingPlatformManager::reset() {
    for (auto& p : m_platforms) {
        p.state = CrumbleState::Solid;
        p.timer = 0.f;
        p.shape.setPosition(p.origin);
        p.shape.setFillColor(p.baseColor);
        p.shape.setOutlineColor(sf::Color(255, 255, 255, 80));
    }
}

bool CrumblingPlatformManager::resolvePlayer(sf::Vector2f& playerPos,
                                              sf::Vector2f& playerVel,
                                              sf::FloatRect playerRect,
                                              bool& onGround,
                                              bool& dashAvail,
                                              int& jumpsLeft,
                                              GameState& state)
{
    bool onCrumble = false;

    for (auto& cp : m_platforms) {
        if (!cp.isSolid()) continue;

        sf::FloatRect pr = cp.bounds();
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
            dashAvail   = true;
            jumpsLeft   = 1;
            onCrumble   = true;
            cp.onLand(); // trigger crumble
            g_discovery.discover(PlatType::Crumbling);
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

    return onCrumble;
}