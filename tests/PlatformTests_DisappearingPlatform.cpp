#include "TestFramework.hpp"
#include <cmath>

namespace sf {
    struct Vector2f { float x, y; Vector2f(float x_=0, float y_=0):x(x_),y(y_){} };
    struct FloatRect { Vector2f position, size; FloatRect(Vector2f p={0,0}, Vector2f s={0,0}):position(p),size(s){} bool findIntersection(const FloatRect& o) const { return !(position.x+size.x<=o.position.x || o.position.x+o.size.x<=position.x || position.y+size.y<=o.position.y || o.position.y+o.size.y<=position.y); } };
    struct RectangleShape { Vector2f position, size; Vector2f getPosition() const {return position;} Vector2f getSize() const {return size;} FloatRect getGlobalBounds() const {return FloatRect(position,size);} void setPosition(Vector2f p){position=p;} void setSize(Vector2f s){size=s;} void setFillColor(sf::Color){} };
    struct Color { int r,g,b,a; Color(int r_,int g_,int b_,int a_=255):r(r_),g(g_),b(b_),a(a_){} };
    struct RenderWindow {};
}

// ============================================================================
// DisappearingPlatform Logic Tests
// ============================================================================

enum class DisappearState { Visible, FadingOut, Hidden, FadingIn };

struct TestDisappearingPlatform {
    sf::RectangleShape shape;
    sf::Color baseColor;
    DisappearState state = DisappearState::Visible;
    float timer = 0.f;
    float visibleTime;
    float hiddenTime;
    float fadeTime;
    float phaseOffset;

    static constexpr float DEFAULT_VISIBLE = 2.0f;
    static constexpr float DEFAULT_HIDDEN = 1.5f;
    static constexpr float DEFAULT_FADE = 0.4f;

    TestDisappearingPlatform(float x, float y, float w, float h,
                             sf::Color col, float visTime = DEFAULT_VISIBLE,
                             float hidTime = DEFAULT_HIDDEN, float fade = DEFAULT_FADE,
                             float offset = 0.f)
        : baseColor(col), visibleTime(visTime), hiddenTime(hidTime), fadeTime(fade), phaseOffset(offset) {
        shape.setSize({w,h});
        shape.setPosition({x,y});
        timer = -offset; // Phase offset
    }

    void update(float dt, float disappearMult = 1.f) {
        timer += dt * disappearMult;

        float cycleTime = visibleTime + hiddenTime + 2.f * fadeTime;
        float cyclePos = std::fmod(timer, cycleTime);
        if (cyclePos < 0) cyclePos += cycleTime;

        if (cyclePos < visibleTime) {
            state = DisappearState::Visible;
        } else if (cyclePos < visibleTime + fadeTime) {
            state = DisappearState::FadingOut;
        } else if (cyclePos < visibleTime + fadeTime + hiddenTime) {
            state = DisappearState::Hidden;
        } else {
            state = DisappearState::FadingIn;
        }
    }

    bool isSolid() const {
        return state == DisappearState::Visible || state == DisappearState::FadingOut;
    }

    float getAlpha() const {
        float cycleTime = visibleTime + hiddenTime + 2.f * fadeTime;
        float cyclePos = std::fmod(timer, cycleTime);
        if (cyclePos < 0) cyclePos += cycleTime;

        if (cyclePos < visibleTime) return 1.f;
        if (cyclePos < visibleTime + fadeTime) {
            return 1.f - (cyclePos - visibleTime) / fadeTime;
        }
        if (cyclePos < visibleTime + fadeTime + hiddenTime) return 0.f;
        return (cyclePos - visibleTime - fadeTime - hiddenTime) / fadeTime;
    }

    sf::FloatRect bounds() const { return shape.getGlobalBounds(); }

    void reset() {
        timer = -phaseOffset;
    }
};

struct DisappearResolveResult {
    sf::Vector2f playerPos;
    sf::Vector2f playerVel;
    bool onGround;
    bool dashAvail;
    int jumpsLeft;
};

DisappearResolveResult resolveDisappearPlayer(TestDisappearingPlatform& dp, sf::Vector2f playerPos, sf::Vector2f playerVel,
                                               sf::FloatRect playerRect, bool onGround, bool dashAvail, int jumpsLeft) {
    if (!dp.isSolid()) return {playerPos, playerVel, onGround, dashAvail, jumpsLeft};

    sf::FloatRect pr = dp.bounds();
    if (!playerRect.findIntersection(pr)) return {playerPos, playerVel, onGround, dashAvail, jumpsLeft};

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

    return {playerPos, playerVel, onGround, dashAvail, jumpsLeft};
}

TEST(DisappearingPlatform, DefaultTimings) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200});
    ASSERT_EQ_F(dp.visibleTime, 2.0f, 0.01);
    ASSERT_EQ_F(dp.hiddenTime, 1.5f, 0.01);
    ASSERT_EQ_F(dp.fadeTime, 0.4f, 0.01);
}

