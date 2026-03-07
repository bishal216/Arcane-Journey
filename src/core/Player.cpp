#include "core/Player.hpp"

#include <algorithm>
#include <cmath>

#include "Constants.hpp"
#include "systems/ArtifactManager.hpp"
#include "systems/DiscoveryTracker.hpp"
#include "systems/Juice.hpp"

// Body dimensions — must match BODY_W/BODY_H in main.cpp
static constexpr float BODY_W = 22.f;
static constexpr float BODY_H = 32.f;
static constexpr float HEAD_R = 11.f;

Player::Player() : m_shape({BODY_W, BODY_H}), m_head(HEAD_R), m_eyeL(2.5f), m_eyeR(2.5f) {
    m_shape.setOrigin({BODY_W / 2.f, BODY_H / 2.f});
    m_head.setOrigin({HEAD_R, HEAD_R});
    m_eyeL.setOrigin({2.5f, 2.5f});
    m_eyeR.setOrigin({2.5f, 2.5f});
    reset();
}

void Player::reset() {
    m_shape.setPosition({(float)WIN_W / 2.f, m_worldH - TILEF * 2.f});
    m_vel = {0.f, 0.f};
    m_onGround = false;
    m_onWall = false;
    m_wallDir = 0;
    m_jumpsLeft = 1;
    m_jumpHeld = false;
    m_dashAvail = true;
    m_dashing = false;
    m_dashTimer = 0.f;
    m_dashCoolTimer = 0.f;
    m_dashDir = {0.f, 0.f};
    m_prevSpace = false;
    m_prevUp = false;
    m_prevShift = false;
    m_isClimbing = false;
    m_climbInput = 0.f;
    m_prevOnGround = false;
}

