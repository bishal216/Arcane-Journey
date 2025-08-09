#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Player.h"
#include "Card.h"

class Game {
public:
    Game();
    void run();

    // Getters
    const std::vector<std::shared_ptr<Player>>& getPlayers() const { return players; }

    // Add player overload for bot
    void addPlayer(const std::string& name, bool isBot = false);

    // Bot turn
    void handleBotTurn(Player& bot);

    bool drawCard();
    bool playCard(int index);
    void endTurn();
    bool isGameOver() const;
    std::shared_ptr<Player> getCurrentPlayer() const;
    void displayStats() const;

private:
    std::vector<std::shared_ptr<Player>> players;
    size_t currentPlayerIndex;
    bool gameOver;

    // Game setup
    void initializeDeck();
    void shuffleDeck();

    // Turn management
    void startTurn();
    void nextPlayer();

    // Game state
    bool checkGameOver() const;
    void displayGameState() const;
    void displayPlayerState(const Player& player) const;
    void handlePlayerTurn(Player& player);

    void attackCurrentPlayer();
}; 