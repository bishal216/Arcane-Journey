#include "TestFramework.hpp"

// We need to test the platform logic without SFML rendering.
// We'll test the core math and state transitions.

// Mock SFML types for testing
namespace sf {
    struct Vector2f {
        float x, y;
        Vector2f(float x_=0, float y_=0) : x(x_), y(y_) {}
        Vector2f operator+(const Vector2f& o) const { return {x+o.x, y+o.y}; }
        Vector2f operator-(const Vector2f& o) const { return {x-o.x, y-o.y}; }
        Vector2f operator*(float s) const { return {x*s, y*s}; }
        Vector2f operator/(float s) const { return {x/s, y/s}; }
    };
    struct FloatRect {
        Vector2f position;
        Vector2f size;
        FloatRect(Vector2f p={0,0}, Vector2f s={0,0}) : position(p), size(s) {}
        bool findIntersection(const FloatRect& other) const {
            return !(position.x + size.x <= other.position.x ||
                     other.position.x + other.size.x <= position.x ||
                     position.y + size.y <= other.position.y ||
                     other.position.y + other.size.y <= position.y);
        }
    };
    struct RectangleShape {
        Vector2f position;
        Vector2f size;
        Vector2f getPosition() const { return position; }
        Vector2f getSize() const { return size; }
        FloatRect getGlobalBounds() const { return FloatRect(position, size); }
        void setPosition(Vector2f p) { position = p; }
        void setSize(Vector2f s) { size = s; }
    };
    struct Color { int r,g,b,a; Color(int r_,int g_,int b_,int a_=255):r(r_),g(g_),b(b_),a(a_){} };
    struct RenderWindow {};
}

// Include the actual platform implementation
// We need to extract the logic from the implementation files
// For testing, we'll replicate the key logic here

// ============================================================================
// MovingPlatform Logic Tests
// ============================================================================

// Copied logic from MovingPlatform.cpp for testing
struct TestMovingPlatform {
    sf::RectangleShape shape;
    sf::Vector2f pointA, pointB, startPos;
    float speed;
    sf::Vector2f velocity;
    sf::Vector2f prevPos;
    sf::Vector2f m_dir;
    bool m_towardB = true;

    TestMovingPlatform(float x, float y, float w, float h, sf::Vector2f a, sf::Vector2f b, float spd)
        : pointA(a), pointB(b), startPos(x, y), speed(spd) {
        shape.setSize({w, h});
        shape.setPosition({x, y});
        sf::Vector2f d = pointB - pointA;
        float len = std::sqrt(d.x * d.x + d.y * d.y);
        m_dir = len > 0.f ? sf::Vector2f(d.x/len, d.y/len) : sf::Vector2f(1.f, 0.f);
        velocity = sf::Vector2f(m_dir.x * speed, m_dir.y * speed);
        prevPos = shape.getPosition();
    }

    void update(float dt, float platformSpeedMult = 1.f) {
        prevPos = shape.getPosition();
        sf::Vector2f pos = shape.getPosition();
        sf::Vector2f target = m_towardB ? pointB : pointA;
        sf::Vector2f toTarget = {target.x - pos.x, target.y - pos.y};
        float dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
        float effSpeed = speed * platformSpeedMult;

        if (dist < effSpeed * dt) {
            shape.setPosition(target);
            m_towardB = !m_towardB;
            sf::Vector2f d = { (m_towardB ? pointB : pointA).x - target.x,
                              (m_towardB ? pointB : pointA).y - target.y };
            float len = std::sqrt(d.x * d.x + d.y * d.y);
            if (len > 0.f) m_dir = sf::Vector2f(d.x/len, d.y/len);
        } else {
            m_dir = sf::Vector2f(toTarget.x/dist, toTarget.y/dist);
            shape.setPosition({ pos.x + m_dir.x * effSpeed * dt, pos.y + m_dir.y * effSpeed * dt });
        }
        velocity = sf::Vector2f(m_dir.x * speed, m_dir.y * speed);
    }

