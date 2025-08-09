#include "game/unitpool.hpp"
#include "game/shop.hpp"
#include "game/player.hpp"
#include "game/battle.hpp"

int main() {
    UnitPool pool;
    Shop shop(pool, 10);

    Player player;
    Player opponent;

    // Add units to both players' teams
    shop.buyUnit(1, player);  // Buy a unit for the player
    shop.buyUnit(2, player);  // Buy another unit for the player

    shop.reroll();
    shop.buyUnit(1, opponent);  // Buy a unit for the opponent
    shop.buyUnit(2, opponent);  // Buy another unit for the opponent

    player.displayTeam();
    opponent.displayTeam();

    // Start a battle between the player and opponent
    Battle battle(player, opponent);
    battle.startBattle();

    return 0;
}
