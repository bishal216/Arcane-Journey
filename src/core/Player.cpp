#include "systems/DiscoveryTracker.hpp"
#include "core/Player.hpp"
#include "systems/Juice.hpp"
#include "Constants.hpp"
#include <cmath>
#include <algorithm>

static constexpr float BODY_W = 22.f;
static constexpr float BODY_H = 32.f;
static constexpr float HEAD_R = 11.f;

  Player::Player() : m_shape({BODY_W, BODY_H}) {
      m_shape.setOrigin({BODY_W / 2.f, BODY_H / 2.f});

      m_head.setRadius(HEAD_R);
      m_head.setOrigin({HEAD_R, HEAD_R});

      m_eyeL.setRadius(2.5f);  m_eyeL.setOrigin({2.5f, 2.5f});
      m_eyeL.setFillColor(sf::Color(20, 10, 30));
      m_eyeR.setFillColor(sf::Color(20, 10, 30));

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
        g_juice.onJump(m_shape.getPosition());
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
        g_juice.onDash(m_shape.getPosition(), m_dashDir, m_shape.getFillColor());
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
        g_juice.onDash(m_shape.getPosition(), m_dashDir, m_shape.getFillColor());
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
            g_juice.onWallSlide(m_shape.getPosition(), m_wallDir);
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
                g_juice.onLand(m_shape.getPosition(), m_vel.y);
                g_discovery.discover(p.isGoal ? PlatType::Goal : PlatType::Static);
                m_jumpsLeft  = 1;
                m_dashAvail  = true;
                m_isClimbing = false;
                if (p.isGoal){
                    state = GameState::Won;
                    g_juice.onGoal(m_shape.getPosition());
                } 
            } else {
                m_shape.setPosition({m_shape.getPosition().x, plr.position.y + plr.size.y + pr.size.y / 2.f});
            }
            m_vel.y = 0.f;
            pr = m_shape.getGlobalBounds();
        }
    }
}

void Player::updateColor() {
    sf::Color bodyCol, headCol;

    if (m_dashing) {
        bodyCol = sf::Color(255, 120,  50);
        headCol = sf::Color(255, 160,  80);
    } else if (m_isClimbing) {
        bodyCol = sf::Color(180, 220, 100);
        headCol = sf::Color(210, 240, 140);
    } else if (!m_dashAvail) {
        bodyCol = sf::Color(160, 100, 220);
        headCol = sf::Color(200, 140, 255);
    } else if (!m_onGround) {
        bodyCol = sf::Color( 80, 150, 240);
        headCol = sf::Color(120, 190, 255);
    } else {
        bodyCol = sf::Color(100, 180, 255);
        headCol = sf::Color(140, 210, 255);
    }

    m_shape.setFillColor(bodyCol);
    m_shape.setOutlineColor(sf::Color(255, 255, 255, 60));
    m_shape.setOutlineThickness(1.f);

    m_head.setFillColor(headCol);
    m_head.setOutlineColor(sf::Color(255, 255, 255, 80));
    m_head.setOutlineThickness(1.f);
}

void Player::draw(sf::RenderWindow& window) const {
    float sx = g_juice.playerScaleX();
    float sy = g_juice.playerScaleY();

    sf::Vector2f pos = m_shape.getPosition();

    sf::RectangleShape body({BODY_W * sx, BODY_H * sy});
    body.setOrigin({BODY_W * sx / 2.f, BODY_H * sy / 2.f});
    body.setPosition(pos);
    body.setFillColor(m_shape.getFillColor());
    body.setOutlineColor(sf::Color(255, 255, 255, 60));
    body.setOutlineThickness(1.f);
    window.draw(body);

    // Head stays same size, just shifts with body
    sf::CircleShape head = m_head;
    head.setPosition({pos.x, pos.y - BODY_H * sy / 2.f - HEAD_R});
    window.draw(head);

    // Eyes
    bool  facingLeft = m_vel.x < -10.f;
    float eyeY  = pos.y - BODY_H * sy / 2.f - HEAD_R * 0.5f;
    float eyeOX = facingLeft ? -6.f :  6.f;
    float eyeIX = facingLeft ? -1.f :  1.f;
    sf::CircleShape eyeL = m_eyeL, eyeR = m_eyeR;
    eyeL.setPosition({pos.x + (facingLeft ? eyeOX : eyeIX), eyeY});
    eyeR.setPosition({pos.x + (facingLeft ? eyeIX : eyeOX), eyeY});
    window.draw(eyeL);
    window.draw(eyeR);
}