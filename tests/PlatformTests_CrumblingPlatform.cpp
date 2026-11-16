#include "TestFramework.hpp"
#include <cmath>

// Mock SFML
namespace sf {
    struct Vector2f { float x, y; Vector2f(float x_=0, float y_=0):x(x_),y(y_){} };
    struct FloatRect { Vector2f position, size; FloatRect(Vector2f p={0,0}, Vector2f s={0,0}):position(p),size(s){} bool findIntersection(const FloatRect& o) const { return !(position.x+size.x<=o.position.x || o.position.x+o.size.x<=position.x || position.y+size.y<=o.position.y || o.position.y+o.size.y<=position.y); } };
    struct RectangleShape { Vector2f position, size; Vector2f getPosition() const {return position;} Vector2f getSize() const {return size;} FloatRect getGlobalBounds() const {return FloatRect(position,size);} void setPosition(Vector2f p){position=p;} void setSize(Vector2f s){size=s;} };
    struct Color { int r,g,b,a; Color(int r_,int g_,int b_,int a_=255):r(r_),g(g_),b(b_),a(a_){} };
    struct RenderWindow {};
}

// ============================================================================
// CrumblingPlatform Logic Tests
// ============================================================================

enum class CrumbleState { Solid, Shaking, Gone, Respawning };

struct TestCrumblingPlatform {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Color baseColor;
    CrumbleState state = CrumbleState::Solid;
    float timer = 0.f;
    float shakeOffset = 0.f;

    static constexpr float SHAKE_TIME = 0.4f;
    static constexpr float GONE_TIME = 1.8f;
    static constexpr float RESPAWN_TIME = 0.4f;

    TestCrumblingPlatform(float x, float y, float w, float h, sf::Color col)
        : origin({x,y}), baseColor(col) {
        shape.setSize({w,h});
        shape.setPosition({x,y});
    }

    void onLand() {
        if (state == CrumbleState::Solid) {
            state = CrumbleState::Shaking;
            timer = 0.f;
        }
    }

    void update(float dt, float crumbleSpeedMult = 1.f) {
        timer += dt * crumbleSpeedMult;

        switch (state) {
            case CrumbleState::Solid:
                break;

            case CrumbleState::Shaking: {
                float intensity = (timer / SHAKE_TIME) * 4.f;
                shakeOffset = std::sin(timer * 60.f) * intensity;
                shape.setPosition({origin.x + shakeOffset, origin.y});

                float t = timer / SHAKE_TIME;
                // color flash test - just verify it changes
                (void)t;

                if (timer >= SHAKE_TIME) {
                    state = CrumbleState::Gone;
                    timer = 0.f;
                }
                break;
            }

            case CrumbleState::Gone:
                if (timer >= GONE_TIME) {
                    state = CrumbleState::Respawning;
                    timer = 0.f;
                    shape.setPosition(origin);
                }
                break;

            case CrumbleState::Respawning: {
                float t = timer / RESPAWN_TIME;
                // alpha fade test
                (void)t;
                if (timer >= RESPAWN_TIME) {
                    state = CrumbleState::Solid;
                    timer = 0.f;
                }
                break;
            }
        }
    }

    bool isSolid() const { return state == CrumbleState::Solid || state == CrumbleState::Shaking; }
    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }

    void reset() {
        state = CrumbleState::Solid;
        timer = 0.f;
        shape.setPosition(origin);
    }
};

// Collision resolution for crumbling platforms
struct CrumbleResolveResult {
    sf::Vector2f playerPos;
    sf::Vector2f playerVel;
    bool onGround;
    bool dashAvail;
    int jumpsLeft;
    bool onCrumble;
};

CrumbleResolveResult resolveCrumblePlayer(TestCrumblingPlatform& cp, sf::Vector2f playerPos, sf::Vector2f playerVel,
                                           sf::FloatRect playerRect, bool onGround, bool dashAvail, int jumpsLeft) {
    bool onCrumble = false;

    if (!cp.isSolid()) return {playerPos, playerVel, onGround, dashAvail, jumpsLeft, false};

    sf::FloatRect pr = cp.bounds();
    if (!playerRect.findIntersection(pr)) return {playerPos, playerVel, onGround, dashAvail, jumpsLeft, false};

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

    float minOverlap = std::min({overlapTop, overlapBottom, overlapLeft, overlapRight});

    if (minOverlap == overlapTop && playerVel.y >= 0.f) {
        playerPos.y = platTop - playerRect.size.y / 2.f;
        playerVel.y = 0.f;
        onGround = true;
        dashAvail = true;
        jumpsLeft = 1;
        onCrumble = true;
        cp.onLand();  // trigger crumble
    } else if (minOverlap == overlapBottom && playerVel.y < 0.f) {
        playerPos.y = platBottom + playerRect.size.y / 2.f;
        playerVel.y = 0.f;
    } else if (minOverlap == overlapLeft) {
        playerPos.x = platLeft - playerRect.size.x / 2.f;
        playerVel.x = 0.f;
    } else if (minOverlap == overlapRight) {
        playerPos.x = platRight + playerRect.size.x / 2.f;
        playerVel.x = 0.f;
    }

    return {playerPos, playerVel, onGround, dashAvail, jumpsLeft, onCrumble};
}

