#include "core/DiscoveryTracker.hpp"
#include "MovingPlatform.hpp"
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------------
// MovingPlatform
// ---------------------------------------------------------------------------

MovingPlatform::MovingPlatform(float x, float y, float w, float h,
                               sf::Vector2f a, sf::Vector2f b,
                               float spd, sf::Color col)
    : pointA(a), pointB(b), speed(spd)
{
    shape.setSize({w, h});
    shape.setPosition({x, y});
    shape.setFillColor(col);
    shape.setOutlineColor(sf::Color(255, 255, 255, 60));
    shape.setOutlineThickness(1.5f);

    sf::Vector2f d = pointB - pointA;
    isVertical = std::abs(d.y) > std::abs(d.x);
    float len = std::sqrt(d.x*d.x + d.y*d.y);
    m_dir = len > 0.f ? d / len : sf::Vector2f(1.f, 0.f);
    velocity = m_dir * speed;
}

void MovingPlatform::update(float dt) {
    // Store position before moving so resolvePlayer can use prevPos
    prevPos = shape.getPosition();

    sf::Vector2f pos    = shape.getPosition();
    sf::Vector2f target = m_towardB ? pointB : pointA;

    sf::Vector2f toTarget = target - pos;
    float dist = std::sqrt(toTarget.x*toTarget.x + toTarget.y*toTarget.y);

    if (dist < speed * dt) {
        shape.setPosition(target);
        m_towardB = !m_towardB;
        sf::Vector2f d = (m_towardB ? pointB : pointA) - target;
        float len = std::sqrt(d.x*d.x + d.y*d.y);
        m_dir = len > 0.f ? d / len : m_dir;
    } else {
        m_dir = toTarget / dist;
        shape.move(m_dir * speed * dt);
    }

    velocity = m_dir * speed;
}

void MovingPlatform::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

// ---------------------------------------------------------------------------
// MovingPlatformManager
// ---------------------------------------------------------------------------

void MovingPlatformManager::add(float x, float y, float w, float h,
                                sf::Vector2f a, sf::Vector2f b,
                                float speed, sf::Color col)
{
    m_platforms.emplace_back(x, y, w, h, a, b, speed, col);
}

void MovingPlatformManager::update(float dt) {
    for (auto& p : m_platforms)
        p.update(dt);
}

void MovingPlatformManager::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_platforms)
        p.draw(window);
}

bool MovingPlatformManager::resolvePlayer(sf::Vector2f& playerPos,
                                          sf::Vector2f& playerVel,
                                          sf::FloatRect playerRect,
                                          bool& onGround, float dt)
{
    bool carried = false;

    for (auto& mp : m_platforms) {
        sf::FloatRect pr = mp.bounds();

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

        // ------------------------------------------------------------
        // FIX: for vertical platforms, the platform's own Y velocity
        // can cause the player to appear to be "inside from below" even
        // when they're riding on top.  Use the platform's delta-Y this
        // frame to bias the decision: if the platform moved UP and the
        // overlap is small, treat it as a top-landing regardless of
        // the raw minOverlap winner.
        // ------------------------------------------------------------
        float platDeltaY  = mp.shape.getPosition().y - mp.prevPos.y;
        bool  platMovedUp = platDeltaY < 0.f;

        // Adjust effective overlapTop when platform is rising into player
        float effectiveOverlapTop = overlapTop;
        if (platMovedUp)
            effectiveOverlapTop -= std::abs(platDeltaY); // shrink so top wins

        float minOverlap = std::min({effectiveOverlapTop, overlapBottom,
                                     overlapLeft, overlapRight});

        if (minOverlap == effectiveOverlapTop && playerVel.y >= -mp.speed - 10.f) {
            // Land / ride on top
            playerPos.y = platTop - playerRect.size.y / 2.f;
            playerVel.y = 0.f;
            onGround    = true;
            g_discovery.discover(mp.isVertical ? PlatType::MoveV : PlatType::MoveH);
            carried     = true;

            // Carry: match platform displacement exactly (both X and Y)
            sf::Vector2f delta = mp.shape.getPosition() - mp.prevPos;
            playerPos += delta;

        } else if (minOverlap == overlapBottom && playerVel.y < 0.f) {
            // Bonk head on underside
            playerPos.y = platBottom + playerRect.size.y / 2.f;
            playerVel.y = 0.f;

            // If platform is descending fast, push player down with it
            if (platDeltaY > 0.f)
                playerPos.y += platDeltaY;

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

    return carried;
}