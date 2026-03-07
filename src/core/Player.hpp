#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "core/World.hpp"

enum class GameState { Playing, Won };

class Player {
   public:
    Player();

    void reset();
    void resetRun() {
        m_glassDashUsed = false;
        m_dashEverUsed = false;
    }
    void setWorldH(float h) { m_worldH = h; }
    void setWorldW(float w) { m_worldW = w; }

    void update(float dt, const std::vector<Platform>& platforms, GameState& state);
    void draw(sf::RenderWindow& window) const;

    sf::Vector2f position() const { return m_shape.getPosition(); }
    sf::Vector2f velocity() const { return m_vel; }
    sf::FloatRect globalBounds() const { return m_shape.getGlobalBounds(); }

    bool isOnGround() const { return m_onGround; }
    bool isDashAvail() const { return m_dashAvail; }
    bool hasDashBeenUsed() const { return m_dashEverUsed; }
    int jumpsLeft() const { return m_jumpsLeft; }
    bool isClimbing() const { return m_isClimbing; }
    float climbInput() const { return m_climbInput; }

    void setPosition(sf::Vector2f v) { m_shape.setPosition(v); }
    void setVelocity(sf::Vector2f v) { m_vel = v; }
    void setOnGround(bool v) { m_onGround = v; }
    void setDashAvail(bool v) { m_dashAvail = v; }
    void setJumpsLeft(int v) { m_jumpsLeft = v; }
    void setClimbing(bool v) { m_isClimbing = v; }

   private:
    sf::RectangleShape m_shape;
    sf::CircleShape m_head;
    sf::CircleShape m_eyeL;
    sf::CircleShape m_eyeR;

    sf::Vector2f m_vel = {0.f, 0.f};
    float m_worldH = 2704.f;
    float m_worldW = 1024.f;

    bool m_onGround = false;
    bool m_prevOnGround = false;
    bool m_onWall = false;
    int m_wallDir = 0;
    int m_jumpsLeft = 1;
    bool m_jumpHeld = false;
    bool m_dashAvail = true;
    bool m_dashing = false;
    float m_dashTimer = 0.f;
    float m_dashCoolTimer = 0.f;
    bool m_glassDashUsed = false;  // Glass Dash artifact — one dash per run
    bool m_dashEverUsed = false;   // for Ghost Run achievement tracking
    sf::Vector2f m_dashDir = {0.f, 0.f};
    bool m_prevSpace = false;
    bool m_prevUp = false;
    bool m_prevShift = false;
    bool m_isClimbing = false;
    float m_climbInput = 0.f;

    void resolveHorizontal(const std::vector<Platform>& platforms);
    void resolveVertical(const std::vector<Platform>& platforms, GameState& state);
    void updateColor();
};