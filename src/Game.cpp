#include "Game.h"
#include "CardFactory.h"
#include "DeckManager.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <iomanip>

Game::Game() : currentPlayerIndex(0), gameOver(false) {
    CardFactory::getInstance().initialize();
    DeckManager::getInstance().initializeDeck();
}

void Game::addPlayer(const std::string& name, bool isBot) {
    players.push_back(std::make_shared<Player>(name, isBot));
}

bool Game::drawCard() {
    auto card = DeckManager::getInstance().drawCard();
    if (!card) {
        return false;
    }
    
    auto currentPlayer = getCurrentPlayer();
    currentPlayer->addCardToHand(card);
    return true;
}

bool Game::playCard(int index) {
    auto currentPlayer = getCurrentPlayer();
    if (index < 0 || index >= static_cast<int>(currentPlayer->getHand().size())) {
        return false;
    }
    
    auto card = currentPlayer->getHand()[index];
    currentPlayer->playCard(index);
    
    // Apply card effects
    if (card->getType() == Card::Type::LIMB) {
        auto limbCard = std::dynamic_pointer_cast<LimbCard>(card);
        if (limbCard) {
            currentPlayer->addLimb(limbCard);
        }
    } else if (card->getType() == Card::Type::MUTATION) {
        auto mutationCard = std::dynamic_pointer_cast<MutationCard>(card);
        if (mutationCard) {
            currentPlayer->addMutation(mutationCard);
        }
    } else if (card->getType() == Card::Type::GEAR) {
        auto gearCard = std::dynamic_pointer_cast<GearCard>(card);
        if (gearCard) {
            currentPlayer->equipGear(gearCard);
        }
    } else if (card->getType() == Card::Type::ACTION) {
        // For now, action cards just deal damage
        attackCurrentPlayer();
    }
    
    // Add the played card to the discard pile
    DeckManager::getInstance().addToDiscardPile(card);
    return true;
}

void Game::endTurn() {
    // Check if game is over
    for (const auto& player : players) {
        if (player->getHealth() <= 0) {
            gameOver = true;
            return;
        }
    }
    
    // Move to next player
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
}

bool Game::isGameOver() const {
    return gameOver;
}

std::shared_ptr<Player> Game::getCurrentPlayer() const {
    return players[currentPlayerIndex];
}

void Game::attackCurrentPlayer() {
    auto currentPlayer = getCurrentPlayer();
    int damage = currentPlayer->getAttack();
    
    // Find the other player
    for (const auto& player : players) {
        if (player != currentPlayer) {
            int actualDamage = std::max(0, damage - player->getDefense());
            player->takeDamage(actualDamage);
            break;
        }
    }
}

void Game::displayStats() const {
    std::cout << "\n=== Game Status ===\n";
    for (const auto& player : players) {
        std::cout << "\n" << player->getName() << (player->getIsBot() ? " (Bot)" : "") << ":\n";
        std::cout << "Health: " << player->getHealth() << "\n";
        std::cout << "Attack: " << player->getAttack() << "\n";
        std::cout << "Defense: " << player->getDefense() << "\n";
        
        // Display limbs
        std::cout << "\nLimbs:\n";
        for (const auto& [type, limb] : player->getLimbs()) {
            std::cout << "- " << limb->getName() << " (Durability: " << limb->getDurability() << ")\n";
        }
        
        // Display mutations
        std::cout << "\nMutations:\n";
        for (const auto& mutation : player->getMutations()) {
            std::cout << "- " << mutation->getName() << "\n";
        }
        
        // Display gear
        std::cout << "\nGear:\n";
        for (const auto& gear : player->getEquippedGear()) {
            std::cout << "- " << gear->getName() << "\n";
        }
        
        std::cout << "\nHand size: " << player->getHand().size() << "\n";
        std::cout << "------------------------\n";
    }
    std::cout << "\nCurrent turn: " << getCurrentPlayer()->getName() << "\n";
    std::cout << "==================\n\n";
} 