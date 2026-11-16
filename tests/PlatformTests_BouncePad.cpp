#include "TestFramework.hpp"
#include <cmath>

namespace sf {
    struct Vector2f { float x, y; Vector2f(float x_=0, float y_=0):x(x_),y(y_){} Vector2f operator*(float s) const {return {x*s,y*s};} };
    struct FloatRect { Vector2f position, size; FloatRect(Vector2f p={0,0}, Vector2f s={0,0}):position(p),size(s){} bool findIntersection(const FloatRect& o) const { return !(position.x+size.x<=o.position.x || o.position.x+o.size.x<=position.x || position.y+size.y<=o.position.y || o.position.y+o.size.y<=position.y); } };
    struct RectangleShape { Vector2f position, size; Vector2f getPosition() const {return position;} Vector2f getSize() const {return size;} FloatRect getGlobalBounds() const {return FloatRect(position,size);} void setPosition(Vector2f p){position=p;} void setSize(Vector2f s){size=s;} };
    struct Color { int r,g,b,a; Color(int r_,int g_,int b_,int a_=255):r(r_),g(g_),b(b_),a(a_){} };
    struct RenderWindow {};
}

// ============================================================================
// BouncePad Logic Tests
// ============================================================================

struct TestBouncePad {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Vector2f size;
    float bounceForce;
    float animTimer = 0.f;
    bool animActive = false;
    static constexpr float ANIM_DURATION = 0.3f;

    TestBouncePad(float x, float y, float w, float h, float force = 900.f)
        : origin({x,y}), size({w,h}), bounceForce(force) {
        shape.setSize({w,h});
        shape.setPosition({x,y});
    }

    void trigger() {
        animActive = true;
        animTimer = 0.f;
    }

    void update(float dt) {
        if (animActive) {
            animTimer += dt;
            if (animTimer >= ANIM_DURATION) {
                animActive = false;
                animTimer = 0.f;
            }
        }
    }

    float getSquashScale() const {
        if (!animActive) return 1.f;
        float t = animTimer / ANIM_DURATION;
        // Squash down then stretch up
        return 1.f - 0.3f * std::sin(t * 3.14159f * 2.f);
    }

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }
};

struct BounceResolveResult {
    sf::Vector2f playerPos;
    sf::Vector2f playerVel;
    bool onGround;
};

BounceResolveResult resolveBouncePlayer(TestBouncePad& bp, sf::Vector2f playerPos, sf::Vector2f playerVel,
                                         sf::FloatRect playerRect, bool onGround) {
    sf::FloatRect pr = bp.bounds();
    if (!playerRect.findIntersection(pr)) return {playerPos, playerVel, onGround};

    float playerBottom = playerRect.position.y + playerRect.size.y;
    float playerTop = playerRect.position.y;
    float platTop = pr.position.y;
    float platBottom = pr.position.y + pr.size.y;

    float overlapTop = playerBottom - platTop;
    float overlapBottom = platBottom - playerTop;

    // Only bounce if landing on top
    if (overlapTop < overlapBottom && playerVel.y > 0.f) {
        playerPos.y = platTop - playerRect.size.y / 2.f;
        playerVel.y = -bp.bounceForce;  // Launch upward
        onGround = true;
        bp.trigger();
    }

    return {playerPos, playerVel, onGround};
}

TEST(BouncePad, DefaultBounceForce) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);
    ASSERT_EQ_F(bp.bounceForce, 900, 0.01);
}

TEST(BouncePad, CustomBounceForce) {
    TestBouncePad bp(100, 200, 64, 16, 1200.f);
    ASSERT_EQ_F(bp.bounceForce, 1200, 0.01);
}

TEST(BouncePad, TriggerStartsAnimation) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);
    ASSERT_FALSE(bp.animActive);
    bp.trigger();
    ASSERT_TRUE(bp.animActive);
    ASSERT_EQ_F(bp.animTimer, 0, 0.01);
}

TEST(BouncePad, AnimationCompletes) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);
    bp.trigger();
    bp.update(0.4f); // More than ANIM_DURATION (0.3s)
    ASSERT_FALSE(bp.animActive);
}

TEST(BouncePad, SquashScaleDuringAnimation) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);
    bp.trigger();
    bp.update(0.0f);
    float scale = bp.getSquashScale();
    ASSERT_LT(scale, 1.0f); // Squashed
    ASSERT_GT(scale, 0.5f); // Not too squashed
}

TEST(BouncePad, NoSquashWhenNotAnimating) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);
    ASSERT_EQ_F(bp.getSquashScale(), 1.0f, 0.01);
}

// ============================================================================
// Collision Resolution Tests
// ============================================================================

TEST(BounceResolve, LandingOnTopTriggersBounce) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 300); // Falling
    sf::FloatRect playerRect({121, 154}, {22, 32});

    auto result = resolveBouncePlayer(bp, playerPos, playerVel, playerRect, false);

    ASSERT_TRUE(result.onGround);
    ASSERT_EQ_F(result.playerVel.y, -900, 0.01); // Launched upward
    ASSERT_TRUE(bp.animActive);
}

TEST(BounceResolve, HittingFromBelowDoesNotBounce) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);

    sf::Vector2f playerPos(132, 230); // Below platform
    sf::Vector2f playerVel(0, -300);  // Moving up
    sf::FloatRect playerRect({121, 214}, {22, 32});

    auto result = resolveBouncePlayer(bp, playerPos, playerVel, playerRect, false);

    ASSERT_FALSE(result.onGround);
    ASSERT_EQ_F(result.playerVel.y, -300, 0.01); // Velocity unchanged
    ASSERT_FALSE(bp.animActive);
}

TEST(BounceResolve, SideCollisionDoesNotBounce) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);

    sf::Vector2f playerPos(60, 184);
    sf::Vector2f playerVel(200, 0);
    sf::FloatRect playerRect({49, 168}, {22, 32});

    auto result = resolveBouncePlayer(bp, playerPos, playerVel, playerRect, false);

    ASSERT_FALSE(result.onGround);
    ASSERT_EQ_F(result.playerVel.y, 0, 0.01);
    ASSERT_FALSE(bp.animActive);
}

TEST(BounceResolve, BounceMultArtifact) {
    TestBouncePad bp(100, 200, 64, 16, 900.f);

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 300);
    sf::FloatRect playerRect({121, 154}, {22, 32});

    // Simulate artifact bounceMult
    float bounceMult = 1.5f;
    float modifiedForce = bp.bounceForce * bounceMult;

    // Manually apply modified force
    sf::FloatRect pr = bp.bounds();
    if (playerRect.findIntersection(pr)) {
        float playerBottom = playerRect.position.y + playerRect.size.y;
        float platTop = pr.position.y;
        float overlapTop = playerBottom - platTop;
        if (overlapTop < 32 && playerVel.y > 0.f) {
            playerVel.y = -modifiedForce;
        }
    }

    ASSERT_EQ_F(playerVel.y, -1350, 0.01);
}