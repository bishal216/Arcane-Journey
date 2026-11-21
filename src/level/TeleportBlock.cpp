#include "level/TeleportBlock.hpp"

#include <algorithm>
#include <cmath>

#include "Constants.hpp"
#include "systems/DiscoveryTracker.hpp"

void TeleportBlockManager::add(float x, float y, float w, float h) {
    TeleportBlock tb;
    tb.bounds = sf::FloatRect({x, y}, {w, h});
    tb.spawnPos = {x + w * 0.5f, y - TILEF};  // land one tile above the block
    tb.pairIndex = -1;

    tb.body.setSize({w, h});
    tb.body.setPosition({x, y});
    tb.body.setFillColor(PLAT_TELEPORT);
    tb.body.setOutlineColor(ARCANE);
    tb.body.setOutlineThickness(2.f);

    tb.fill.setSize({0.f, 4.f});
    tb.fill.setPosition({x, y + h - 4.f});
    tb.fill.setFillColor(sf::Color(180, 120, 255)); // lighter arcane for charge bar

    m_blocks.push_back(tb);
}

int TeleportBlockManager::pairAll() {
    // Sequential pairing: 0↔1, 2↔3, 4↔5 ...
    for (int i = 0; i + 1 < (int)m_blocks.size(); i += 2) {
        m_blocks[i].pairIndex = i + 1;
        m_blocks[i + 1].pairIndex = i;
    }
    // Odd one out — no partner: it only acts as a plain platform. Draw it in red so the
    // mistake is visible in-game, and let the caller report it.
    int unpaired = 0;
    for (auto& b : m_blocks) {
        if (b.pairIndex < 0) {
            b.body.setOutlineColor(ROSE);
            ++unpaired;
        }
    }
    return unpaired;
}

void TeleportBlockManager::update(float dt, sf::Vector2f& playerPos, bool playerOnGround) {
    for (auto& tb : m_blocks) {
        if (tb.cooldown > 0.f) {
            tb.cooldown -= dt;
            tb.chargeTimer = 0.f;
            tb.playerOn = false;
            // Update fill bar to empty during cooldown
            tb.fill.setSize({0.f, 4.f});
            continue;
        }

        if (tb.playerOn && playerOnGround && tb.pairIndex >= 0) {
            tb.chargeTimer += dt;

            // Update charge bar width
            float progress = std::min(tb.chargeTimer / CHARGE_TIME, 1.f);
            tb.fill.setSize({tb.bounds.size.x * progress, 4.f});
            tb.fill.setPosition({tb.bounds.position.x, tb.bounds.position.y + TILE - 4.f});

            if (tb.chargeTimer >= CHARGE_TIME) {
                // Teleport!
                g_discovery.discover(PlatType::Teleport);
                auto& dest = m_blocks[tb.pairIndex];
                playerPos = dest.spawnPos;

                // Apply cooldown to DESTINATION so player doesn't bounce back
                dest.cooldown = COOLDOWN;
                dest.chargeTimer = 0.f;
                dest.fill.setSize({0.f, 4.f});

                // Reset this block too
                tb.chargeTimer = 0.f;
                tb.cooldown = COOLDOWN;
                tb.fill.setSize({0.f, 4.f});
            }
        } else {
            // Decay charge if player steps off
            tb.chargeTimer = std::max(0.f, tb.chargeTimer - dt * 2.f);
            float progress = std::min(tb.chargeTimer / CHARGE_TIME, 1.f);
            tb.fill.setSize({tb.bounds.size.x * progress, 4.f});
        }

        tb.playerOn = false;  // reset each frame — resolvePlayer sets it back
    }
}

void TeleportBlockManager::resolvePlayer(sf::Vector2f& pos, sf::Vector2f& vel, sf::FloatRect pb,
                                         bool& onGround, bool& dashAvail, int& jumpsLeft) {
    for (auto& tb : m_blocks) {
        if (!pb.findIntersection(tb.bounds)) continue;

        float pbBottom = pb.position.y + pb.size.y;
        float tbTop = tb.bounds.position.y;

        if (vel.y >= 0.f && pbBottom <= tbTop + vel.y + 2.f) {
            // Landing on top
            pos.y = tbTop - pb.size.y / 2.f;
            vel.y = 0.f;
            onGround = true;
            dashAvail = true;
            jumpsLeft = 1;
            tb.playerOn = true;
        }
    }
}

void TeleportBlockManager::draw(sf::RenderWindow& window) const {
    for (const auto& tb : m_blocks) {
        window.draw(tb.body);
        if (tb.fill.getSize().x > 0.f) window.draw(tb.fill);

        // Draw a small arrow pointing toward the pair
        // (simple visual cue — just tint differently if paired)
    }
}

void TeleportBlockManager::reset() {
    for (auto& b : m_blocks) {
        b.chargeTimer = 0.f;
        b.cooldown = 0.f;
        b.playerOn = false;
    }
}