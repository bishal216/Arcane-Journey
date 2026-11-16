#include "TestFramework.hpp"
#include <cmath>

namespace sf {
    struct Vector2f { float x, y; Vector2f(float x_=0, float y_=0):x(x_),y(y_){} };
    struct FloatRect { Vector2f position, size; FloatRect(Vector2f p={0,0}, Vector2f s={0,0}):position(p),size(s){} bool findIntersection(const FloatRect& o) const { return !(position.x+size.x<=o.position.x || o.position.x+o.size.x<=position.x || position.y+size.y<=o.position.y || o.position.y+o.size.y<=position.y); } };
    struct RectangleShape { Vector2f position, size; Vector2f getPosition() const {return position;} Vector2f getSize() const {return size;} FloatRect getGlobalBounds() const {return FloatRect(position,size);} void setPosition(Vector2f p){position=p;} void setSize(Vector2f s){size=s;} void setFillColor(sf::Color){} void setOutlineColor(sf::Color){} void setOutlineThickness(float){}};
    struct Color { int r,g,b,a; Color(int r_,int g_,int b_,int a_=255):r(r_),g(g_),b(b_),a(a_){} };
    struct RenderWindow {};
}

// ============================================================================
// Trapdoor Logic Tests
// ============================================================================

enum class TrapdoorStyle { OneWay, Hatch };
enum class HatchState { Closed, Opening, Open, Closing };

struct TestTrapdoor {
    sf::RectangleShape shape;
    sf::Vector2f origin;
    sf::Color baseColor;
    TrapdoorStyle style;
    HatchState hatchState = HatchState::Closed;
    float hatchTimer = 0.f;
    float openAngle = 0.f;

    static constexpr float OPEN_TIME = 0.18f;
    static constexpr float STAY_TIME = 0.55f;
    static constexpr float CLOSE_TIME = 0.25f;

    TestTrapdoor(float x, float y, float w, float h, TrapdoorStyle s, sf::Color col)
        : origin({x,y}), baseColor(col), style(s) {
        shape.setSize({w,h});
        shape.setPosition({x,y});
    }

    void onLand() {
        if (style == TrapdoorStyle::Hatch && hatchState == HatchState::Closed) {
            hatchState = HatchState::Opening;
            hatchTimer = 0.f;
        }
    }

    void update(float dt) {
        if (style != TrapdoorStyle::Hatch) return;

        hatchTimer += dt;
        switch (hatchState) {
            case HatchState::Opening: {
                float t = hatchTimer / OPEN_TIME;
                openAngle = t * 90.f;
                if (hatchTimer >= OPEN_TIME) {
                    hatchState = HatchState::Open;
                    hatchTimer = 0.f;
                    openAngle = 90.f;
                }
                break;
            }
            case HatchState::Open: {
                if (hatchTimer >= STAY_TIME) {
                    hatchState = HatchState::Closing;
                    hatchTimer = 0.f;
                }
                break;
            }
            case HatchState::Closing: {
                float t = hatchTimer / CLOSE_TIME;
                openAngle = 90.f - t * 90.f;
                if (hatchTimer >= CLOSE_TIME) {
                    hatchState = HatchState::Closed;
                    hatchTimer = 0.f;
                    openAngle = 0.f;
                }
                break;
            }
            case HatchState::Closed:
            default:
                break;
        }
    }

    bool isSolidFromAbove() const {
        if (style == TrapdoorStyle::OneWay) return true;
        return hatchState == HatchState::Closed || hatchState == HatchState::Closing;
    }

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }

    void reset() {
        hatchState = HatchState::Closed;
        hatchTimer = 0.f;
        openAngle = 0.f;
    }
};

struct TestTrapdoorManager {
    std::vector<TestTrapdoor> m_trapdoors;
    bool m_standingOnOneWay = false;

    void addOneWay(float x, float y, float w, float h, sf::Color col) {
        m_trapdoors.emplace_back(x, y, w, h, TrapdoorStyle::OneWay, col);
    }
    void addHatch(float x, float y, float w, float h, sf::Color col) {
        m_trapdoors.emplace_back(x, y, w, h, TrapdoorStyle::Hatch, col);
    }

