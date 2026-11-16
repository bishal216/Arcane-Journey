#include "TestFramework.hpp"
#include <cmath>

namespace sf {
    struct Vector2f { float x, y; Vector2f(float x_=0, float y_=0):x(x_),y(y_){} };
    struct FloatRect { Vector2f position, size; FloatRect(Vector2f p={0,0}, Vector2f s={0,0}):position(p),size(s){} bool findIntersection(const FloatRect& o) const { return !(position.x+size.x<=o.position.x || o.position.x+o.size.x<=position.x || position.y+size.y<=o.position.y || o.position.y+o.size.y<=position.y); } };
    struct RectangleShape { Vector2f position, size; Vector2f getPosition() const {return position;} Vector2f getSize() const {return size;} FloatRect getGlobalBounds() const {return FloatRect(position,size);} void setPosition(Vector2f p){position=p;} void setSize(Vector2f s){size=s;} };
    struct Color { int r,g,b,a; Color(int r_,int g_,int b_,int a_=255):r(r_),g(g_),b(b_),a(a_){} };
    struct RenderWindow {};
}

// ============================================================================
// SinkingPlatform Logic Tests
// ============================================================================

enum class SinkState { Idle, Sinking, Rising };

struct TestSinkingPlatform {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Color baseColor;
    SinkState state = SinkState::Idle;
    float sinkOffset = 0.f;

    static constexpr float SINK_SPEED = 55.f;
    static constexpr float RISE_SPEED = 90.f;
    static constexpr float MAX_SINK = 80.f;

    TestSinkingPlatform(float x, float y, float w, float h, sf::Color col)
        : origin({x,y}), baseColor(col) {
        shape.setSize({w,h});
        shape.setPosition({x,y});
    }

    void update(float dt, bool playerOnTop) {
        switch (state) {
            case SinkState::Idle:
                if (playerOnTop) {
                    state = SinkState::Sinking;
                }
                break;

            case SinkState::Sinking: {
                sinkOffset += SINK_SPEED * dt;
                shape.setPosition({origin.x, origin.y + sinkOffset});

                if (sinkOffset >= MAX_SINK || !playerOnTop) {
                    state = SinkState::Rising;
                }
                break;
            }

            case SinkState::Rising: {
                sinkOffset -= RISE_SPEED * dt;
                if (sinkOffset <= 0.f) {
                    sinkOffset = 0.f;
                    state = SinkState::Idle;
                }
                shape.setPosition({origin.x, origin.y + sinkOffset});
                break;
            }
        }
    }

    bool isSolid() const { return true; }
    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }

    void reset() {
        state = SinkState::Idle;
        sinkOffset = 0.f;
        shape.setPosition(origin);
    }
};

struct TestSinkingManager {
    std::vector<TestSinkingPlatform> m_platforms;

    void add(float x, float y, float w, float h, sf::Color col) {
        m_platforms.emplace_back(x, y, w, h, col);
    }

    void update(float dt) {
        // No-op; real update in resolvePlayer
    }

    void draw(sf::RenderWindow&) const {}

    void reset() {
        for (auto& p : m_platforms) p.reset();
    }

    struct ResolveResult {
        sf::Vector2f playerPos;
        sf::Vector2f playerVel;
        bool onGround;
        bool dashAvail;
        int jumpsLeft;
    };

    ResolveResult resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                                bool& onGround, bool& dashAvail, int& jumpsLeft, float dt) {
        for (auto& sp : m_platforms) {
            sf::FloatRect pr = sp.bounds();
            if (!playerRect.findIntersection(pr)) {
                sp.update(dt, false);
                continue;
            }

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

            bool playerOnTop = (minOverlap == overlapTop && playerVel.y >= 0.f);

            if (playerOnTop) {
                playerPos.y = platTop - playerRect.size.y / 2.f;
                playerVel.y = 0.f;
                onGround = true;
                dashAvail = true;
                jumpsLeft = 1;
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

            sp.update(dt, playerOnTop);
            playerRect.position = {playerPos.x - playerRect.size.x / 2.f,
                                   playerPos.y - playerRect.size.y / 2.f};
        }

        return {playerPos, playerVel, onGround, dashAvail, jumpsLeft};
    }
};

TEST(SinkingPlatform, StartsIdle) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    ASSERT_EQ(sp.state, SinkState::Idle);
    ASSERT_EQ_F(sp.sinkOffset, 0, 0.01);
}

TEST(SinkingPlatform, SinksWhenPlayerOnTop) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    sp.update(0.1f, true); // Player on top
    ASSERT_EQ(sp.state, SinkState::Sinking);
    ASSERT_GT(sp.sinkOffset, 0);
    ASSERT_EQ_F(sp.shape.getPosition().y, 200 + sp.sinkOffset, 0.01);
}

TEST(SinkingPlatform, SinksAtSinkSpeed) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    sp.update(1.0f, true);
    ASSERT_NEAR(sp.sinkOffset, 55.f, 0.1f); // SINK_SPEED * 1s
}

