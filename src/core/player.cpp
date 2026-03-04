#include "DiscoveryTracker.hpp"
#include "core/Player.hpp"
#include "Constants.hpp"
#include <cmath>
#include <algorithm>

Player::Player() : m_shape({32.f, 44.f}) {
    m_shape.setOrigin({16.f, 22.f});
    reset();
}

void Player::reset() {
    // Spawn near the bottom of the world — 2 tiles above the floor
    m_shape.setPosition({(float)WIN_W / 2.f, m_worldH - TILEF * 2.f});
    m_vel           = {0.f, 0.f};
    m_onGround      = false;
    m_onWall        = false;
    m_wallDir       = 0;
    m_jumpsLeft     = 1;
    m_jumpHeld      = false;
    m_dashAvail     = true;
    m_dashing       = false;
    m_dashTimer     = 0.f;
    m_dashCoolTimer = 0.f;
    m_dashDir       = {0.f, 0.f};
    m_prevSpace     = false;
    m_prevUp        = false;
    m_prevShift     = false;
    m_isClimbing    = false;
    m_climbInput    = 0.f;
}

void Player::update(float dt, const std::vector<Platform>& platforms, GameState& state) {
    using Key = sf::Keyboard::Key;

    bool left  = sf::Keyboard::isKeyPressed(Key::Left)  || sf::Keyboard::isKeyPressed(Key::A);
    bool right = sf::Keyboard::isKeyPressed(Key::Right) || sf::Keyboard::isKeyPressed(Key::D);
    bool up    = sf::Keyboard::isKeyPressed(Key::Up)    || sf::Keyboard::isKeyPressed(Key::W);
    bool down  = sf::Keyboard::isKeyPressed(Key::Down)  || sf::Keyboard::isKeyPressed(Key::S);
    bool space = sf::Keyboard::isKeyPressed(Key::Space);
    bool shift = sf::Keyboard::isKeyPressed(Key::LShift)|| sf::Keyboard::isKeyPressed(Key::RShift);

    bool jumpPressed  = (space && !m_prevSpace) || (up && !m_prevUp);
    bool shiftPressed = shift && !m_prevShift;
    m_jumpHeld  = space || up;
    m_prevSpace = space;
    m_prevUp    = up;
    m_prevShift = shift;

    m_climbInput = 0.f;
    if (up)   m_climbInput = -1.f;
    if (down) m_climbInput =  1.f;

    if (jumpPressed && m_isClimbing) {
        m_isClimbing = false;
        m_vel.y = JUMP_VEL;
    }

    if (m_dashCoolTimer > 0.f) m_dashCoolTimer -= dt;

    // --- Dash ---
    if (shiftPressed && m_dashAvail && !m_dashing && m_dashCoolTimer <= 0.f) {
        m_dashing       = true;
        m_dashTimer     = DASH_DURATION;
        m_dashAvail     = false;
        m_dashCoolTimer = DASH_COOLDOWN;
        float dx  = right ? 1.f : (left  ? -1.f : (m_vel.x >= 0.f ? 1.f : -1.f));
        float dy  = up    ? -1.f : (down ? 1.f  : 0.f);
        float len = std::sqrt(dx*dx + dy*dy);
        m_dashDir = {dx / len, dy / len};
        m_vel     = m_dashDir * DASH_SPEED;
        m_isClimbing = false;
    }

    if (m_dashing) {
        m_dashTimer -= dt;
        m_vel = m_dashDir * DASH_SPEED;
        if (m_dashTimer <= 0.f) {
            m_dashing = false;
            m_vel.x  *= 0.4f;
            m_vel.y   = std::min(m_vel.y, 100.f);
        }
    } else if (m_isClimbing) {
        float accel = AIR_ACCEL;
        float fric  = AIR_FRIC;
        if (left)       m_vel.x -= accel * dt;
        else if (right) m_vel.x += accel * dt;
        else {
            float dec = fric * dt;
            if (std::abs(m_vel.x) <= dec) m_vel.x = 0.f;
            else m_vel.x -= dec * (m_vel.x > 0.f ? 1.f : -1.f);
        }
        m_vel.x = std::clamp(m_vel.x, -MAX_SPEED, MAX_SPEED);
    } else {
        float accel = m_onGround ? RUN_ACCEL : AIR_ACCEL;
        float fric  = m_onGround ? FRICTION  : AIR_FRIC;

        if (left)       m_vel.x -= accel * dt;
        else if (right) m_vel.x += accel * dt;
        else {
            float dec = fric * dt;
            if (std::abs(m_vel.x) <= dec) m_vel.x = 0.f;
            else m_vel.x -= dec * (m_vel.x > 0.f ? 1.f : -1.f);
        }
        m_vel.x = std::clamp(m_vel.x, -MAX_SPEED, MAX_SPEED);

        float grav = (m_jumpHeld && m_vel.y < 0.f) ? JUMP_HOLD_GRAV : GRAVITY;
        m_vel.y = std::min(m_vel.y + grav * dt, MAX_FALL);

        if (jumpPressed) {
            if (m_onGround) {
                m_vel.y     = JUMP_VEL;
                m_jumpsLeft = 1;
            } else if (m_onWall) {
                m_vel.x     = -(float)m_wallDir * WALL_JUMP_X;
                m_vel.y     = WALL_JUMP_Y;
                m_jumpsLeft = 1;
            } else if (m_jumpsLeft > 0) {
                m_vel.y = JUMP_VEL * 0.9f;
                m_jumpsLeft--;
            }
        }
    }

    m_shape.move({m_vel.x * dt, 0.f});
    resolveHorizontal(platforms);
    m_shape.move({0.f, m_vel.y * dt});
    resolveVertical(platforms, state);

    // Hard boundary — stop at world edges
    {
        float hw = m_shape.getSize().x / 2.f;
        float px = m_shape.getPosition().x;
        float worldW = m_worldW;
        if (px - hw < 0.f)       { m_shape.setPosition({hw,          m_shape.getPosition().y}); m_vel.x = 0.f; }
        if (px + hw > worldW)    { m_shape.setPosition({worldW - hw, m_shape.getPosition().y}); m_vel.x = 0.f; }
    }

    updateColor();
}

