#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct TeleportBlock {
    sf::FloatRect bounds;
    sf::Vector2f spawnPos;  // where player lands after teleporting here
    int pairIndex;          // index of the paired TeleportBlock
    float chargeTimer = 0.f;
    float cooldown = 0.f;
    bool playerOn = false;

    // Visuals
    sf::RectangleShape body;
    sf::RectangleShape fill;  // charge progress bar
};

class TeleportBlockManager {
   public:
    // Add a block at tile position — pairing is done after all blocks added
    void add(float x, float y, float w, float h);
    void pairAll();  // call once after all blocks added

    void update(float dt, sf::Vector2f& playerPos, bool playerOnGround);
    void draw(sf::RenderWindow& window) const;
    void reset();  // clear all charge/cooldown state

    // Solid collision — returns true and resolves if player lands on top
    void resolvePlayer(sf::Vector2f& pos, sf::Vector2f& vel, sf::FloatRect playerBounds,
                       bool& onGround);

   private:
    std::vector<TeleportBlock> m_blocks;

    static constexpr float CHARGE_TIME = 0.5f;
    static constexpr float COOLDOWN = 1.2f;  // after teleport, don't re-trigger
};