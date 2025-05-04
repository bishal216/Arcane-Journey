#include "battle.hpp"
#include <iostream>

Battle::Battle(Player& player, Player& opponent) : player(player), opponent(opponent) {}

void Battle::startBattle() {
    std::cout << "Battle Start!\n";
    while (true) {
        fightRound();
        if (isTeamDefeated(player)) {
            std::cout << "You lost the battle!\n";
            break;
        }
        if (isTeamDefeated(opponent)) {
            std::cout << "You won the battle!\n";
            break;
        }
    }
}


void Battle::fightRound() {
    std::cout << "\nRound Start!\n";

    // Perform attacks and check abilities for the player's team
    for (int i = 0; i < player.getTeamSize(); ++i) {
        const Unit& unit = player.getUnit(i);
        if (unit.getHealth() > 0) {
            std::cout << unit.getName() << " attacks!\n";
            // Here you'd trigger unit abilities and apply damage
            // For simplicity, just decrement opponent's health directly
            // Assuming opponent's units are arranged in a similar way:
            if (opponent.getTeamSize() > i) {
                Unit& opponentUnit = opponent.getUnit(i);
                opponentUnit.takeDamage(unit.getAttack());
            }
        }
    }

    // Perform attacks and check abilities for the opponent's team
    for (int i = 0; i < opponent.getTeamSize(); ++i) {
        const Unit& unit = opponent.getUnit(i);
        if (unit.getHealth() > 0) {
            std::cout << unit.getName() << " attacks!\n";
            // Similar to the player's team, apply damage to the player's units
            if (player.getTeamSize() > i) {
                Unit& playerUnit = player.getUnit(i);
                playerUnit.takeDamage(unit.getAttack());
            }
        }
    }
}

bool Battle::isTeamDefeated(const Player& team) const {
    for (const auto& unit : team.getAllUnits()) {
        if (unit.getHealth() > 0) {
            return false;  // Team is not defeated
        }
    }
    return true;  // All units are defeated
}
