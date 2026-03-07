#include "Trapdoor.hpp"

#include <algorithm>
#include <cmath>

#include "systems/DiscoveryTracker.hpp"

// ---------------------------------------------------------------------------
// Trapdoor
// ---------------------------------------------------------------------------

Trapdoor::Trapdoor(float x, float y, float w, float h, TrapdoorStyle s, sf::Color col)
    : origin({x, y}), baseColor(col), style(s) {
    shape.setSize({w, h});
    shape.setPosition({x, y});
    shape.setFillColor(col);
    shape.setOutlineColor(sf::Color(255, 255, 255, 60));
    shape.setOutlineThickness(1.f);
}

bool Trapdoor::isSolidFromAbove() const {
    if (style == TrapdoorStyle::OneWay) return true;  // always solid from above, never from below

    // Hatch is passable while Open
    return hatchState != HatchState::Open;
}

void Trapdoor::onLand() {
    if (style == TrapdoorStyle::Hatch && hatchState == HatchState::Closed) {
        hatchState = HatchState::Opening;
        hatchTimer = 0.f;
    }
}

void Trapdoor::update(float dt) {
    if (style == TrapdoorStyle::OneWay) return;

    hatchTimer += dt;

    switch (hatchState) {
        case HatchState::Closed:
            openAngle = 0.f;
            break;

        case HatchState::Opening: {
            float t = std::min(hatchTimer / OPEN_TIME, 1.f);
            // ease-out quad
            openAngle = 90.f * (1.f - (1.f - t) * (1.f - t));
            if (hatchTimer >= OPEN_TIME) {
                openAngle = 90.f;
                hatchState = HatchState::Open;
                hatchTimer = 0.f;
            }
            break;
        }

        case HatchState::Open:
            if (hatchTimer >= STAY_TIME) {
                hatchState = HatchState::Closing;
                hatchTimer = 0.f;
            }
            break;

        case HatchState::Closing: {
            float t = std::min(hatchTimer / CLOSE_TIME, 1.f);
            // ease-in quad
            openAngle = 90.f * (1.f - t * t);
            if (hatchTimer >= CLOSE_TIME) {
                openAngle = 0.f;
                hatchState = HatchState::Closed;
                hatchTimer = 0.f;
            }
            break;
        }
    }

    // Tint slightly lighter while open
    float glow = openAngle / 90.f * 40.f;
    shape.setFillColor(sf::Color((uint8_t)std::min(255.f, baseColor.r + glow),
                                 (uint8_t)std::min(255.f, baseColor.g + glow),
                                 (uint8_t)std::min(255.f, baseColor.b + glow)));
}

void Trapdoor::draw(sf::RenderWindow& window) const {
    if (style == TrapdoorStyle::OneWay) {
        // Draw as a thin platform with arrow indicators showing one-way direction
        window.draw(shape);

        // Small upward-pointing chevrons to hint at one-way nature
        float w = shape.getSize().x;
        float h = shape.getSize().y;
        auto pos = shape.getPosition();
        int nArrows = std::max(1, (int)(w / 32));
        float spacing = w / nArrows;

        sf::ConvexShape arrow(3);
        arrow.setFillColor(sf::Color(255, 255, 255, 100));
        for (int i = 0; i < nArrows; ++i) {
            float cx = pos.x + spacing * i + spacing * 0.5f;
            float cy = pos.y + h * 0.5f;
            arrow.setPoint(0, {cx, cy - 5.f});
            arrow.setPoint(1, {cx - 5.f, cy + 3.f});
            arrow.setPoint(2, {cx + 5.f, cy + 3.f});
            window.draw(arrow);
        }
        return;
    }

    // Hatch: draw the frame, then swing the door panel around the left edge
    auto pos = shape.getPosition();
    float w = shape.getSize().x;
    float h = shape.getSize().y;

    // Frame
    sf::RectangleShape frame({w, h});
    frame.setPosition(pos);
    frame.setFillColor(sf::Color(baseColor.r / 2, baseColor.g / 2, baseColor.b / 2, 200));
    frame.setOutlineColor(sf::Color(255, 255, 255, 80));
    frame.setOutlineThickness(1.f);
    window.draw(frame);

    if (hatchState == HatchState::Open) return;  // nothing to draw when fully open

    // Door panel rotated around its left edge
    // We simulate the swing by scaling X (2D approximation of rotation)
    float scale = std::cos(openAngle * 3.14159f / 180.f);
    sf::RectangleShape door({w * scale, h});
    door.setFillColor(baseColor);
    door.setOutlineColor(sf::Color(255, 255, 255, 60));
    door.setOutlineThickness(1.f);
    door.setPosition(pos);
    window.draw(door);

    // Hinge dot
    sf::CircleShape hinge(3.f);
    hinge.setFillColor(sf::Color(220, 220, 200));
    hinge.setPosition({pos.x - 2.f, pos.y + h * 0.5f - 3.f});
    window.draw(hinge);
}

// ---------------------------------------------------------------------------
// TrapdoorManager
// ---------------------------------------------------------------------------

void TrapdoorManager::addOneWay(float x, float y, float w, float h, sf::Color col) {
    m_trapdoors.emplace_back(x, y, w, h, TrapdoorStyle::OneWay, col);
}

void TrapdoorManager::addHatch(float x, float y, float w, float h, sf::Color col) {
    m_trapdoors.emplace_back(x, y, w, h, TrapdoorStyle::Hatch, col);
}

void TrapdoorManager::update(float dt) {
    for (auto& t : m_trapdoors) t.update(dt);
}

void TrapdoorManager::draw(sf::RenderWindow& window) const {
    for (const auto& t : m_trapdoors) t.draw(window);
}

bool TrapdoorManager::resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel,
                                    sf::FloatRect playerRect, bool& onGround, bool& dashAvail,
                                    int& jumpsLeft) {
    bool landed = false;

    for (auto& td : m_trapdoors) {
        if (!td.isSolidFromAbove()) continue;

        sf::FloatRect pr = td.bounds();
        if (!playerRect.findIntersection(pr)) continue;

        float playerBottom = playerRect.position.y + playerRect.size.y;
        float playerTop = playerRect.position.y;
        float platTop = pr.position.y;

        // ---------------------------------------------------------------
        // ONE-WAY RULE:
        // Only resolve if:
        //   (a) player is moving downward (vel.y >= 0), AND
        //   (b) player's bottom was above (or at) the platform top last
        //       frame — i.e. playerBottom - playerVel.y*dt <= platTop
        //       (they entered from above, not from below)
        // This lets the player jump upward through the platform freely.
        // ---------------------------------------------------------------
        float prevBottom = playerBottom - playerVel.y * 0.016f;  // approx prev frame
        bool cameFromAbove = prevBottom <= platTop + 2.f;        // small epsilon

        if (playerVel.y < 0.f) continue;  // rising — pass through
        if (!cameFromAbove) continue;     // came from below — pass through

        // Land on top
        playerPos.y = platTop - playerRect.size.y / 2.f;
        playerVel.y = 0.f;
        onGround = true;
        dashAvail = true;
        jumpsLeft = 1;
        landed = true;

        td.onLand();

        // Re-sync rect for subsequent iterations
        playerRect.position = {playerPos.x - playerRect.size.x / 2.f,
                               playerPos.y - playerRect.size.y / 2.f};
    }

    return landed;
}

void TrapdoorManager::reset() {
    for (auto& t : m_trapdoors) {
        t.hatchState = HatchState::Closed;
        t.hatchTimer = 0.f;
    }
}