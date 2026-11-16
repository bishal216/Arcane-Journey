#include "TestFramework.hpp"
#include <vector>
#include <cmath>

namespace sf {
    struct Vector2f { float x, y; Vector2f(float x_=0, float y_=0):x(x_),y(y_){} };
    struct FloatRect { Vector2f position, size; FloatRect(Vector2f p={0,0}, Vector2f s={0,0}):position(p),size(s){} bool findIntersection(const FloatRect& o) const { return !(position.x+size.x<=o.position.x || o.position.x+o.size.x<=position.x || position.y+size.y<=o.position.y || o.position.y+o.size.y<=position.y); } };
    struct RectangleShape { Vector2f position, size; Vector2f getPosition() const {return position;} Vector2f getSize() const {return size;} FloatRect getGlobalBounds() const {return FloatRect(position,size);} void setPosition(Vector2f p){position=p;} void setSize(Vector2f s){size=s;} void setFillColor(sf::Color){} void setOutlineColor(sf::Color){} void setOutlineThickness(float){}};
    struct Color { int r,g,b,a; Color(int r_,int g_,int b_,int a_=255):r(r_),g(g_),b(b_),a(a_){} };
    struct RenderWindow {};
}

// ============================================================================
// TeleportBlock Logic Tests
// ============================================================================

struct TestTeleportBlock {
    sf::FloatRect bounds;
    sf::Vector2f spawnPos;
    int pairIndex = -1;
    float chargeTimer = 0.f;
    float cooldown = 0.f;
    bool playerOn = false;

    TestTeleportBlock(float x, float y, float w, float h)
        : bounds({x,y}, {w,h}), spawnPos({x + w/2.f, y - 40.f}) {}

    static constexpr float CHARGE_TIME = 0.5f;
    static constexpr float COOLDOWN = 1.2f;

    void update(float dt, sf::Vector2f& playerPos, bool playerOnGround) {
        bool wasOn = playerOn;
        playerOn = false;

        // Check if player is standing on this block
        if (playerOnGround) {
            sf::FloatRect playerRect({playerPos.x - 11, playerPos.y - 16}, {22, 32});
            if (bounds.findIntersection(playerRect)) {
                // Check if player bottom is at or near block top
                float playerBottom = playerRect.position.y + playerRect.size.y;
                float blockTop = bounds.position.y;
                if (playerBottom >= blockTop - 2.f && playerBottom <= blockTop + 10.f) {
                    playerOn = true;
                }
            }
        }

        if (playerOn) {
            if (!wasOn) chargeTimer = 0.f;
            chargeTimer += dt;
        } else {
            chargeTimer = 0.f;
        }

        if (cooldown > 0.f) cooldown -= dt;
    }

    void reset() {
        chargeTimer = 0.f;
        cooldown = 0.f;
        playerOn = false;
    }

    bool isReady() const { return chargeTimer >= CHARGE_TIME && cooldown <= 0.f && pairIndex >= 0; }
    float getChargeProgress() const { return std::min(1.f, chargeTimer / CHARGE_TIME); }
};

struct TestTeleportManager {
    std::vector<TestTeleportBlock> m_blocks;

    void add(float x, float y, float w, float h) {
        m_blocks.emplace_back(x, y, w, h);
    }

    int pairAll() {
        int unpaired = 0;
        for (size_t i = 0; i + 1 < m_blocks.size(); i += 2) {
            m_blocks[i].pairIndex = (int)(i + 1);
            m_blocks[i + 1].pairIndex = (int)i;
        }
        if (m_blocks.size() % 2 == 1) {
            m_blocks.back().pairIndex = -1;
            unpaired = 1;
        }
        return unpaired;
    }

    void update(float dt, sf::Vector2f& playerPos, bool playerOnGround) {
        for (auto& b : m_blocks) b.update(dt, playerPos, playerOnGround);
    }

    void reset() {
        for (auto& b : m_blocks) b.reset();
    }