    void reset() {
        shape.setPosition(startPos);
        m_dir = sf::Vector2f(pointB.x - pointA.x, pointB.y - pointA.y);
        float len = std::sqrt(m_dir.x * m_dir.x + m_dir.y * m_dir.y);
        if (len > 0.f) m_dir = sf::Vector2f(m_dir.x/len, m_dir.y/len);
        m_towardB = true;
        velocity = sf::Vector2f(m_dir.x * speed, m_dir.y * speed);
        prevPos = startPos;
    }

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
};

TEST(MovingPlatform, HorizontalMovement) {
    TestMovingPlatform mp(100, 200, 64, 32, {100, 200}, {300, 200}, 100);

    // Should move toward pointB (right)
    ASSERT_TRUE(mp.m_towardB);
    ASSERT_EQ_F(mp.velocity.x, 100, 0.01);
    ASSERT_EQ_F(mp.velocity.y, 0, 0.01);

    mp.update(0.5f); // Move for 0.5s at 100 px/s = 50px
    ASSERT_EQ_F(mp.shape.getPosition().x, 150, 0.1);
    ASSERT_EQ_F(mp.shape.getPosition().y, 200, 0.1);
}

TEST(MovingPlatform, VerticalMovement) {
    TestMovingPlatform mp(200, 100, 32, 64, {200, 100}, {200, 300}, 80);

    ASSERT_TRUE(mp.m_towardB);
    ASSERT_EQ_F(mp.velocity.x, 0, 0.01);
    ASSERT_EQ_F(mp.velocity.y, 80, 0.01);

    mp.update(0.5f); // Move for 0.5s at 80 px/s = 40px
    ASSERT_EQ_F(mp.shape.getPosition().x, 200, 0.1);
    ASSERT_EQ_F(mp.shape.getPosition().y, 140, 0.1);
}

TEST(MovingPlatform, DirectionReversalAtPointB) {
    TestMovingPlatform mp(100, 200, 64, 32, {100, 200}, {200, 200}, 100);

    // Move to pointB
    mp.update(1.1f); // Should reach pointB (100px at 100px/s = 1s)
    ASSERT_FALSE(mp.m_towardB); // Should now be moving toward A
    ASSERT_EQ_F(mp.shape.getPosition().x, 200, 0.1);
    ASSERT_EQ_F(mp.velocity.x, -100, 0.01);
}

TEST(MovingPlatform, DirectionReversalAtPointA) {
    TestMovingPlatform mp(100, 200, 64, 32, {100, 200}, {200, 200}, 100);

    // Move to B and back to A
    mp.update(2.1f); // 1s to B, 1s back to A, 0.1s toward B again
    ASSERT_TRUE(mp.m_towardB);
    ASSERT_EQ_F(mp.shape.getPosition().x, 110, 0.1);
}

TEST(MovingPlatform, ResetRestoresState) {
    TestMovingPlatform mp(150, 200, 64, 32, {100, 200}, {300, 200}, 100);

    // Move somewhere
    mp.update(1.0f);
    ASSERT_NE(mp.shape.getPosition().x, 150);

    // Reset
    mp.reset();
    ASSERT_EQ_F(mp.shape.getPosition().x, 150, 0.01);
    ASSERT_EQ_F(mp.shape.getPosition().y, 200, 0.01);
    ASSERT_TRUE(mp.m_towardB);
    ASSERT_EQ_F(mp.velocity.x, 100, 0.01);
}

TEST(MovingPlatform, DiagonalMovement) {
    TestMovingPlatform mp(100, 100, 32, 32, {100, 100}, {200, 200}, 100 * std::sqrt(2));

    // Velocity should be normalized diagonal
    ASSERT_EQ_F(std::abs(mp.velocity.x), 100, 1);
    ASSERT_EQ_F(std::abs(mp.velocity.y), 100, 1);

    mp.update(0.5f); // Should move ~70px in each axis (0.5 * 100 * sqrt(2) * 1/sqrt(2))
    ASSERT_EQ_F(mp.shape.getPosition().x, 150, 1);
    ASSERT_EQ_F(mp.shape.getPosition().y, 150, 1);
}