TEST(SinkingPlatform, MaxSinkTriggersRising) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    sp.sinkOffset = 79.f;
    sp.state = SinkState::Sinking;
    sp.update(0.1f, true); // Should exceed MAX_SINK (80)
    ASSERT_EQ(sp.state, SinkState::Rising);
}

TEST(SinkingPlatform, PlayerStepsOffTriggersRising) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    sp.state = SinkState::Sinking;
    sp.sinkOffset = 30.f;
    sp.update(0.1f, false); // Player NOT on top
    ASSERT_EQ(sp.state, SinkState::Rising);
}

TEST(SinkingPlatform, RisesAtRiseSpeed) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    sp.state = SinkState::Rising;
    sp.sinkOffset = 80.f;
    sp.update(1.0f, false);
    ASSERT_NEAR(sp.sinkOffset, 80.f - 90.f, 0.1f); // RISE_SPEED * 1s
}

TEST(SinkingPlatform, RiseCompletesReturnsToIdle) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    sp.state = SinkState::Rising;
    sp.sinkOffset = 10.f;
    sp.update(0.2f, false); // 10/90 ≈ 0.11s to reach 0
    ASSERT_EQ(sp.state, SinkState::Idle);
    ASSERT_EQ_F(sp.sinkOffset, 0, 0.01);
    ASSERT_EQ_F(sp.shape.getPosition().y, 200, 0.01);
}

TEST(SinkingPlatform, ResetReturnsToIdle) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    sp.state = SinkState::Sinking;
    sp.sinkOffset = 50.f;
    sp.shape.setPosition({100, 250});
    sp.reset();
    ASSERT_EQ(sp.state, SinkState::Idle);
    ASSERT_EQ_F(sp.sinkOffset, 0, 0.01);
    ASSERT_EQ_F(sp.shape.getPosition().y, 200, 0.01);
}

TEST(SinkingPlatform, AlwaysSolid) {
    TestSinkingPlatform sp(100, 200, 64, 32, {100,150,80});
    ASSERT_TRUE(sp.isSolid());

    sp.state = SinkState::Sinking;
    ASSERT_TRUE(sp.isSolid());

    sp.state = SinkState::Rising;
    ASSERT_TRUE(sp.isSolid());
}

// ============================================================================
// Collision Resolution Tests
// ============================================================================

TEST(SinkingResolve, LandOnPlatformStartsSinking) {
    TestSinkingManager mgr;
    mgr.add(100, 200, 64, 32, {100,150,80});

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, 0.016f);

    ASSERT_TRUE(result.onGround);
    ASSERT_EQ(mgr.m_platforms[0].state, SinkState::Sinking);
}

TEST(SinkingResolve, PlatformSinksWhilePlayerStands) {
    TestSinkingManager mgr;
    mgr.add(100, 200, 64, 32, {100,150,80});

    sf::Vector2f playerPos(132, 184);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({121, 168}, {22, 32});
    bool onGround = true, dashAvail = true; int jumpsLeft = 1;

    // Simulate standing for 0.5s
    for (int i = 0; i < 30; ++i) {
        mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, 0.016f);
    }

    // Platform should have sunk ~2.64px (55 * 0.048s)
    ASSERT_GT(mgr.m_platforms[0].sinkOffset, 2.5f);
    // Player should be carried down with platform
    ASSERT_GT(playerPos.y, 184);
}

TEST(SinkingResolve, PlayerStepsOffPlatformRises) {
    TestSinkingManager mgr;
    mgr.add(100, 200, 64, 32, {100,150,80});

    // First, make it sink
    sf::Vector2f playerPos(132, 184);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({121, 168}, {22, 32});
    bool onGround = true, dashAvail = true; int jumpsLeft = 1;

    for (int i = 0; i < 20; ++i) {
        mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, 0.016f);
    }

    float sunkOffset = mgr.m_platforms[0].sinkOffset;
    ASSERT_GT(sunkOffset, 0);

    // Now player moves off horizontally
    playerRect.position.x = 300; // Far away
    onGround = false;

    mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, 0.016f);

    ASSERT_EQ(mgr.m_platforms[0].state, SinkState::Rising);
}

TEST(SinkingResolve, SideCollision) {
    TestSinkingManager mgr;
    mgr.add(100, 200, 64, 32, {100,150,80});

    sf::Vector2f playerPos(60, 184);
    sf::Vector2f playerVel(200, 0);
    sf::FloatRect playerRect({49, 168}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, 0.016f);

    ASSERT_EQ_F(result.playerPos.x, 100 - 11, 0.1);
    ASSERT_EQ_F(result.playerVel.x, 0, 0.01);
}

TEST(SinkingResolve, BonkHead) {
    TestSinkingManager mgr;
    mgr.add(100, 200, 64, 32, {100,150,80});

    sf::Vector2f playerPos(132, 230);
    sf::Vector2f playerVel(0, -200);
    sf::FloatRect playerRect({121, 214}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, 0.016f);

    ASSERT_EQ_F(result.playerPos.y, 232 + 16, 0.1);
    ASSERT_EQ_F(result.playerVel.y, 0, 0.01);
    ASSERT_FALSE(result.onGround);
}