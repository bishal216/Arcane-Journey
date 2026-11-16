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

bool ClimbableManager::hasPieceAbove(const Climbable& c, float x) const {
    for (const auto& d : m_climbables) {
        if (&d == &c) continue;
        if (x < d.grabZone.position.x || x > d.grabZone.position.x + d.grabZone.size.x) continue;
        float gap = c.topY() - d.bottomY();
        if (gap >= -1.f && gap <= MAX_HOP_GAP) return true;
    }
    return false;
}

bool ClimbableManager::resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel,
                                     sf::FloatRect playerRect, bool& onGround, bool& isClimbing,
                                     float climbInput, bool grabBlocked, float dt) {
    if (m_regrabTimer > 0.f) m_regrabTimer -= dt;

    // Try the nearest piece first, so side-by-side ropes/ladders grab the one the player is
    // actually at instead of whichever comes first in the level file.
    std::vector<const Climbable*> order;
    order.reserve(m_climbables.size());
    for (const auto& c : m_climbables) order.push_back(&c);
    std::sort(order.begin(), order.end(), [&](const Climbable* a, const Climbable* b) {
        return std::abs(a->centerX() - playerPos.x) < std::abs(b->centerX() - playerPos.x);
    });

    for (const Climbable* cp : order) {
        const Climbable& c = *cp;
        if (!playerRect.findIntersection(c.bounds())) continue;

        // Just jumped off a rope/ladder: don't re-grab it for a moment so the jump carries
        // through. (A velocity check can't tell this apart from pressing Up at the foot of
        // a ladder, which is also a ground jump — that must still grab immediately.)
        if (!isClimbing && (grabBlocked || m_regrabTimer > 0.f)) continue;

        // Only engage climbing if they press up/down (or are already climbing).
        bool wantsClimb = (climbInput != 0.f) || isClimbing;
        if (!wantsClimb) continue;

        // Don't start climbing in a direction that leads away from the piece: pressing Down
        // while the player's centre is already below it (e.g. standing on the floor under its
        // end), or Up while it is already above it. Uses the centre so pieces shorter than
        // the player can still be grabbed.
        if (!isClimbing) {
            if (climbInput > 0.f && playerPos.y > c.bottomY()) continue;
            if (climbInput < 0.f && playerPos.y < c.topY()) continue;
        }

        // Snap player X to centre of climbable
        playerPos.x = c.centerX();

        // Suspend gravity, drive vertical by input
        playerVel.x = 0.f;
        playerVel.y = climbInput * CLIMB_SPEED * c.speedMult() * g_artifacts.mods().climbSpeedMult;
        onGround = false;
        isClimbing = true;
        m_lastWasRope = (c.type == ClimbableType::Rope);
        g_discovery.discover(c.type == ClimbableType::Rope ? PlatType::Rope : PlatType::Ladder);

        // Ends of the piece. (Letting go and re-grabbing every frame made the player flicker
        // at the top and made chains of pieces impossible to climb.)
        float halfH = playerRect.size.y * 0.5f;
        if (playerPos.y - halfH < c.topY()) {
            // Head is above the top. Sliding in from above (Down held) is fine; otherwise stop.
            if (climbInput <= 0.f) {
                playerPos.y = c.topY() + halfH;
                playerVel.y = 0.f;
                if (climbInput < 0.f && hasPieceAbove(c, playerPos.x)) {
                    // Hop the gap to the next piece of the chain.
                    isClimbing = false;
                    playerVel.y = -HOP_SPEED;
                    m_regrabTimer = HOP_REGRAB_DELAY;
                }
                // else: end of the chain — cling to the top.
            }
        } else if (playerPos.y + halfH > c.bottomY()) {
            // Feet are below the bottom.
            if (climbInput > 0.f) {
                // Climbed off the bottom: let go and fall.
                isClimbing = false;
                playerVel.y = 0.f;
                m_regrabTimer = DROP_REGRAB_DELAY;
            } else if (climbInput == 0.f) {
                playerPos.y = c.bottomY() - halfH;  // idle: tuck back onto the piece
                playerVel.y = 0.f;
            }
            // Moving up from below: let the body slide into the piece.
        }

        return true;
    }

    isClimbing = false;
    return false;
}