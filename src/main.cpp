#include "Game.h"
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";  // ANSI escape sequence to clear screen
}

void printHand(const std::vector<std::shared_ptr<Card>>& hand) {
    std::cout << "\nYour hand:\n";
    for (size_t i = 0; i < hand.size(); ++i) {
        std::cout << i + 1 << ". " << hand[i]->getName() 
                  << " (" << hand[i]->getDescription() << ")\n";
    }
}

void printPlayerStatus(const std::shared_ptr<Player>& player) {
    std::cout << "\n" << player->getName() << "'s Status:\n";
    std::cout << "Health: " << player->getHealth() << "\n";
    std::cout << "Attack: " << player->getAttack() << "\n";
    std::cout << "Defense: " << player->getDefense() << "\n";
}

int main() {
    Game game;
    
    // Add players
    std::string playerName;
    std::cout << "Enter your name: ";
    std::getline(std::cin, playerName);
    game.addPlayer(playerName, false);
    game.addPlayer("Bot", true);
    
    // Game loop
    while (!game.isGameOver()) {
        clearScreen();
        auto currentPlayer = game.getCurrentPlayer();
        
        std::cout << "\n=== " << currentPlayer->getName() << "'s Turn ===\n";
        
        // Draw a card
        if (game.drawCard()) {
            std::cout << "Drew a card!\n";
        } else {
            std::cout << "No cards left in deck!\n";
        }
        
        // Show player status
        printPlayerStatus(currentPlayer);
        
        // Show hand
        printHand(currentPlayer->getHand());
        
        if (currentPlayer->getIsBot()) {
            // Bot's turn
            std::cout << "\nBot is thinking...\n";
            // Simple bot strategy: play first card in hand
            if (!currentPlayer->getHand().empty()) {
                game.playCard(0);
                std::cout << "Bot played a card!\n";
            }
        } else {
            // Human player's turn
            std::cout << "\nWhat would you like to do?\n";
            std::cout << "1. Play a card\n";
            std::cout << "2. End turn\n";
            std::cout << "Choice: ";
            
            int choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            if (choice == 1) {
                std::cout << "Enter card number to play (1-" << currentPlayer->getHand().size() << "): ";
                int cardIndex;
                std::cin >> cardIndex;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                if (cardIndex > 0 && cardIndex <= static_cast<int>(currentPlayer->getHand().size())) {
                    game.playCard(cardIndex - 1);
                } else {
                    std::cout << "Invalid card number!\n";
                }
            }
        }
        
        // End turn
        game.endTurn();
        
        // Small delay to make the game more readable
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    // Game over
    clearScreen();
    std::cout << "\n=== Game Over ===\n";
    for (const auto& player : game.getPlayers()) {
        std::cout << player->getName() << " - Health: " << player->getHealth() << "\n";
    }
    
    return 0;
} 