#include "Climbable.hpp"

#include <algorithm>
#include <cmath>

#include "systems/ArtifactManager.hpp"
#include "systems/DiscoveryTracker.hpp"

// ---------------------------------------------------------------------------
// Climbable
// ---------------------------------------------------------------------------

Climbable::Climbable(float x, float topY, float height, ClimbableType t, sf::Color col)
    : type(t), baseColor(col) {
    float w = (t == ClimbableType::Rope) ? ROPE_W : LADDER_W;

    shape.setSize({w, height});
    shape.setPosition({x - w * 0.5f, topY});
    shape.setFillColor(col);

    if (t == ClimbableType::Ladder) {
        shape.setOutlineColor(sf::Color(col.r, col.g, col.b, 160));
        shape.setOutlineThickness(1.f);
    }

    // Grab zone is wider so it's easier to catch
    grabZone = sf::FloatRect({x - w * 0.5f - GRAB_PAD, topY}, {w + GRAB_PAD * 2.f, height});

    swayAmount = (t == ClimbableType::Rope) ? 3.f : 0.f;
}

void Climbable::update(float dt) {
    pulseTimer += dt;
    swayTimer += dt;

    if (type == ClimbableType::Rope) {
        // Gentle horizontal sway on the rope visual
        float sway = std::sin(swayTimer * 1.4f) * swayAmount;
        auto pos = shape.getPosition();
        shape.setPosition({pos.x + sway * dt, pos.y});  // just cosmetic drift
    }
}

void Climbable::draw(sf::RenderWindow& window) const {
    if (type == ClimbableType::Ladder) {
        // Draw two vertical rails
        sf::RectangleShape rail({3.f, shape.getSize().y});
        rail.setFillColor(baseColor);

        auto p = shape.getPosition();
        float w = shape.getSize().x;
        rail.setPosition({p.x + 2.f, p.y});
        window.draw(rail);
        rail.setPosition({p.x + w - 5.f, p.y});
        window.draw(rail);

        // Draw rungs
        sf::RectangleShape rung({w - 4.f, 3.f});
        rung.setFillColor(sf::Color(baseColor.r + 30, baseColor.g + 20, baseColor.b, 220));
        float rungSpacing = 20.f;
        float h = shape.getSize().y;
        int numRungs = static_cast<int>(h / rungSpacing);
        for (int i = 0; i <= numRungs; ++i) {
            rung.setPosition({p.x + 2.f, p.y + i * rungSpacing});
            window.draw(rung);
        }
    } else {
        // Rope: draw the main shape with slight taper at top
        window.draw(shape);

        // Draw knot dots along rope for texture
        sf::CircleShape knot(2.5f);
        knot.setFillColor(sf::Color(baseColor.r - 40, baseColor.g - 30, baseColor.b - 20, 200));
        auto p = shape.getPosition();
        float h = shape.getSize().y;
        int n = static_cast<int>(h / 30.f);
        for (int i = 1; i < n; ++i) {
            knot.setPosition({p.x, p.y + i * 30.f});
            window.draw(knot);
        }
    }
}

// ---------------------------------------------------------------------------
// ClimbableManager
// ---------------------------------------------------------------------------

void ClimbableManager::addRope(float cx, float topY, float height, sf::Color col) {
    m_climbables.emplace_back(cx, topY, height, ClimbableType::Rope, col);
}

void ClimbableManager::addLadder(float cx, float topY, float height, sf::Color col) {
    m_climbables.emplace_back(cx, topY, height, ClimbableType::Ladder, col);
}

void ClimbableManager::update(float dt) {
    for (auto& c : m_climbables) c.update(dt);
}

void ClimbableManager::draw(sf::RenderWindow& window) const {
    for (const auto& c : m_climbables) c.draw(window);
}

bool ClimbableManager::resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel,
                                     sf::FloatRect playerRect, bool& onGround, bool& isClimbing,
                                     float climbInput, float dt) {
    // Check if player overlaps any climbable grab zone
    for (const auto& c : m_climbables) {
        if (!playerRect.findIntersection(c.bounds())) continue;

        // Player is touching this climbable.
        // Only engage climbing if they press up/down (or are already climbing).
        bool wantsClimb = (climbInput != 0.f) || isClimbing;
        if (!wantsClimb) continue;

        // Snap player X to centre of climbable
        float snapX = c.centerX();
        playerPos.x = snapX;

        // Suspend gravity, drive vertical by input
        playerVel.x = 0.f;
        playerVel.y = climbInput * CLIMB_SPEED * g_artifacts.mods().climbSpeedMult;
        onGround = false;
        isClimbing = true;

        // Clamp player within rope/ladder vertical bounds
        float halfH = playerRect.size.y * 0.5f;
        if (playerPos.y - halfH < c.topY()) {
            // Reached top — let them step off onto platform above
            playerPos.y = c.topY() + halfH;
            playerVel.y = 0.f;
            isClimbing = false;
            onGround = true;
        }
        if (playerPos.y + halfH > c.bottomY()) {
            playerPos.y = c.bottomY() - halfH;
            playerVel.y = 0.f;
            isClimbing = false;
        }

        return true;
    }

    // Not on any climbable
    isClimbing = false;
    return false;
}