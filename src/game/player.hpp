#pragma once
#include "unit.hpp"
#include <vector>

class Player {
private:
    std::vector<Unit> team;  // Player's current team of units
    int maxTeamSize;         // Max size of the team

public:
    Player(int maxTeamSize = 5);

    // Add unit to the team
    bool addUnit(const Unit& unit);

    // Display the player's team
    void displayTeam() const;

    // Get the team's size
    int getTeamSize() const { return team.size(); }

    // Get unit by index (for battle or display purposes)
    Unit& getUnit(int index);

    // Get all units in the team
    const std::vector<Unit>& getAllUnits() const;
};