void Player::resolveHorizontal(const std::vector<Platform>& platforms) {
    sf::FloatRect pr = m_shape.getGlobalBounds();
    m_onWall  = false;
    m_wallDir = 0;
    for (const auto& p : platforms) {
        sf::FloatRect plr = p.shape.getGlobalBounds();
        if (pr.findIntersection(plr)) {
            if (m_vel.x > 0.f)
                m_shape.setPosition({plr.position.x - pr.size.x / 2.f, m_shape.getPosition().y});
            else if (m_vel.x < 0.f)
                m_shape.setPosition({plr.position.x + plr.size.x + pr.size.x / 2.f, m_shape.getPosition().y});
            m_onWall  = true;
            m_wallDir = (m_vel.x > 0.f) ? 1 : -1;
            m_vel.x   = 0.f;
            pr = m_shape.getGlobalBounds();
        }
    }
}

void Player::resolveVertical(const std::vector<Platform>& platforms, GameState& state) {
    sf::FloatRect pr = m_shape.getGlobalBounds();
    m_onGround = false;
    for (const auto& p : platforms) {
        sf::FloatRect plr = p.shape.getGlobalBounds();
        if (pr.findIntersection(plr)) {
            if (m_vel.y > 0.f) {
                m_shape.setPosition({m_shape.getPosition().x, plr.position.y - pr.size.y / 2.f});
                m_onGround   = true;
                g_discovery.discover(p.isGoal ? PlatType::Goal : PlatType::Static);
                m_jumpsLeft  = 1;
                m_dashAvail  = true;
                m_isClimbing = false;
                if (p.isGoal) state = GameState::Won;
            } else {
                m_shape.setPosition({m_shape.getPosition().x, plr.position.y + plr.size.y + pr.size.y / 2.f});
            }
            m_vel.y = 0.f;
            pr = m_shape.getGlobalBounds();
        }
    }
}

void Player::updateColor() {
    if (m_dashing)
        m_shape.setFillColor(sf::Color(255, 120, 50));
    else if (m_isClimbing)
        m_shape.setFillColor(sf::Color(180, 220, 100));
    else if (!m_dashAvail)
        m_shape.setFillColor(sf::Color(160, 100, 220));
    else
        m_shape.setFillColor(sf::Color(100, 180, 255));
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(m_shape);
}