TEST(CrumblingPlatform, StartsSolid) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Solid);
    ASSERT_TRUE(cp.isSolid());
}

TEST(CrumblingPlatform, OnLandTriggersShaking) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.onLand();
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Shaking);
    ASSERT_EQ_F(cp.timer, 0, 0.01);
    ASSERT_TRUE(cp.isSolid()); // Still solid while shaking
}

TEST(CrumblingPlatform, OnLandDoesNothingIfNotSolid) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.state = CrumbleState::Gone;
    cp.onLand();
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Gone); // No change
}

TEST(CrumblingPlatform, ShakingTransitionsToGone) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.onLand();
    cp.update(0.5f); // More than SHAKE_TIME (0.4s)
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Gone);
    ASSERT_FALSE(cp.isSolid());
}

TEST(CrumblingPlatform, GoneTransitionsToRespawning) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.onLand();
    cp.update(0.5f); // Gone
    cp.update(2.0f); // More than GONE_TIME (1.8s)
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Respawning);
    ASSERT_FALSE(cp.isSolid());
}

TEST(CrumblingPlatform, RespawningTransitionsToSolid) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.onLand();
    cp.update(0.5f); // Gone
    cp.update(2.0f); // Respawning
    cp.update(0.5f); // More than RESPAWN_TIME (0.4s)
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Solid);
    ASSERT_TRUE(cp.isSolid());
}

TEST(CrumblingPlatform, ResetRestoresSolid) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.onLand();
    cp.update(3.0f); // Fully cycled
    cp.reset();
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Solid);
    ASSERT_EQ_F(cp.timer, 0, 0.01);
    ASSERT_EQ_F(cp.shape.getPosition().x, 100, 0.01);
    ASSERT_EQ_F(cp.shape.getPosition().y, 200, 0.01);
}

TEST(CrumblingPlatform, ShakeOffsetCalculation) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.onLand();
    cp.update(0.1f); // In shaking phase
    // shakeOffset should be non-zero
    ASSERT_NE(cp.shakeOffset, 0.f);
}

TEST(CrumblingPlatform, CrumbleSpeedMultArtifact) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.onLand();

    // With 2x speed, should transition twice as fast
    cp.update(0.2f, 2.0f); // Effective 0.4s
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Gone);

    // With 0.5x speed, should take twice as long
    TestCrumblingPlatform cp2(100, 200, 64, 32, {180,120,60});
    cp2.onLand();
    cp2.update(0.2f, 0.5f); // Effective 0.1s
    ASSERT_EQ((int)cp2.state, (int)CrumbleState::Shaking);
}

// ============================================================================
// Collision Resolution Tests
// ============================================================================

TEST(CrumblingResolve, LandTriggersCrumble) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});

    auto result = resolveCrumblePlayer(cp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_TRUE(result.onGround);
    ASSERT_TRUE(result.onCrumble);
    ASSERT_EQ((int)cp.state, (int)CrumbleState::Shaking);
}

TEST(CrumblingResolve, NoCollisionWhenGone) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});
    cp.onLand();
    cp.update(3.0f); // Gone state

    sf::Vector2f playerPos(132, 184); // Inside platform bounds
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({121, 168}, {22, 32});

    auto result = resolveCrumblePlayer(cp, playerPos, playerVel, playerRect, false, false, 0);

    // Should pass through - no collision resolution
    ASSERT_FALSE(result.onGround);
    ASSERT_FALSE(result.onCrumble);
    ASSERT_EQ_F(result.playerPos.y, 184, 0.01);
}

TEST(CrumblingResolve, SideCollisionWhileSolid) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});

    sf::Vector2f playerPos(60, 184);
    sf::Vector2f playerVel(200, 0);
    sf::FloatRect playerRect({49, 168}, {22, 32});

    auto result = resolveCrumblePlayer(cp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_EQ_F(result.playerPos.x, 100 - 11, 0.1);
    ASSERT_EQ_F(result.playerVel.x, 0, 0.01);
}

TEST(CrumblingResolve, BonkHeadWhileSolid) {
    TestCrumblingPlatform cp(100, 200, 64, 32, {180,120,60});

    sf::Vector2f playerPos(132, 230);
    sf::Vector2f playerVel(0, -200);
    sf::FloatRect playerRect({121, 214}, {22, 32});

    auto result = resolveCrumblePlayer(cp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_EQ_F(result.playerPos.y, 232 + 16, 0.1);
    ASSERT_EQ_F(result.playerVel.y, 0, 0.01);
    ASSERT_FALSE(result.onGround);
}