TEST(MovingPlatform, PlatformSpeedMultArtifact) {
    TestMovingPlatform mp(100, 200, 64, 32, {100, 200}, {300, 200}, 100);

    // With 2x speed multiplier
    mp.update(0.5f, 2.0f);
    ASSERT_EQ_F(mp.shape.getPosition().x, 200, 0.1); // 100 * 2 * 0.5 = 100px

    // With 0.5x speed multiplier
    mp.reset();
    mp.update(0.5f, 0.5f);
    ASSERT_EQ_F(mp.shape.getPosition().x, 125, 0.1); // 100 * 0.5 * 0.5 = 25px
}

// ============================================================================
// MovingPlatformManager resolvePlayer Tests
// ============================================================================

// Simplified resolvePlayer logic for testing
struct ResolveResult {
    sf::Vector2f playerPos;
    sf::Vector2f playerVel;
    bool onGround;
    bool dashAvail;
    int jumpsLeft;
    bool carried;
};

ResolveResult resolveMovingPlayer(TestMovingPlatform& mp, sf::Vector2f playerPos, sf::Vector2f playerVel,
                                   sf::FloatRect playerRect, bool onGround, bool dashAvail, int jumpsLeft) {
    bool carried = false;
    sf::Vector2f prevPos = mp.prevPos;
    sf::Vector2f currPos = mp.shape.getPosition();
    sf::FloatRect pr = mp.bounds();

    if (playerRect.findIntersection(pr)) {
        float playerBottom = playerRect.position.y + playerRect.size.y;
        float playerTop = playerRect.position.y;
        float platTop = pr.position.y;
        float platBottom = pr.position.y + pr.size.y;
        float playerLeft = playerRect.position.x;
        float playerRight = playerRect.position.x + playerRect.size.x;
        float platLeft = pr.position.x;
        float platRight = pr.position.x + pr.size.x;

        float overlapTop = playerBottom - platTop;
        float overlapBottom = platBottom - playerTop;
        float overlapLeft = playerRight - platLeft;
        float overlapRight = platRight - playerLeft;

        float platDeltaY = currPos.y - prevPos.y;
        bool platMovedUp = platDeltaY < 0.f;
        float effectiveOverlapTop = overlapTop;
        if (platMovedUp) effectiveOverlapTop -= std::abs(platDeltaY);

        float minOverlap = std::min({effectiveOverlapTop, overlapBottom, overlapLeft, overlapRight});

        if (minOverlap == effectiveOverlapTop && playerVel.y >= -mp.speed - 10.f) {
            playerPos.y = platTop - playerRect.size.y / 2.f;
            playerVel.y = 0.f;
            onGround = true;
            dashAvail = true;
            jumpsLeft = 1;
            carried = true;
            sf::Vector2f delta = {currPos.x - prevPos.x, currPos.y - prevPos.y};
            playerPos.x += delta.x;
            playerPos.y += delta.y;
        } else if (minOverlap == overlapBottom && playerVel.y < 0.f) {
            playerPos.y = platBottom + playerRect.size.y / 2.f;
            playerVel.y = 0.f;
            if (platDeltaY > 0.f) playerPos.y += platDeltaY;
        } else if (minOverlap == overlapLeft) {
            playerPos.x = platLeft - playerRect.size.x / 2.f;
            playerVel.x = 0.f;
        } else if (minOverlap == overlapRight) {
            playerPos.x = platRight + playerRect.size.x / 2.f;
            playerVel.x = 0.f;
        }
    }

    return {playerPos, playerVel, onGround, dashAvail, jumpsLeft, carried};
}

