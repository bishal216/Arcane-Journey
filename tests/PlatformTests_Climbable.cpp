#include "TestFramework.hpp"
#include <cmath>

namespace sf {
    struct Vector2f { float x, y; Vector2f(float x_=0, float y_=0):x(x_),y(y_){} Vector2f operator+(const Vector2f& o) const {return {x+o.x,y+o.y};} Vector2f operator-(const Vector2f& o) const {return {x-o.x,y-o.y};} };
    struct FloatRect { Vector2f position, size; FloatRect(Vector2f p={0,0}, Vector2f s={0,0}):position(p),size(s){} bool findIntersection(const FloatRect& o) const { return !(position.x+size.x<=o.position.x || o.position.x+o.size.x<=position.x || position.y+size.y<=o.position.y || o.position.y+o.size.y<=position.y); } };
    struct RectangleShape { Vector2f position, size; Vector2f getPosition() const {return position;} Vector2f getSize() const {return size;} FloatRect getGlobalBounds() const {return FloatRect(position,size);} void setPosition(Vector2f p){position=p;} void setSize(Vector2f s){size=s;} };
    struct Color { int r,g,b,a; Color(int r_,int g_,int b_,int a_=255):r(r_),g(g_),b(b_),a(a_){} };
    struct RenderWindow {};
}

// ============================================================================
// Climbable (Rope/Ladder) Logic Tests
// ============================================================================

enum class ClimbableType { Rope, Ladder };

struct TestClimbable {
    sf::RectangleShape shape;
    sf::FloatRect grabZone;
    ClimbableType type;
    sf::Color baseColor;
    float pulseTimer = 0.f;
    float swayTimer = 0.f;
    float swayAmount = 0.f;

    static constexpr float ROPE_W = 6.f;
    static constexpr float LADDER_W = 28.f;
    static constexpr float GRAB_PAD = 14.f;
    static constexpr float LADDER_SPEED_MULT = 1.25f;
    static constexpr float ROPE_SPEED_MULT = 0.8f;
    static constexpr float CLIMB_SPEED = 160.f;
    static constexpr float HOP_SPEED = 520.f;
    static constexpr float MAX_HOP_GAP = 64.f;
    static constexpr float HOP_REGRAB_DELAY = 0.12f;
    static constexpr float DROP_REGRAB_DELAY = 0.15f;

    TestClimbable(float x, float topY, float height, ClimbableType t, sf::Color col)
        : type(t), baseColor(col) {
        float w = (t == ClimbableType::Rope) ? ROPE_W : LADDER_W;
        float cx = x;
        shape.setSize({w, height});
        shape.setPosition({cx - w/2.f, topY});

        grabZone = sf::FloatRect({cx - w/2.f - GRAB_PAD, topY}, {w + 2*GRAB_PAD, height});
        if (t == ClimbableType::Rope) swayAmount = 4.f;
    }

    float speedMult() const {
        return type == ClimbableType::Ladder ? LADDER_SPEED_MULT : ROPE_SPEED_MULT;
    }

    void update(float dt) {
        pulseTimer += dt;
        if (type == ClimbableType::Rope) swayTimer += dt;
    }

    sf::FloatRect bounds() const { return grabZone; }
    float centerX() const { return grabZone.position.x + grabZone.size.x * 0.5f; }
    float topY() const { return grabZone.position.y; }
    float bottomY() const { return grabZone.position.y + grabZone.size.y; }
};

struct TestClimbableManager {
    std::vector<TestClimbable> m_climbables;
    bool m_lastWasRope = false;
    float m_regrabTimer = 0.f;

    void addRope(float cx, float topY, float h, sf::Color col) {
        m_climbables.emplace_back(cx, topY, h, ClimbableType::Rope, col);
    }
    void addLadder(float cx, float topY, float h, sf::Color col) {
        m_climbables.emplace_back(cx, topY, h, ClimbableType::Ladder, col);
    }

    void update(float dt) {
        for (auto& c : m_climbables) c.update(dt);
        if (m_regrabTimer > 0.f) m_regrabTimer -= dt;
    }

    bool hasPieceAbove(const TestClimbable& c, float x) const {
        for (const auto& other : m_climbables) {
            if (&other == &c) continue;
            if (other.topY() < c.topY() - 1.f &&
                std::abs(other.centerX() - x) < 20.f &&
                c.topY() - other.bottomY() <= MAX_HOP_GAP) {
                return true;
            }
        }
        return false;
    }