    // Collision resolution (solid from top only)
    void resolvePlayer(sf::Vector2f& pos, sf::Vector2f& vel, sf::FloatRect playerBounds,
                       bool& onGround, bool& dashAvail, int& jumpsLeft) {
        for (auto& b : m_blocks) {
            if (!playerBounds.findIntersection(b.bounds)) continue;

            float playerBottom = playerBounds.position.y + playerBounds.size.y;
            float playerTop = playerBounds.position.y;
            float platTop = b.bounds.position.y;
            float platBottom = b.bounds.position.y + b.bounds.size.y;
            float playerLeft = playerBounds.position.x;
            float playerRight = playerBounds.position.x + playerBounds.size.x;
            float platLeft = b.bounds.position.x;
            float platRight = b.bounds.position.x + b.bounds.size.x;

            float overlapTop = playerBottom - platTop;
            float overlapBottom = platBottom - playerTop;
            float overlapLeft = playerRight - platLeft;
            float overlapRight = platRight - playerLeft;

            float minOverlap = std::min({overlapTop, overlapBottom, overlapLeft, overlapRight});

            if (minOverlap == overlapTop && vel.y >= 0.f) {
                pos.y = platTop - playerBounds.size.y / 2.f;
                vel.y = 0.f;
                onGround = true;
                dashAvail = true;
                jumpsLeft = 1;
            } else if (minOverlap == overlapBottom && vel.y < 0.f) {
                pos.y = platBottom + playerBounds.size.y / 2.f;
                vel.y = 0.f;
            } else if (minOverlap == overlapLeft) {
                pos.x = platLeft - playerBounds.size.x / 2.f;
                vel.x = 0.f;
            } else if (minOverlap == overlapRight) {
                pos.x = platRight + playerBounds.size.x / 2.f;
                vel.x = 0.f;
            }

            playerBounds.position = {pos.x - playerBounds.size.x / 2.f,
                                     pos.y - playerBounds.size.y / 2.f};
        }
    }

    // Check for teleport trigger
    bool checkTeleport(sf::Vector2f& playerPos) {
        for (auto& b : m_blocks) {
            if (b.isReady()) {
                // Teleport!
                int partnerIdx = b.pairIndex;
                if (partnerIdx >= 0 && partnerIdx < (int)m_blocks.size()) {
                    playerPos = m_blocks[partnerIdx].spawnPos;
                    b.cooldown = TestTeleportBlock::COOLDOWN;
                    m_blocks[partnerIdx].cooldown = TestTeleportBlock::COOLDOWN;
                    return true;
                }
            }
        }
        return false;
    }
};

TEST(TeleportBlock, PairingSequential) {
    TestTeleportManager mgr;
    mgr.add(100, 200, 32, 32); // 0
    mgr.add(300, 200, 32, 32); // 1
    mgr.add(500, 200, 32, 32); // 2
    mgr.add(700, 200, 32, 32); // 3

    int unpaired = mgr.pairAll();
    ASSERT_EQ(unpaired, 0);
    ASSERT_EQ(mgr.m_blocks[0].pairIndex, 1);
    ASSERT_EQ(mgr.m_blocks[1].pairIndex, 0);
    ASSERT_EQ(mgr.m_blocks[2].pairIndex, 3);
    ASSERT_EQ(mgr.m_blocks[3].pairIndex, 2);
}

TEST(TeleportBlock, OddCountHasUnpaired) {
    TestTeleportManager mgr;
    mgr.add(100, 200, 32, 32); // 0
    mgr.add(300, 200, 32, 32); // 1
    mgr.add(500, 200, 32, 32); // 2

    int unpaired = mgr.pairAll();
    ASSERT_EQ(unpaired, 1);
    ASSERT_EQ(mgr.m_blocks[0].pairIndex, 1);
    ASSERT_EQ(mgr.m_blocks[1].pairIndex, 0);
    ASSERT_EQ(mgr.m_blocks[2].pairIndex, -1); // Unpaired
}

TEST(TeleportBlock, ChargeTimer) {
    TestTeleportBlock tb(100, 200, 32, 32);
    tb.pairIndex = 1;

    sf::Vector2f playerPos(116, 184); // On top of block
    tb.update(0.3f, playerPos, true); // Player standing on it
    ASSERT_EQ_F(tb.chargeTimer, 0.3f, 0.01);
    ASSERT_FALSE(tb.isReady());

    tb.update(0.3f, playerPos, true); // Total 0.6f > CHARGE_TIME (0.5f)
    ASSERT_TRUE(tb.isReady());
}