    void update(float dt) {
        for (auto& t : m_trapdoors) t.update(dt);
    }

    void reset() {
        for (auto& t : m_trapdoors) t.reset();
    }

    struct ResolveResult {
        sf::Vector2f playerPos;
        sf::Vector2f playerVel;
        bool onGround;
        bool dashAvail;
        int jumpsLeft;
        bool standingOnOneWay;
    };

    ResolveResult resolvePlayer(sf::Vector2f& playerPos, sf::Vector2f& playerVel, sf::FloatRect playerRect,
                                bool& onGround, bool& dashAvail, int& jumpsLeft, bool dropping) {
        m_standingOnOneWay = false;

        for (auto& td : m_trapdoors) {
            // One-way platforms are ignored when dropping
            if (dropping && td.style == TrapdoorStyle::OneWay) continue;

            sf::FloatRect pr = td.bounds();
            if (!playerRect.findIntersection(pr)) continue;

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
                // Check if solid from above
                if (td.isSolidFromAbove()) {
                    playerPos.y = platTop - playerRect.size.y / 2.f;
                    playerVel.y = 0.f;
                    onGround = true;
                    dashAvail = true;
                    jumpsLeft = 1;
                    if (td.style == TrapdoorStyle::OneWay) m_standingOnOneWay = true;
                    td.onLand(); // Trigger hatch
                }
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

            playerRect.position = {playerPos.x - playerRect.size.x / 2.f,
                                   playerPos.y - playerRect.size.y / 2.f};
        }

        return {playerPos, playerVel, onGround, dashAvail, jumpsLeft, m_standingOnOneWay};
    }

    bool standingOnOneWay() const { return m_standingOnOneWay; }
};

TEST(Trapdoor, OneWayStyle) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::OneWay, {120,80,40});
    ASSERT_EQ(td.style, TrapdoorStyle::OneWay);
    ASSERT_TRUE(td.isSolidFromAbove());
}

TEST(Trapdoor, HatchStyleClosed) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::Hatch, {100,70,35});
    ASSERT_EQ(td.style, TrapdoorStyle::Hatch);
    ASSERT_TRUE(td.isSolidFromAbove());
}

TEST(Trapdoor, HatchStyleOpening) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::Hatch, {100,70,35});
    td.hatchState = HatchState::Opening;
    ASSERT_FALSE(td.isSolidFromAbove());
}

TEST(Trapdoor, HatchStyleOpen) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::Hatch, {100,70,35});
    td.hatchState = HatchState::Open;
    ASSERT_FALSE(td.isSolidFromAbove());
}

TEST(Trapdoor, HatchStyleClosing) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::Hatch, {100,70,35});
    td.hatchState = HatchState::Closing;
    ASSERT_TRUE(td.isSolidFromAbove());
}

TEST(Trapdoor, OnLandTriggersHatch) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::Hatch, {100,70,35});
    td.onLand();
    ASSERT_EQ(td.hatchState, HatchState::Opening);
    ASSERT_EQ_F(td.hatchTimer, 0, 0.01);
}

TEST(Trapdoor, OnLandDoesNothingIfNotClosed) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::Hatch, {100,70,35});
    td.hatchState = HatchState::Open;
    td.onLand();
    ASSERT_EQ(td.hatchState, HatchState::Open); // No change
}

TEST(Trapdoor, HatchAnimationCycle) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::Hatch, {100,70,35});
    td.onLand();

    td.update(0.1f); // Opening
    ASSERT_EQ(td.hatchState, HatchState::Opening);
    ASSERT_GT(td.openAngle, 0);
    ASSERT_LT(td.openAngle, 90);

    td.update(0.1f); // Total 0.2 > OPEN_TIME (0.18) -> Open
    ASSERT_EQ(td.hatchState, HatchState::Open);
    ASSERT_EQ_F(td.openAngle, 90, 0.01);

    td.update(0.6f); // Total 0.6 > STAY_TIME (0.55) -> Closing
    ASSERT_EQ(td.hatchState, HatchState::Closing);

    td.update(0.3f); // Total 0.3 > CLOSE_TIME (0.25) -> Closed
    ASSERT_EQ(td.hatchState, HatchState::Closed);
    ASSERT_EQ_F(td.openAngle, 0, 0.01);
}