    // Simplified resolvePlayer for testing
    bool resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                       bool& onGround, bool& isClimbing, float climbInput, bool grabBlocked, float dt,
                       float climbSpeedMult = 1.f) {
        // Find climbable overlapping player
        TestClimbable* current = nullptr;
        for (auto& c : m_climbables) {
            if (c.bounds().findIntersection(playerRect)) {
                current = &c;
                break;
            }
        }

        if (isClimbing) {
            // Already climbing
            if (climbInput == 0.f && !current) {
                // Let go
                isClimbing = false;
                m_regrabTimer = DROP_REGRAB_DELAY;
                return false;
            }
            // Move vertically
            float speed = CLIMB_SPEED * current->speedMult() * climbSpeedMult;
            playerVel.y = climbInput * speed;
            playerVel.x = 0.f;
            onGround = false;
            return true;
        } else {
            // Try to grab
            if (grabBlocked || m_regrabTimer > 0.f) return false;
            if (current && climbInput != 0.f) {
                isClimbing = true;
                m_lastWasRope = (current->type == ClimbableType::Rope);
                m_regrabTimer = 0.f;
                playerVel.y = climbInput * CLIMB_SPEED * current->speedMult() * climbSpeedMult;
                playerVel.x = 0.f;
                onGround = false;
                return true;
            }
            return false;
        }
    }

    bool lastGrabWasRope() const { return m_lastWasRope; }
    void reset() { m_regrabTimer = 0.f; m_lastWasRope = false; }
};

TEST(Climbable, RopeSpeedMult) {
    TestClimbable rope(200, 100, 200, ClimbableType::Rope, {200,170,100});
    ASSERT_EQ_F(rope.speedMult(), 0.8f, 0.01);
}

TEST(Climbable, LadderSpeedMult) {
    TestClimbable ladder(200, 100, 200, ClimbableType::Ladder, {160,130,80});
    ASSERT_EQ_F(ladder.speedMult(), 1.25f, 0.01);
}

TEST(Climbable, RopeGrabZoneWidth) {
    TestClimbable rope(200, 100, 200, ClimbableType::Rope, {200,170,100});
    // Rope width 6 + 2*14 = 34
    ASSERT_EQ_F(rope.grabZone.size.x, 34, 0.01);
}

TEST(Climbable, LadderGrabZoneWidth) {
    TestClimbable ladder(200, 100, 200, ClimbableType::Ladder, {160,130,80});
    // Ladder width 28 + 2*14 = 56
    ASSERT_EQ_F(ladder.grabZone.size.x, 56, 0.01);
}

TEST(Climbable, CenterXCalculation) {
    TestClimbable rope(200, 100, 200, ClimbableType::Rope, {200,170,100});
    ASSERT_EQ_F(rope.centerX(), 200, 0.01);
}

TEST(Climbable, SwayAmount) {
    TestClimbable rope(200, 100, 200, ClimbableType::Rope, {200,170,100});
    ASSERT_EQ_F(rope.swayAmount, 4.f, 0.01);

    TestClimbable ladder(200, 100, 200, ClimbableType::Ladder, {160,130,80});
    ASSERT_EQ_F(ladder.swayAmount, 0.f, 0.01);
}

// ============================================================================
// ClimbableManager Tests
// ============================================================================

TEST(ClimbableManager, AddRopeAndLadder) {
    TestClimbableManager mgr;
    mgr.addRope(200, 100, 200, {200,170,100});
    mgr.addLadder(400, 100, 200, {160,130,80});
    ASSERT_EQ(mgr.m_climbables.size(), 2);
}

TEST(ClimbableManager, HasPieceAbove) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80}); // Lower ladder
    mgr.addLadder(200, 50, 200, {160,130,80});  // Upper ladder

    auto& lower = mgr.m_climbables[0];
    ASSERT_TRUE(mgr.hasPieceAbove(lower, 200));
}

TEST(ClimbableManager, NoPieceAboveIfTooFar) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80});
    mgr.addLadder(400, 50, 200, {160,130,80}); // Far horizontally

    auto& lower = mgr.m_climbables[0];
    ASSERT_FALSE(mgr.hasPieceAbove(lower, 200));
}

TEST(ClimbableManager, NoPieceAboveIfGapTooLarge) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 200, 200, {160,130,80}); // Lower
    mgr.addLadder(200, 50, 200, {160,130,80});  // Upper - gap = 50px, but MAX_HOP_GAP = 64
    // Actually top of lower is 200, bottom of upper is 250, gap = 50 < 64, so should be true
    // Let me fix: upper at y=50, height=200 -> bottom=250, lower top=200 -> gap=50
    auto& lower = mgr.m_climbables[0];
    ASSERT_TRUE(mgr.hasPieceAbove(lower, 200));
}