void Player::update(float dt, const std::vector<Platform>& platforms, GameState& state) {
    using Key = sf::Keyboard::Key;
    const auto& qm = g_artifacts.mods();

    bool left = sf::Keyboard::isKeyPressed(Key::Left) || sf::Keyboard::isKeyPressed(Key::A);
    bool right = sf::Keyboard::isKeyPressed(Key::Right) || sf::Keyboard::isKeyPressed(Key::D);
    bool up = sf::Keyboard::isKeyPressed(Key::Up) || sf::Keyboard::isKeyPressed(Key::W);
    bool down = sf::Keyboard::isKeyPressed(Key::Down) || sf::Keyboard::isKeyPressed(Key::S);
    bool space = sf::Keyboard::isKeyPressed(Key::Space);
    bool shift = sf::Keyboard::isKeyPressed(Key::LShift) || sf::Keyboard::isKeyPressed(Key::RShift);

    // Mirrored quirk — swap left/right
    if (qm.inputMirrored) std::swap(left, right);

    bool jumpPressed = (space && !m_prevSpace) || (up && !m_prevUp);
    bool shiftPressed = shift && !m_prevShift;
    m_jumpHeld = space || up;
    m_prevSpace = space;
    m_prevUp = up;
    m_prevShift = shift;

    m_climbInput = 0.f;
    if (up) m_climbInput = -1.f;
    if (down) m_climbInput = 1.f;

    if (jumpPressed && m_isClimbing) {
        m_isClimbing = false;
        m_vel.y = JUMP_VEL * qm.jumpMult;
        g_juice.onJump(m_shape.getPosition());
    }

    if (m_dashCoolTimer > 0.f) m_dashCoolTimer -= dt;

    // Lead Boots — dash recharges instantly on ground
    if (m_onGround && !qm.dashEnabled == false) {
        // handled via dashEnabled flag — instant recharge uses coolTimer=0
    }

    // --- Dash ---
    bool canDash = qm.dashEnabled && m_dashAvail && !m_dashing && m_dashCoolTimer <= 0.f &&
                   !(m_glassDashUsed);  // Glass Dash: one dash per run

    if (shiftPressed && canDash) {
        m_dashing = true;
        m_dashTimer = DASH_DURATION;
        m_dashAvail = false;
        m_dashCoolTimer = DASH_COOLDOWN;
        m_dashEverUsed = true;
        if (m_glassDashUsed == false) m_glassDashUsed = true;  // only matters for GlassDash

        float dx = right ? 1.f : (left ? -1.f : (m_vel.x >= 0.f ? 1.f : -1.f));
        float dy = up ? -1.f : (down ? 1.f : 0.f);
        float len = std::sqrt(dx * dx + dy * dy);
        m_dashDir = {dx / len, dy / len};
        m_vel = m_dashDir * DASH_SPEED * qm.dashSpeedMult;
        m_isClimbing = false;

        g_juice.onDash(m_shape.getPosition(), m_dashDir, m_shape.getFillColor());
    }

    if (m_dashing) {
        m_dashTimer -= dt;
        m_vel = m_dashDir * DASH_SPEED * qm.dashSpeedMult;
        g_juice.onDash(m_shape.getPosition(), m_dashDir, m_shape.getFillColor());
        if (m_dashTimer <= 0.f) {
            m_dashing = false;
            m_vel.x *= 0.4f;
            m_vel.y = std::min(m_vel.y, 100.f);
        }
    } else if (m_isClimbing) {
        float accel = AIR_ACCEL * qm.speedMult;
        float fric = AIR_FRIC * qm.frictionMult;
        if (left)
            m_vel.x -= accel * dt;
        else if (right)
            m_vel.x += accel * dt;
        else {
            float dec = fric * dt;
            if (std::abs(m_vel.x) <= dec)
                m_vel.x = 0.f;
            else
                m_vel.x -= dec * (m_vel.x > 0.f ? 1.f : -1.f);
        }
        m_vel.x = std::clamp(m_vel.x, -MAX_SPEED * qm.speedMult, MAX_SPEED * qm.speedMult);
    } else {
        float accel = (m_onGround ? RUN_ACCEL : AIR_ACCEL) * qm.speedMult;
        float fric = (m_onGround ? FRICTION : AIR_FRIC) * qm.frictionMult;
        float maxSpd = MAX_SPEED * qm.speedMult;

        if (left)
            m_vel.x -= accel * dt;
        else if (right)
            m_vel.x += accel * dt;
        else {
            float dec = fric * dt;
            if (std::abs(m_vel.x) <= dec)
                m_vel.x = 0.f;
            else
                m_vel.x -= dec * (m_vel.x > 0.f ? 1.f : -1.f);
        }
        m_vel.x = std::clamp(m_vel.x, -maxSpd, maxSpd);

        // Gravity — affected by gravityMult and fallMult
        float grav = ((m_jumpHeld && m_vel.y < 0.f) ? JUMP_HOLD_GRAV : GRAVITY) * qm.gravityMult;
        float maxFall = MAX_FALL * qm.fallMult;
        m_vel.y = std::min(m_vel.y + grav * dt, maxFall);

        if (jumpPressed) {
            if (m_onGround) {
                m_vel.y = JUMP_VEL * qm.jumpMult;
                m_jumpsLeft = 1;
                g_juice.onJump(m_shape.getPosition());
            } else if (m_onWall && qm.wallJump) {
                m_vel.x = -(float)m_wallDir * WALL_JUMP_X;
                m_vel.y = WALL_JUMP_Y * qm.jumpMult;
                m_jumpsLeft = 1;
                g_juice.onJump(m_shape.getPosition());
            } else if (m_jumpsLeft > 0 && qm.doubleJump) {
                m_vel.y = JUMP_VEL * 0.9f * qm.jumpMult;
                m_jumpsLeft--;
                g_juice.onJump(m_shape.getPosition());
            }
        }
    }

    // Wall slide sparks
    if (m_onWall && !m_onGround && m_vel.y > 0.f)
        g_juice.onWallSlide(m_shape.getPosition(), m_wallDir);

    m_prevOnGround = m_onGround;

    m_shape.move({m_vel.x * dt, 0.f});
    resolveHorizontal(platforms);
    m_shape.move({0.f, m_vel.y * dt});
    resolveVertical(platforms, state);

    // Hard world boundary
    {
        float hw = m_shape.getSize().x / 2.f;
        float px = m_shape.getPosition().x;
        if (px - hw < 0.f) {
            m_shape.setPosition({hw, m_shape.getPosition().y});
            m_vel.x = 0.f;
        }
        if (px + hw > m_worldW) {
            m_shape.setPosition({m_worldW - hw, m_shape.getPosition().y});
            m_vel.x = 0.f;
        }
    }

    // Tiny quirk — scale hitbox
    {
        float s = g_artifacts.mods().sizeScale;
        m_shape.setSize({BODY_W * s, BODY_H * s});
        m_shape.setOrigin({BODY_W * s / 2.f, BODY_H * s / 2.f});
    }

    updateColor();
}

void Player::resolveHorizontal(const std::vector<Platform>& platforms) {
    sf::FloatRect pr = m_shape.getGlobalBounds();
    m_onWall = false;
    m_wallDir = 0;
    for (const auto& p : platforms) {
        sf::FloatRect plr = p.shape.getGlobalBounds();
        if (pr.findIntersection(plr)) {
            if (m_vel.x > 0.f)
                m_shape.setPosition({plr.position.x - pr.size.x / 2.f, m_shape.getPosition().y});
            else if (m_vel.x < 0.f)
                m_shape.setPosition(
                    {plr.position.x + plr.size.x + pr.size.x / 2.f, m_shape.getPosition().y});
            m_onWall = true;
            m_wallDir = (m_vel.x > 0.f) ? 1 : -1;
            m_vel.x = 0.f;
            pr = m_shape.getGlobalBounds();
        }
    }
}

