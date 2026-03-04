#include "core/DiscoveryTracker.hpp"
#include "BouncePad.hpp"
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------------
// BouncePad
// ---------------------------------------------------------------------------

BouncePad::BouncePad(float x, float y, float w, float h, float force, sf::Color col)
    : origin({x, y}), size({w, h}), bounceForce(force)
{
    shape.setSize({w, h});
    shape.setPosition({x, y});
    shape.setFillColor(col);
    shape.setOutlineColor(sf::Color(255, 255, 255, 100));
    shape.setOutlineThickness(1.5f);
}

void BouncePad::trigger() {
    animTimer  = 0.f;
    animActive = true;
}

void BouncePad::update(float dt) {
    if (!animActive) return;
    animTimer += dt;

    float t = animTimer / ANIM_DURATION;
    if (t >= 1.f) {
        animActive = false;
        shape.setSize(size);
        shape.setPosition(origin);
        return;
    }

    // Squash down then spring back using a sine curve
    // t=0: normal, t=0.2: max squash, t=1: normal
    float squash = 1.f - 0.5f * std::sin(t * 3.14159f);  // 0.5 -> 1.0 -> 0.5
    float stretchH = size.y * squash;
    float stretchW = size.x * (2.f - squash); // compensate width

    shape.setSize({stretchW, stretchH});
    // Keep bottom edge fixed so it doesn't appear to float
    shape.setPosition({origin.x - (stretchW - size.x) / 2.f,
                       origin.y + (size.y - stretchH)});
}

void BouncePad::draw(sf::RenderWindow& window) const {
    window.draw(shape);

    // Draw small arrow indicators pointing up
    float cx = shape.getPosition().x + shape.getSize().x / 2.f;
    float top = shape.getPosition().y;
    sf::RectangleShape arrow({4.f, 10.f});
    arrow.setFillColor(sf::Color(255, 255, 255, 160));
    arrow.setOrigin({2.f, 10.f});
    for (int i = -1; i <= 1; i++) {
        arrow.setPosition({cx + i * 12.f, top - 4.f});
        window.draw(arrow);
    }
}

// ---------------------------------------------------------------------------
// BouncePadManager
// ---------------------------------------------------------------------------

void BouncePadManager::add(float x, float y, float w, float h,
                            float force, sf::Color col) {
    m_pads.emplace_back(x, y, w, h, force, col);
}

void BouncePadManager::update(float dt) {
    for (auto& p : m_pads)
        p.update(dt);
}

void BouncePadManager::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_pads)
        p.draw(window);
}

void BouncePadManager::resolvePlayer(sf::Vector2f& playerPos,
                                      sf::Vector2f& playerVel,
                                      sf::FloatRect playerRect,
                                      bool& onGround)
{
    for (auto& pad : m_pads) {
        sf::FloatRect pr = pad.bounds();
        if (!playerRect.findIntersection(pr)) continue;

        float playerBottom = playerRect.position.y + playerRect.size.y;
        float platTop      = pr.position.y;

        // Only bounce when falling onto the top surface
        if (playerVel.y >= 0.f && playerBottom <= platTop + 8.f) {
            playerPos.y = platTop - playerRect.size.y / 2.f;
            playerVel.y = -pad.bounceForce;
            onGround    = false; // airborne immediately
            pad.trigger();
            g_discovery.discover(PlatType::Bounce);

            // Re-sync rect
            playerRect.position = {playerPos.x - playerRect.size.x / 2.f,
                                   playerPos.y - playerRect.size.y / 2.f};
        }
    }
}