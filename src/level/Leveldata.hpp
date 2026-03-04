#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "core/World.hpp"
#include "core/Player.hpp"
#include "level/MovingPlatform.hpp"
#include "level/CrumblingPlatform.hpp"
#include "level/BouncePad.hpp"
#include "level/DisappearingPlatform.hpp"
#include "level/Climbable.hpp"
#include "level/Trapdoor.hpp"
#include "level/TeleportBlock.hpp"

class LevelData {
public:
    LevelData() = default;

    void addStatic        (float x, float y, float w, float h, sf::Color col);
    void addGoal          (float x, float y, float w, float h);
    void addMoving        (float x, float y, float w, float h,
                           sf::Vector2f a, sf::Vector2f b,
                           float speed, sf::Color col);
    void addCrumbling     (float x, float y, float w, float h, sf::Color col);
    void addBounce        (float x, float y, float w, float h,
                           float force, sf::Color col);
    void addDisappearing  (float x, float y, float w, float h, sf::Color col,
                           float on, float off, float fade, float phase);
    void addRope          (float cx, float topY, float h, sf::Color col);
    void addLadder        (float cx, float topY, float h, sf::Color col);
    void addTrapdoorOneWay(float x, float y, float w, float h, sf::Color col);
    void addTrapdoorHatch (float x, float y, float w, float h, sf::Color col);
    void addTeleport      (float x, float y, float w, float h);
    void addCoin          (float cx, float cy);
    void addSection       (float worldY, const std::string& name);
    void sortSections     ();
    void finalize         ();   // call after all tiles added — pairs teleports etc.

    void update       (float dt);
    void draw         (sf::RenderWindow& window) const;
    void resolvePlayer(Player& player, float dt, GameState& state);
    void resetDynamic ();

    const std::vector<Platform>&  platforms()  const;
    const std::vector<Section>&   sections()   const;
    const std::vector<CoinSpawn>& coinSpawns() const;
    int   getSectionIndex(float worldY)        const;

private:
    World                       m_world;
    MovingPlatformManager       m_moving;
    CrumblingPlatformManager    m_crumbling;
    BouncePadManager            m_bounce;
    DisappearingPlatformManager m_disappearing;
    ClimbableManager            m_climbables;
    TrapdoorManager             m_trapdoors;
    TeleportBlockManager        m_teleports;
};