void Player::resolveVertical(const std::vector<Platform>& platforms, GameState& state) {
    sf::FloatRect pr = m_shape.getGlobalBounds();
    float prevVY = m_vel.y;
    m_onGround = false;

    for (const auto& p : platforms) {
        sf::FloatRect plr = p.shape.getGlobalBounds();
        if (pr.findIntersection(plr)) {
            if (m_vel.y > 0.f) {
                m_shape.setPosition({m_shape.getPosition().x, plr.position.y - pr.size.y / 2.f});
                m_onGround = true;
                m_jumpsLeft = 1;
                m_dashAvail = true;
                m_isClimbing = false;
                g_discovery.discover(p.isGoal ? PlatType::Goal : PlatType::Static);

                // Juice: land
                if (!m_prevOnGround) g_juice.onLand(m_shape.getPosition(), prevVY);

                // Bouncy Castle
                if (g_artifacts.mods().bouncy)
                    m_vel.y = -180.f;
                else
                    m_vel.y = 0.f;

                // Glass Cannon — hard fall resets to hub
                if (g_artifacts.mods().glassCannon && prevVY > 500.f)
                    state = GameState::Playing;  // flag handled in main — treat as forced reset

                if (p.isGoal) state = GameState::Won;
            } else {
                m_shape.setPosition(
                    {m_shape.getPosition().x, plr.position.y + plr.size.y + pr.size.y / 2.f});
                m_vel.y = 0.f;
            }
            pr = m_shape.getGlobalBounds();
        }
    }
}

void Player::updateColor() {
    sf::Color bodyCol;
    if (m_dashing)
        bodyCol = sf::Color(255, 120, 50);
    else if (m_isClimbing)
        bodyCol = sf::Color(180, 220, 100);
    else if (!m_dashAvail)
        bodyCol = sf::Color(160, 100, 220);
    else if (!m_onGround)
        bodyCol = sf::Color(80, 140, 255);
    else
        bodyCol = sf::Color(100, 180, 255);

    m_shape.setFillColor(bodyCol);
    m_head.setFillColor(bodyCol);
    m_eyeL.setFillColor(sf::Color::White);
    m_eyeR.setFillColor(sf::Color::White);
}

void Player::draw(sf::RenderWindow& window) const {
    const auto& qm = g_artifacts.mods();
    float sx = g_juice.playerScaleX() * qm.sizeScale;
    float sy = g_juice.playerScaleY() * qm.sizeScale;

    sf::Vector2f pos = m_shape.getPosition();

    // --- Cape / aura / wings drawn first (behind player) ---
    g_artifacts.drawCosmetics(window, pos, sx, sy, m_vel.x < -10.f);

    // --- Body ---
    sf::RectangleShape body({BODY_W * sx, BODY_H * sy});
    body.setOrigin({BODY_W * sx / 2.f, BODY_H * sy / 2.f});
    body.setPosition(pos);
    body.setFillColor(m_shape.getFillColor());
    body.setOutlineColor(sf::Color(255, 255, 255, 60));
    body.setOutlineThickness(1.f);
    window.draw(body);

    // --- Head ---
    float headR = HEAD_R * ((sx + sy) / 2.f);
    sf::CircleShape head(headR);
    head.setOrigin({headR, headR});
    head.setPosition({pos.x, pos.y - BODY_H * sy / 2.f - headR});
    head.setFillColor(m_head.getFillColor());
    window.draw(head);

    // --- Eyes ---
    bool facingLeft = m_vel.x < -10.f;
    float eyeY = pos.y - BODY_H * sy / 2.f - headR * 0.5f;
    float eyeOX = facingLeft ? -6.f : 6.f;
    float eyeIX = facingLeft ? -1.f : 1.f;

    sf::CircleShape eyeL(2.5f), eyeR(2.5f);
    eyeL.setOrigin({2.5f, 2.5f});
    eyeR.setOrigin({2.5f, 2.5f});
    eyeL.setFillColor(sf::Color::White);
    eyeR.setFillColor(sf::Color::White);
    eyeL.setPosition({pos.x + (facingLeft ? eyeOX : eyeIX), eyeY});
    eyeR.setPosition({pos.x + (facingLeft ? eyeIX : eyeOX), eyeY});
    window.draw(eyeL);
    window.draw(eyeR);
}