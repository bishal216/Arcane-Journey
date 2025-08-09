#include "Player.h"
#include "Game.h"
#include <iostream>
#include <algorithm>

Player::Player(const std::string& name, bool isBot)
    : name(name), isBot(isBot), health(100), attack(0), defense(0) {
}

bool Player::hasFullSkeleton() const {
    return limbs.count(LimbCard::LimbType::HEAD) > 0 &&
           limbs.count(LimbCard::LimbType::SPINE) > 0 &&
           limbs.count(LimbCard::LimbType::ARM) == 2 &&
           limbs.count(LimbCard::LimbType::LEG) == 2;
}

void Player::addCardToHand(std::shared_ptr<Card> card) {
    hand.push_back(card);
}

void Player::playCard(int index) {
    if (index >= 0 && index < static_cast<int>(hand.size())) {
        hand.erase(hand.begin() + index);
    }
}

void Player::takeDamage(int amount) {
    health = std::max(0, health - amount);
}

void Player::addLimb(std::shared_ptr<LimbCard> limb) {
    limbs[limb->getLimbType()] = limb;
    updateStats();
}

void Player::removeLimb(LimbCard::LimbType type) {
    limbs.erase(type);
    updateStats();
}

bool Player::hasLimb(LimbCard::LimbType type) const {
    return limbs.count(type) > 0;
}

void Player::addMutation(std::shared_ptr<MutationCard> mutation) {
    mutations.push_back(mutation);
    updateStats();
}

void Player::removeMutation(size_t index) {
    if (index >= mutations.size()) {
        std::cout << "Invalid mutation index!\n";
        return;
    }
    mutations.erase(mutations.begin() + index);
    updateStats();
}

void Player::equipGear(std::shared_ptr<GearCard> gear) {
    gearSlots.push_back(gear);
    updateStats();
}

void Player::startTurn() {
    // Decrease durability of limbs
    for (auto& [type, limb] : limbs) {
        if (limb->getDurability() > 0) {
            limb->decreaseDurability();
        }
    }
    updateStats();
}

void Player::endTurn(Game& game) {
    // Apply end-of-turn effects
    for (auto& mutation : mutations) {
        mutation->endTurnEffect(game, *this);
    }
    for (auto& gear : gearSlots) {
        gear->endTurnEffect(game, *this);
    }
}

void Player::updateStats() {
    attack = 0;
    defense = 0;
    
    // Calculate stats from limbs
    for (const auto& [type, limb] : limbs) {
        attack += limb->getAttackBonus();
        defense += limb->getDefenseBonus();
    }
    
    // Calculate stats from mutations
    for (const auto& mutation : mutations) {
        attack += mutation->getAttackBonus();
        defense += mutation->getDefenseBonus();
    }
    
    // Calculate stats from gear
    for (const auto& gear : gearSlots) {
        attack += gear->getAttackBonus();
        defense += gear->getDefenseBonus();
    }
}

bool Player::hasLimb(LimbCard::LimbType type) const {
    return limbs.count(type) > 0;
}