TEST(DisappearingPlatform, CustomTimings) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200}, 3.0f, 2.0f, 0.5f);
    ASSERT_EQ_F(dp.visibleTime, 3.0f, 0.01);
    ASSERT_EQ_F(dp.hiddenTime, 2.0f, 0.01);
    ASSERT_EQ_F(dp.fadeTime, 0.5f, 0.01);
}

TEST(DisappearingPlatform, PhaseOffset) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200}, 2.0f, 1.5f, 0.4f, 1.0f);
    ASSERT_EQ_F(dp.timer, -1.0f, 0.01);
}

TEST(DisappearingPlatform, CycleStates) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200}, 2.0f, 1.5f, 0.4f);

    // Start: Visible
    dp.update(0.0f);
    ASSERT_EQ((int)dp.state, (int)DisappearState::Visible);

    // After visible time: FadingOut
    dp.update(2.1f);
    ASSERT_EQ((int)dp.state, (int)DisappearState::FadingOut);

    // After fade: Hidden
    dp.update(0.5f);
    ASSERT_EQ((int)dp.state, (int)DisappearState::Hidden);

    // After hidden: FadingIn
    dp.update(1.6f);
    ASSERT_EQ((int)dp.state, (int)DisappearState::FadingIn);

    // After fade in: Visible again
    dp.update(0.5f);
    ASSERT_EQ((int)dp.state, (int)DisappearState::Visible);
}

TEST(DisappearingPlatform, IsSolidInVisibleAndFadingOut) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200});

    dp.state = DisappearState::Visible;
    ASSERT_TRUE(dp.isSolid());

    dp.state = DisappearState::FadingOut;
    ASSERT_TRUE(dp.isSolid());

    dp.state = DisappearState::Hidden;
    ASSERT_FALSE(dp.isSolid());

    dp.state = DisappearState::FadingIn;
    ASSERT_FALSE(dp.isSolid());
}

TEST(DisappearingPlatform, AlphaCalculation) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200}, 2.0f, 1.5f, 0.4f);

    dp.timer = 0; // Visible
    ASSERT_EQ_F(dp.getAlpha(), 1.f, 0.01);

    dp.timer = 2.2f; // FadingOut (0.2 into 0.4 fade)
    ASSERT_NEAR(dp.getAlpha(), 0.5f, 0.01);

    dp.timer = 3.0f; // Hidden
    ASSERT_EQ_F(dp.getAlpha(), 0.f, 0.01);

    dp.timer = 4.7f; // FadingIn (0.2 into 0.4 fade)
    ASSERT_NEAR(dp.getAlpha(), 0.5f, 0.01);
}

TEST(DisappearingPlatform, ResetRestoresTimer) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200}, 2.0f, 1.5f, 0.4f, 1.0f);
    dp.update(10.0f); // Advance timer
    dp.reset();
    ASSERT_EQ_F(dp.timer, -1.0f, 0.01);
}

TEST(DisappearingPlatform, DisappearMultArtifact) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200}, 2.0f, 1.5f, 0.4f);

    // With 2x multiplier, cycle is twice as fast
    dp.update(1.0f, 2.0f); // Effective 2s
    // Should be in FadingOut (after 2s visible)
    ASSERT_EQ((int)dp.state, (int)DisappearState::FadingOut);
}

// ============================================================================
// Collision Resolution Tests
// ============================================================================

TEST(DisappearResolve, CollisionWhenSolid) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200});
    dp.state = DisappearState::Visible;

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});

    auto result = resolveDisappearPlayer(dp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_TRUE(result.onGround);
    ASSERT_EQ_F(result.playerVel.y, 0, 0.01);
}

TEST(DisappearResolve, NoCollisionWhenHidden) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200});
    dp.state = DisappearState::Hidden;

    sf::Vector2f playerPos(132, 184); // Inside platform
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({121, 168}, {22, 32});

    auto result = resolveDisappearPlayer(dp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_FALSE(result.onGround);
    ASSERT_EQ_F(result.playerPos.y, 184, 0.01);
}

TEST(DisappearResolve, NoCollisionWhenFadingIn) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200});
    dp.state = DisappearState::FadingIn;

    sf::Vector2f playerPos(132, 184);
    sf::Vector2f playerVel(0, 0);
    sf::FloatRect playerRect({121, 168}, {22, 32});

    auto result = resolveDisappearPlayer(dp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_FALSE(result.onGround);
}

TEST(DisappearResolve, CollisionWhenFadingOut) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200});
    dp.state = DisappearState::FadingOut;

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});

    auto result = resolveDisappearPlayer(dp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_TRUE(result.onGround);
}

TEST(DisappearResolve, SideCollision) {
    TestDisappearingPlatform dp(100, 200, 64, 32, {140,80,200});
    dp.state = DisappearState::Visible;

    sf::Vector2f playerPos(60, 184);
    sf::Vector2f playerVel(200, 0);
    sf::FloatRect playerRect({49, 168}, {22, 32});

    auto result = resolveDisappearPlayer(dp, playerPos, playerVel, playerRect, false, false, 0);

    ASSERT_EQ_F(result.playerPos.x, 100 - 11, 0.1);
    ASSERT_EQ_F(result.playerVel.x, 0, 0.01);
}