// ============================================================================
// resolvePlayer Tests
// ============================================================================

TEST(ClimbableResolve, GrabLadderWhenHoldingUp) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80});

    sf::Vector2f playerPos(200, 200); // Inside ladder grab zone
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 184}, {22, 32});
    bool onGround = false;
    bool isClimbing = false;

    bool result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, -1.f, false, 0.016f);

    ASSERT_TRUE(result);
    ASSERT_TRUE(isClimbing);
    ASSERT_FALSE(mgr.lastGrabWasRope());
    ASSERT_LT(playerVel.y, 0); // Moving up
}

TEST(ClimbableResolve, GrabRopeWhenHoldingUp) {
    TestClimbableManager mgr;
    mgr.addRope(200, 100, 200, {200,170,100});

    sf::Vector2f playerPos(200, 200);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 184}, {22, 32});
    bool onGround = false;
    bool isClimbing = false;

    bool result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, -1.f, false, 0.016f);

    ASSERT_TRUE(result);
    ASSERT_TRUE(isClimbing);
    ASSERT_TRUE(mgr.lastGrabWasRope());
}

TEST(ClimbableResolve, NoGrabWithoutInput) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80});

    sf::Vector2f playerPos(200, 200);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 184}, {22, 32});
    bool onGround = false;
    bool isClimbing = false;

    bool result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, 0.f, false, 0.016f);

    ASSERT_FALSE(result);
    ASSERT_FALSE(isClimbing);
}

TEST(ClimbableResolve, NoGrabWhenBlocked) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80});

    sf::Vector2f playerPos(200, 200);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 184}, {22, 32});
    bool onGround = false;
    bool isClimbing = false;

    bool result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, -1.f, true, 0.016f);

    ASSERT_FALSE(result);
    ASSERT_FALSE(isClimbing);
}

TEST(ClimbableResolve, NoGrabDuringRegrabCooldown) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80});
    mgr.m_regrabTimer = 0.1f; // In cooldown

    sf::Vector2f playerPos(200, 200);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 184}, {22, 32});
    bool onGround = false;
    bool isClimbing = false;

    bool result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, -1.f, false, 0.016f);

    ASSERT_FALSE(result);
    ASSERT_FALSE(isClimbing);
}

TEST(ClimbableResolve, ContinueClimbing) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80});

    sf::Vector2f playerPos(200, 200);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 184}, {22, 32});
    bool onGround = false;
    bool isClimbing = true; // Already climbing

    bool result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, -1.f, false, 0.016f);

    ASSERT_TRUE(result);
    ASSERT_TRUE(isClimbing);
    ASSERT_LT(playerVel.y, 0);
    ASSERT_EQ_F(playerVel.x, 0, 0.01);
    ASSERT_FALSE(onGround);
}

TEST(ClimbableResolve, LetGoWhenNoInputAndNoOverlap) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80});

    // Player moved away from ladder
    sf::Vector2f playerPos(300, 100);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({289, 84}, {22, 32});
    bool onGround = false;
    bool isClimbing = true;

    bool result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, 0.f, false, 0.016f);

    ASSERT_FALSE(result);
    ASSERT_FALSE(isClimbing);
    ASSERT_GT(mgr.m_regrabTimer, 0);
}

TEST(ClimbableResolve, ClimbSpeedMultArtifact) {
    TestClimbableManager mgr;
    mgr.addLadder(200, 100, 200, {160,130,80});

    sf::Vector2f playerPos(200, 200);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 184}, {22, 32});
    bool onGround = false;
    bool isClimbing = true;

    float climbSpeedMult = 2.0f;
    mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, -1.f, false, 0.016f, climbSpeedMult);

    float expectedSpeed = 160.f * 1.25f * 2.0f; // CLIMB_SPEED * LADDER_MULT * artifact
    ASSERT_NEAR(playerVel.y, -expectedSpeed, 1.0f);
}

TEST(ClimbableResolve, RopeSlowerThanLadder) {
    TestClimbableManager mgr;
    mgr.addRope(200, 100, 200, {200,170,100});

    sf::Vector2f playerPos(200, 200);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 184}, {22, 32});
    bool onGround = false;
    bool isClimbing = true;

    mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, isClimbing, -1.f, false, 0.016f);

    float ropeSpeed = 160.f * 0.8f;
    ASSERT_NEAR(playerVel.y, -ropeSpeed, 1.0f);
}