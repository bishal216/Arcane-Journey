#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "systems/DiscoveryTracker.hpp"
#include "systems/ArtifactManager.hpp"

// ---------------------------------------------------------------------------
// KeyDoor — solid door that unlocks when its paired switch is activated
// Switch is a separate tile that toggles the door state
// ---------------------------------------------------------------------------

enum class KeyDoorState { Locked, Unlocking, Unlocked, Locking };

struct KeyDoor {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Color lockedColor;
    sf::Color unlockedColor;
    KeyDoorState state = KeyDoorState::Locked;
    float animTimer = 0.f;
    float openProgress = 0.f;  // 0 = closed, 1 = fully open (passable)

    std::string switchId;      // ID of the switch that controls this door
    int doorId = -1;           // Unique door index for pairing

    static constexpr float UNLOCK_TIME = 0.5f;   // Time to open
    static constexpr float LOCK_TIME = 0.3f;     // Time to close (if relocked)

    KeyDoor(float x, float y, float w, float h, const std::string& switchId_, int doorId_,
            sf::Color lockedCol = sf::Color(120, 60, 60), sf::Color unlockedCol = sf::Color(60, 160, 80))
        : origin({x, y}), lockedColor(lockedCol), unlockedColor(unlockedCol),
          switchId(switchId_), doorId(doorId_) {
        shape.setSize({w, h});
        shape.setPosition({x, y});
        shape.setFillColor(lockedColor);
        shape.setOutlineColor(sf::Color(255, 200, 100));
        shape.setOutlineThickness(2.f);
    }

    void unlock() {
        if (state == KeyDoorState::Locked) {
            state = KeyDoorState::Unlocking;
            animTimer = 0.f;
        }
    }

    void lock() {
        if (state == KeyDoorState::Unlocked) {
            state = KeyDoorState::Locking;
            animTimer = 0.f;
        }
    }

    void update(float dt) {
        switch (state) {
            case KeyDoorState::Unlocking: {
                animTimer += dt;
                openProgress = std::min(1.f, animTimer / UNLOCK_TIME);
                // Color transition
                float t = openProgress;
                uint8_t r = (uint8_t)(lockedColor.r + (unlockedColor.r - lockedColor.r) * t);
                uint8_t g = (uint8_t)(lockedColor.g + (unlockedColor.g - lockedColor.g) * t);
                uint8_t b = (uint8_t)(lockedColor.b + (unlockedColor.b - lockedColor.b) * t);
                shape.setFillColor(sf::Color(r, g, b, 255));
                // Visual: door slides up as it opens
                shape.setPosition({origin.x, origin.y - shape.getSize().y * openProgress});
                if (animTimer >= UNLOCK_TIME) {
                    state = KeyDoorState::Unlocked;
                    openProgress = 1.f;
                }
                break;
            }
            case KeyDoorState::Locking: {
                animTimer += dt;
                openProgress = std::max(0.f, 1.f - animTimer / LOCK_TIME);
                float t = openProgress;
                uint8_t r = (uint8_t)(lockedColor.r + (unlockedColor.r - lockedColor.r) * t);
                uint8_t g = (uint8_t)(lockedColor.g + (unlockedColor.g - lockedColor.g) * t);
                uint8_t b = (uint8_t)(lockedColor.b + (unlockedColor.b - lockedColor.b) * t);
                shape.setFillColor(sf::Color(r, g, b, 255));
                shape.setPosition({origin.x, origin.y - shape.getSize().y * openProgress});
                if (animTimer >= LOCK_TIME) {
                    state = KeyDoorState::Locked;
                    openProgress = 0.f;
                    shape.setPosition(origin);
                }
                break;
            }
            case KeyDoorState::Locked:
            case KeyDoorState::Unlocked:
                break;
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (state != KeyDoorState::Unlocked) window.draw(shape);
        // Could draw "unlocked" visual (just outline) when unlocked
        if (state == KeyDoorState::Unlocked) {
            sf::RectangleShape outline = shape;
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineColor(unlockedColor);
            outline.setOutlineThickness(2.f);
            window.draw(outline);
        }
    }

    bool isSolid() const { return state == KeyDoorState::Locked || state == KeyDoorState::Locking; }

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }

    void reset() {
        state = KeyDoorState::Locked;
        animTimer = 0.f;
        openProgress = 0.f;
        shape.setPosition(origin);
        shape.setFillColor(lockedColor);
    }
};

// Switch that activates KeyDoors with matching switchId
struct KeySwitch {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    std::string switchId;
    bool activated = false;
    float pressTimer = 0.f;

    static constexpr float PRESS_COOLDOWN = 0.5f;

    KeySwitch(float x, float y, float w, float h, const std::string& id,
              sf::Color col = sf::Color(255, 200, 50))
        : origin({x, y}), switchId(id) {
        shape.setSize({w, h});
        shape.setPosition({x, y});
        shape.setFillColor(col);
        shape.setOutlineColor(sf::Color(255, 255, 150));
        shape.setOutlineThickness(2.f);
    }

    void update(float dt) {
        if (pressTimer > 0.f) pressTimer -= dt;
    }

    void draw(sf::RenderWindow& window) const { window.draw(shape); }

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }

    bool tryActivate() {
        if (pressTimer > 0.f) return false;
        activated = !activated;
        pressTimer = PRESS_COOLDOWN;
        return true;
    }

    void reset() {
        activated = false;
        pressTimer = 0.f;
    }
};

class KeyDoorManager {
public:
    void addDoor(float x, float y, float w, float h, const std::string& switchId, int doorId,
                 sf::Color lockedCol = sf::Color(120, 60, 60), sf::Color unlockedCol = sf::Color(60, 160, 80));
    void addSwitch(float x, float y, float w, float h, const std::string& switchId,
                   sf::Color col = sf::Color(255, 200, 50));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void reset();

    // Call when player presses E near a switch
    bool tryActivateSwitch(const sf::FloatRect& playerRect);

    // Collision resolution for doors
    void resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                       bool& onGround, bool& dashAvail, int& jumpsLeft);

    const std::vector<KeyDoor>& doors() const { return m_doors; }
    const std::vector<KeySwitch>& switches() const { return m_switches; }

private:
    std::vector<KeyDoor> m_doors;
    std::vector<KeySwitch> m_switches;

    void syncDoorStates();
};

extern KeyDoorManager g_keyDoors;