TEST(Trapdoor, OneWayHasNoAnimation) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::OneWay, {120,80,40});
    td.onLand();
    td.update(1.0f);
    ASSERT_EQ(td.hatchState, HatchState::Closed);
    ASSERT_EQ_F(td.openAngle, 0, 0.01);
}

TEST(Trapdoor, ResetClosesHatch) {
    TestTrapdoor td(100, 200, 64, 16, TrapdoorStyle::Hatch, {100,70,35});
    td.onLand();
    td.update(1.0f);
    td.reset();
    ASSERT_EQ(td.hatchState, HatchState::Closed);
    ASSERT_EQ_F(td.hatchTimer, 0, 0.01);
    ASSERT_EQ_F(td.openAngle, 0, 0.01);
}

// ============================================================================
// Collision Resolution Tests
// ============================================================================

TEST(TrapdoorResolve, OneWaySolidFromAbove) {
    TestTrapdoorManager mgr;
    mgr.addOneWay(100, 200, 64, 16, {120,80,40});

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, false);

    ASSERT_TRUE(result.onGround);
    ASSERT_TRUE(result.standingOnOneWay);
}

TEST(TrapdoorResolve, OneWayPassableFromBelow) {
    TestTrapdoorManager mgr;
    mgr.addOneWay(100, 200, 64, 16, {120,80,40});

    sf::Vector2f playerPos(132, 230); // Below platform
    sf::Vector2f playerVel(0, -200);  // Moving up
    sf::FloatRect playerRect({121, 214}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, false);

    ASSERT_FALSE(result.onGround);
    ASSERT_FALSE(result.standingOnOneWay);
    // Should pass through - position unchanged
    ASSERT_EQ_F(result.playerPos.y, 230, 0.01);
}

TEST(TrapdoorResolve, OneWayIgnoredWhenDropping) {
    TestTrapdoorManager mgr;
    mgr.addOneWay(100, 200, 64, 16, {120,80,40});

    sf::Vector2f playerPos(132, 170); // Above platform
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    // dropping = true
    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, true);

    ASSERT_FALSE(result.onGround);
    ASSERT_FALSE(result.standingOnOneWay);
    // Falls through
}

TEST(TrapdoorResolve, HatchSolidWhenClosed) {
    TestTrapdoorManager mgr;
    mgr.addHatch(100, 200, 64, 16, {100,70,35});

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, false);

    ASSERT_TRUE(result.onGround);
}

TEST(TrapdoorResolve, HatchPassableWhenOpen) {
    TestTrapdoorManager mgr;
    mgr.addHatch(100, 200, 64, 16, {100,70,35});
    mgr.m_trapdoors[0].hatchState = HatchState::Open;

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, false);

    ASSERT_FALSE(result.onGround);
}

TEST(TrapdoorResolve, HatchTriggersOnLand) {
    TestTrapdoorManager mgr;
    mgr.addHatch(100, 200, 64, 16, {100,70,35});

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, false);

    ASSERT_TRUE(result.onGround);
    ASSERT_EQ(mgr.m_trapdoors[0].hatchState, HatchState::Opening);
}

TEST(TrapdoorResolve, SideCollision) {
    TestTrapdoorManager mgr;
    mgr.addOneWay(100, 200, 64, 16, {120,80,40});

    sf::Vector2f playerPos(60, 184);
    sf::Vector2f playerVel(200, 0);
    sf::FloatRect playerRect({49, 168}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    auto result = mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft, false);

    ASSERT_EQ_F(result.playerPos.x, 100 - 11, 0.1);
    ASSERT_EQ_F(result.playerVel.x, 0, 0.01);
}