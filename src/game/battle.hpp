#pragma once
#include "player.hpp"
#include "unit.hpp"

class Battle {
private:
    Player& player;
    Player& opponent;

public:
    Battle(Player& player, Player& opponent);

    // Starts the battle
    void startBattle();

    // Simulate a single combat round
    void fightRound();

    // Check if a team has been defeated
    bool isTeamDefeated(const Player& team) const;
};
