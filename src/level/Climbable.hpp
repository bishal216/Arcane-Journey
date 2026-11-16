#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// A rope or ladder — a vertical region the player can grab and climb.
// While climbing: gravity is suspended, player moves vertically with
// climb speed, can jump off sideways.

enum class ClimbableType { Rope, Ladder };

struct Climbable {
    sf::RectangleShape shape;  // visual (thin for rope, wider for ladder)
    sf::FloatRect grabZone;    // slightly wider than shape — easier to grab
    ClimbableType type;
    sf::Color baseColor;
    float pulseTimer = 0.f;

    // Rope drawing: series of segments with sway
    float swayTimer = 0.f;
    float swayAmount = 0.f;  // pixels of horizontal sway at midpoint

    static constexpr float ROPE_W = 6.f;
    static constexpr float LADDER_W = 28.f;
    static constexpr float GRAB_PAD = 14.f;  // extra grab width each side

    // Ladders are fast and safe; ropes are slower but let you launch sideways off them
    // (see ROPE_LAUNCH_X in Constants.hpp).
    static constexpr float LADDER_SPEED_MULT = 1.25f;
    static constexpr float ROPE_SPEED_MULT = 0.8f;
    float speedMult() const {
        return type == ClimbableType::Ladder ? LADDER_SPEED_MULT : ROPE_SPEED_MULT;
    }

    Climbable(float x, float topY, float height, ClimbableType t, sf::Color col);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    sf::FloatRect bounds() const { return grabZone; }
    float centerX() const { return grabZone.position.x + grabZone.size.x * 0.5f; }
    float topY() const { return grabZone.position.y; }
    float bottomY() const { return grabZone.position.y + grabZone.size.y; }
};

class ClimbableManager {
   public:
    // x = horizontal centre of the rope/ladder
    void addRope(float centreX, float topY, float height, sf::Color col = sf::Color(200, 170, 100));
    void addLadder(float centreX, float topY, float height,
                   sf::Color col = sf::Color(160, 130, 80));

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    // Call each frame. Returns true if player is currently on a climbable.
    // Sets playerVel, suppresses gravity, handles climb movement.
    // climbAxis: up = negative, down = positive (from input, range -1..1)
    bool resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                       bool& onGround,
                       bool& isClimbing,  // in/out
                       float climbInput,  // -1=up, 0=none, +1=down
                       bool grabBlocked,  // player just jumped off: don't re-grab yet
                       float dt);

    static constexpr float CLIMB_SPEED = 160.f;  // pixels per second (before rope/ladder multiplier)

    // Ropes/ladders in a level are often chains of pieces with small gaps. Reaching the top of
    // a piece while holding Up hops across the gap to the next piece (if one is close enough);
    // at the end of a chain the player simply clings to the top.
    static constexpr float HOP_SPEED = 520.f;        // upward launch when hopping to the next piece
    static constexpr float MAX_HOP_GAP = 64.f;       // largest gap (px) that is hopped automatically
    static constexpr float HOP_REGRAB_DELAY = 0.12f; // don't re-grab the piece we just left
    static constexpr float DROP_REGRAB_DELAY = 0.15f; // after letting go at the bottom

    // True if the climbable the player most recently grabbed was a rope.
    bool lastGrabWasRope() const { return m_lastWasRope; }

   private:
    bool hasPieceAbove(const Climbable& c, float x) const;

    std::vector<Climbable> m_climbables;
    bool m_lastWasRope = false;
    float m_regrabTimer = 0.f;  // > 0: just let go of a piece, ignore grabs briefly
};
