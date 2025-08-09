#include "Card.h"
#include "Player.h"
#include "Game.h"
#include <iostream>

// Base Card implementation
Card::Card(const std::string& name, const std::string& description, Type type)
    : name(name), description(description), type(type) {}

// Limb Card implementation
LimbCard::LimbCard(const std::string& name, const std::string& description,
                   LimbType limbType, int attackBonus, int defenseBonus)
    : Card(name, description, Type::LIMB), limbType(limbType),
      attackBonus(attackBonus), defenseBonus(defenseBonus), durability(-1) {}

bool LimbCard::canPlay(const Player& player) const {
    // Check if player can add this limb
    if (limbType == LimbType::ARM || limbType == LimbType::LEG) {
        int count = 0;
        for (const auto& limb : player.getLimbs()) {
            if (limb.first == limbType) count++;
        }
        return count < 2;
    }
    return !player.hasLimb(limbType);
}

void LimbCard::play(Game& game, Player& player) {
    player.addLimb(std::make_shared<LimbCard>(*this));
}

void LimbCard::endTurnEffect(Game& game, Player& player) {
    // Special limb effects can be implemented here
}

// Mutation Card implementation
MutationCard::MutationCard(const std::string& name, const std::string& description,
                          int duration)
    : Card(name, description, Type::MUTATION), duration(duration) {}

bool MutationCard::canPlay(const Player& player) const {
    return true; // Mutations can always be played
}

void MutationCard::play(Game& game, Player& player) {
    player.addMutation(std::make_shared<MutationCard>(*this));
}

void MutationCard::endTurnEffect(Game& game, Player& player) {
    if (duration > 0) {
        duration--;
        if (duration == 0) {
            // Remove mutation when duration expires
            for (size_t i = 0; i < player.getMutations().size(); ++i) {
                if (player.getMutations()[i]->getName() == name) {
                    player.removeMutation(i);
                    break;
                }
            }
        }
    }
}

// Gear Card implementation
GearCard::GearCard(const std::string& name, const std::string& description,
                   int attackBonus, int defenseBonus)
    : Card(name, description, Type::GEAR), attackBonus(attackBonus),
      defenseBonus(defenseBonus) {}

bool GearCard::canPlay(const Player& player) const {
    return true; // Gear can always be equipped
}

void GearCard::play(Game& game, Player& player) {
    player.equipGear(std::make_shared<GearCard>(*this));
}

void GearCard::endTurnEffect(Game& game, Player& player) {
    // Gear effects can be implemented here
}

// Action Card implementation
ActionCard::ActionCard(const std::string& name, const std::string& description)
    : Card(name, description, Type::ACTION) {}

bool ActionCard::canPlay(const Player& player) const {
    return true; // Action cards can always be played
}

void ActionCard::play(Game& game, Player& player) {
    // Implement specific action card effects
    if (name == "Limb Larceny") {
        // Steal a limb from another player
        std::cout << "Choose a player to steal from:\n";
        for (size_t i = 0; i < game.getPlayers().size(); ++i) {
            if (game.getPlayers()[i].get() != &player) {
                std::cout << "[" << i << "] " << game.getPlayers()[i]->getName() << "\n";
            }
        }
        
        int choice;
        std::cin >> choice;
        if (choice >= 0 && choice < game.getPlayers().size() && 
            game.getPlayers()[choice].get() != &player) {
            auto& target = game.getPlayers()[choice];
            if (!target->getLimbs().empty()) {
                // Steal the first limb
                auto limb = target->getLimbs().begin()->second;
                target->removeLimb(limb->getLimbType());
                player.addLimb(limb);
            }
        }
    } else if (name == "Exploding Ribcage") {
        // Damage adjacent players
        for (size_t i = 0; i < game.getPlayers().size(); ++i) {
            if (game.getPlayers()[i].get() != &player) {
                // Remove a random limb
                if (!game.getPlayers()[i]->getLimbs().empty()) {
                    auto& limbs = game.getPlayers()[i]->getLimbs();
                    auto it = limbs.begin();
                    std::advance(it, rand() % limbs.size());
                    game.getPlayers()[i]->removeLimb(it->first);
                }
            }
        }
    }
}

void ActionCard::endTurnEffect(Game& game, Player& player) {
    // Action cards typically don't have end-of-turn effects
} 