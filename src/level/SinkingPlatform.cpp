#include "level/SinkingPlatform.hpp"

#include <algorithm>
#include <cmath>

#include "systems/ArtifactManager.hpp"
#include "systems/DiscoveryTracker.hpp"
#include "systems/Juice.hpp"

// ---------------------------------------------------------------------------
// SinkingPlatform
// ---------------------------------------------------------------------------

SinkingPlatform::SinkingPlatform(float x, float y, float w, float h, sf::Color col)
    : origin({x, y}), baseColor(col) {
    shape.setSize({w, h});
    shape.setPosition({x, y});
    shape.setFillColor(col);
    shape.setOutlineColor(sf::Color(255, 255, 255, 80));
    shape.setOutlineThickness(1.f);
}

void SinkingPlatform::syncShape() { shape.setPosition({origin.x, origin.y + sinkOffset}); }

void SinkingPlatform::update(float dt, bool playerOnTop) {
    const float speedScale = g_artifacts.mods().platformSpeedMult;

    switch (state) {
        case SinkState::Idle:
            if (playerOnTop) state = SinkState::Sinking;
            break;
        case SinkState::Sinking:
            sinkOffset += SINK_SPEED * speedScale * dt;
            if (sinkOffset >= MAX_SINK || !playerOnTop) state = SinkState::Rising;
            break;
        case SinkState::Rising:
            sinkOffset -= RISE_SPEED * speedScale * dt;
            if (sinkOffset <= 0.f) {
                sinkOffset = 0.f;
                state = SinkState::Idle;
            }
            break;
    }

    // Colour: base → amber as it sinks
    float t = std::clamp(sinkOffset / MAX_SINK, 0.f, 1.f);
    uint8_t r = (uint8_t)(baseColor.r + (200 - baseColor.r) * t);
    uint8_t g = (uint8_t)(baseColor.g + (80 - baseColor.g) * t);
    uint8_t b = (uint8_t)(baseColor.b + (20 - baseColor.b) * t);
    shape.setFillColor(sf::Color(r, g, b));

    syncShape();
}

void SinkingPlatform::draw(sf::RenderWindow& window) const { window.draw(shape); }

// ---------------------------------------------------------------------------
// SinkingPlatformManager
// ---------------------------------------------------------------------------

void SinkingPlatformManager::add(float x, float y, float w, float h, sf::Color col) {
    m_platforms.emplace_back(x, y, w, h, col);
}

void SinkingPlatformManager::update(float dt) {
    // Real update is driven by resolvePlayer which has the onTop flag.
    (void)dt;
}

void SinkingPlatformManager::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_platforms) p.draw(window);
}

void SinkingPlatformManager::reset() {
    for (auto& p : m_platforms) {
        p.state = SinkState::Idle;
        p.sinkOffset = 0.f;
        p.shape.setPosition(p.origin);
        p.shape.setFillColor(p.baseColor);
        p.shape.setOutlineColor(sf::Color(255, 255, 255, 80));
    }
}

bool SinkingPlatformManager::resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel,
                                           sf::FloatRect playerRect, bool& onGround,
                                           bool& dashAvail, int& jumpsLeft, float dt) {
    bool onSinking = false;
    const float speedScale = g_artifacts.mods().platformSpeedMult;

    for (auto& sp : m_platforms) {
        sf::FloatRect pr = sp.bounds();

        // Is player standing on top of this platform?
        bool playerOnTop = onGround && playerPos.x + playerRect.size.x * 0.5f > pr.position.x &&
                           playerPos.x - playerRect.size.x * 0.5f < pr.position.x + pr.size.x &&
                           std::abs((playerPos.y + playerRect.size.y * 0.5f) - pr.position.y) < 4.f;

        // Drive sink state with real dt
        switch (sp.state) {
            case SinkState::Idle:
                if (playerOnTop) sp.state = SinkState::Sinking;
                break;
            case SinkState::Sinking:
                sp.sinkOffset += SinkingPlatform::SINK_SPEED * speedScale * dt;
                if (sp.sinkOffset >= SinkingPlatform::MAX_SINK || !playerOnTop)
                    sp.state = SinkState::Rising;
                break;
            case SinkState::Rising:
                sp.sinkOffset -= SinkingPlatform::RISE_SPEED * speedScale * dt;
                if (sp.sinkOffset <= 0.f) {
                    sp.sinkOffset = 0.f;
                    sp.state = SinkState::Idle;
                }
                break;
        }

        // Update colour and position
        float t = std::clamp(sp.sinkOffset / SinkingPlatform::MAX_SINK, 0.f, 1.f);
        uint8_t rc = (uint8_t)(sp.baseColor.r + (200 - sp.baseColor.r) * t);
        uint8_t gc = (uint8_t)(sp.baseColor.g + (80 - sp.baseColor.g) * t);
        uint8_t bc = (uint8_t)(sp.baseColor.b + (20 - sp.baseColor.b) * t);
        sp.shape.setFillColor(sf::Color(rc, gc, bc));
        sp.shape.setPosition({sp.origin.x, sp.origin.y + sp.sinkOffset});
        pr = sp.bounds();  // refresh after move

        // Collision resolution
        if (!playerRect.findIntersection(pr)) continue;

        float playerBottom = playerRect.position.y + playerRect.size.y;
        float playerTop = playerRect.position.y;
        float platTop = pr.position.y;
        float platBottom = pr.position.y + pr.size.y;
        float playerLeft = playerRect.position.x;
        float playerRight = playerRect.position.x + playerRect.size.x;
        float platLeft = pr.position.x;
        float platRight = pr.position.x + pr.size.x;

        float overlapTop = playerBottom - platTop;
        float overlapBottom = platBottom - playerTop;
        float overlapLeft = playerRight - platLeft;
        float overlapRight = platRight - playerLeft;

        float minOverlap = std::min({overlapTop, overlapBottom, overlapLeft, overlapRight});

        if (minOverlap == overlapTop && playerVel.y >= 0.f) {
            playerPos.y = platTop - playerRect.size.y / 2.f;
            playerVel.y = 0.f;
            onGround = true;
            dashAvail = true;
            jumpsLeft = 1;
            onSinking = true;
            g_discovery.discover(PlatType::Sinking);
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

    return onSinking;
}