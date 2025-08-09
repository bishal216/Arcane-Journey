#include "player.hpp"
#include <iostream>

Player::Player(int maxTeamSize) : maxTeamSize(maxTeamSize) {}

bool Player::addUnit(const Unit& unit) {
    if (team.size() < maxTeamSize) {
        team.push_back(unit);
        std::cout << "Added " << unit.getName() << " to your team.\n";
        return true;
    }
    std::cout << "Your team is full! Cannot add more units.\n";
    return false;
}

void Player::displayTeam() const {
    std::cout << "Your Team: \n";
    for (int i = 0; i < team.size(); ++i) {
        const auto& unit = team[i];
        std::cout << i + 1 << ". " << unit.getName() << " - "
                  << "Attack: " << unit.getAttack() << ", Health: " << unit.getHealth()
                  << " - " << unit.getAbilityDescription() << "\n";
    }
}

Unit& Player::getUnit(int index){
    return team[index];
}

const std::vector<Unit>& Player::getAllUnits() const {
    return team;
}