TEST(MovingPlatformResolve, LandOnHorizontalPlatform) {
    TestMovingPlatform mp(100, 200, 64, 32, {100, 200}, {300, 200}, 100);
    mp.update(0.0f); // Initialize prevPos

    sf::Vector2f playerPos(132, 170); // Above platform
    sf::Vector2f playerVel(0, 200);   // Falling
    sf::FloatRect playerRect({132 - 11, 170 - 16}, {22, 32}); // BODY_W=22, BODY_H=32

    auto result = resolveMovingPlayer(mp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_TRUE(result.onGround);
    ASSERT_TRUE(result.dashAvail);
    ASSERT_EQ(result.jumpsLeft, 1);
    ASSERT_EQ_F(result.playerVel.y, 0, 0.01);
    ASSERT_EQ_F(result.playerPos.y, 200 - 16, 0.1); // On top of platform
}

TEST(MovingPlatformResolve, CarryPlayerHorizontally) {
    TestMovingPlatform mp(100, 200, 64, 32, {100, 200}, {300, 200}, 100);
    mp.update(0.0f);

    // Player on platform
    sf::Vector2f playerPos(132, 184);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({121, 168}, {22, 32});

    // Move platform
    mp.update(0.1f); // Moves 10px right

    auto result = resolveMovingPlayer(mp, playerPos, playerVel, playerRect, true, true, 1);

    ASSERT_TRUE(result.carried);
    // Player should be carried 10px right
    ASSERT_EQ_F(result.playerPos.x, 132 + 10, 0.5);
}

TEST(MovingPlatformResolve, CarryPlayerVertically) {
    TestMovingPlatform mp(200, 100, 32, 64, {200, 100}, {200, 300}, 80);
    mp.update(0.0f);

    sf::Vector2f playerPos(200, 84);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 68}, {22, 32});

    mp.update(0.1f); // Moves 8px down

    auto result = resolveMovingPlayer(mp, playerPos, playerVel, playerRect, true, true, 1);

    ASSERT_TRUE(result.carried);
    ASSERT_EQ_F(result.playerPos.y, 84 + 8, 0.5);
}

TEST(MovingPlatformResolve, BonkHeadOnUnderside) {
    TestMovingPlatform mp(100, 200, 64, 32, {100, 200}, {300, 200}, 100);
    mp.update(0.0f);

    // Player hitting from below
    sf::Vector2f playerPos(132, 230); // Below platform (platform top at 200, bottom at 232)
    sf::Vector2f playerVel(0, -200);  // Moving up
    sf::FloatRect playerRect({121, 214}, {22, 32});

    auto result = resolveMovingPlayer(mp, playerPos, playerVel, playerRect, false, false, 0);

    // Should be pushed to platform bottom + half height
    ASSERT_EQ_F(result.playerPos.y, 232 + 16, 0.1);
    ASSERT_EQ_F(result.playerVel.y, 0, 0.01);
    ASSERT_FALSE(result.onGround);
}

TEST(MovingPlatformResolve, SideCollision) {
    TestMovingPlatform mp(100, 200, 64, 32, {100, 200}, {300, 200}, 100);
    mp.update(0.0f);

    // Player hitting from left side
    sf::Vector2f playerPos(60, 184);
    sf::Vector2f playerVel(200, 0);
    sf::FloatRect playerRect({49, 168}, {22, 32});

    auto result = resolveMovingPlayer(mp, playerPos, playerVel, playerRect, false, false, 0);

    // Should be pushed to platform left - half width
    ASSERT_EQ_F(result.playerPos.x, 100 - 11, 0.1);
    ASSERT_EQ_F(result.playerVel.x, 0, 0.01);
}

TEST(MovingPlatformResolve, VerticalPlatformRisingIntoPlayer) {
    // Test the fix for vertical platforms moving up into player
    TestMovingPlatform mp(200, 200, 32, 64, {200, 100}, {200, 300}, 100);
    // Move platform up so it's at y=200, moving toward pointA (y=100)
    mp.m_towardB = false;
    mp.update(0.0f); // Initialize

    // Platform moves UP, player is on top
    sf::Vector2f playerPos(200, 168); // On top of platform at y=200
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({189, 152}, {22, 32});

    // Move platform UP by 10px
    mp.update(0.1f); // Moves toward pointA (y=100), so Y decreases

    // The platform moved up, player should still land on top
    auto result = resolveMovingPlayer(mp, playerPos, playerVel, playerRect, true, true, 1);

    ASSERT_TRUE(result.onGround);
    ASSERT_TRUE(result.carried);
    // Player should be carried UP with platform
    ASSERT_LT(result.playerPos.y, 168);
}

// Helper macro for less-than comparison
#define ASSERT_LT(a, b) \
    do { if (!((a) < (b))) { \
        std::cerr << "FAIL: " << __FILE__ << ":" << __LINE__ \
                  << ": Expected " #a " < " #b " (" << (a) << " vs " << (b) << ")\n"; \
    } } while(0)