TEST(TeleportBlock, ChargeResetsWhenPlayerLeaves) {
    TestTeleportBlock tb(100, 200, 32, 32);
    tb.pairIndex = 1;

    sf::Vector2f playerPos(116, 184);
    tb.update(0.3f, playerPos, true);

    sf::Vector2f playerPosAway(500, 184);
    tb.update(0.1f, playerPosAway, true); // Player left

    ASSERT_EQ_F(tb.chargeTimer, 0.f, 0.01);
}

TEST(TeleportBlock, CooldownAfterTeleport) {
    TestTeleportManager mgr;
    mgr.add(100, 200, 32, 32);
    mgr.add(300, 200, 32, 32);
    mgr.pairAll();

    sf::Vector2f playerPos(116, 184);

    // Charge up first block
    for (int i = 0; i < 10; ++i) mgr.update(0.1f, playerPos, true);

    // Trigger teleport
    bool teleported = mgr.checkTeleport(playerPos);
    ASSERT_TRUE(teleported);
    ASSERT_EQ(mgr.m_blocks[0].cooldown, 1.2f);
    ASSERT_EQ(mgr.m_blocks[1].cooldown, 1.2f);
}

TEST(TeleportBlock, NoTeleportDuringCooldown) {
    TestTeleportManager mgr;
    mgr.add(100, 200, 32, 32);
    mgr.add(300, 200, 32, 32);
    mgr.pairAll();

    mgr.m_blocks[0].cooldown = 1.2f;
    mgr.m_blocks[1].cooldown = 1.2f;

    sf::Vector2f playerPos(116, 184);
    mgr.update(0.6f, playerPos, true); // Charge enough

    bool teleported = mgr.checkTeleport(playerPos);
    ASSERT_FALSE(teleported);
}

TEST(TeleportBlock, NoTeleportWithoutPair) {
    TestTeleportManager mgr;
    mgr.add(100, 200, 32, 32); // Unpaired

    sf::Vector2f playerPos(116, 184);
    for (int i = 0; i < 10; ++i) mgr.update(0.1f, playerPos, true);

    bool teleported = mgr.checkTeleport(playerPos);
    ASSERT_FALSE(teleported);
}

TEST(TeleportBlock, SpawnPosIsAboveBlock) {
    TestTeleportBlock tb(100, 200, 32, 32);
    // spawnPos should be center X, 40px above block top
    ASSERT_EQ_F(tb.spawnPos.x, 100 + 16, 0.01);
    ASSERT_EQ_F(tb.spawnPos.y, 200 - 40, 0.01);
}

TEST(TeleportBlock, CollisionResolution) {
    TestTeleportManager mgr;
    mgr.add(100, 200, 64, 32);
    mgr.pairAll();

    sf::Vector2f playerPos(132, 170);
    sf::Vector2f playerVel(0, 200);
    sf::FloatRect playerRect({121, 154}, {22, 32});
    bool onGround = false, dashAvail = false; int jumpsLeft = 0;

    mgr.resolvePlayer(playerPos, playerVel, playerRect, onGround, dashAvail, jumpsLeft);

    ASSERT_TRUE(onGround);
    ASSERT_TRUE(dashAvail);
    ASSERT_EQ(jumpsLeft, 1);
    ASSERT_EQ_F(playerPos.y, 200 - 16, 0.1);
}

TEST(TeleportBlock, ResetClearsState) {
    TestTeleportManager mgr;
    mgr.add(100, 200, 32, 32);
    mgr.add(300, 200, 32, 32);
    mgr.pairAll();

    sf::Vector2f playerPos(116, 184);
    for (int i = 0; i < 10; ++i) mgr.update(0.1f, playerPos, true);

    mgr.reset();
    ASSERT_EQ_F(mgr.m_blocks[0].chargeTimer, 0.f, 0.01);
    ASSERT_EQ_F(mgr.m_blocks[0].cooldown, 0.f, 0.01);
    ASSERT_FALSE(mgr.m_blocks[0].playerOn);
}