#include "DeckManager.h"
#include "CardFactory.h"
#include <algorithm>

void DeckManager::initializeDeck() {
    deck.clear();
    discardPile.clear();
    
    // Add multiple copies of each card type
    for (int i = 0; i < 3; i++) {  // 3 copies of each card
        // Add limb cards (rarity 1-2)
        auto limbCards = CardFactory::getInstance().getCards(Card::Type::LIMB, 1, 2);
        deck.insert(deck.end(), limbCards.begin(), limbCards.end());
        
        // Add mutation cards (rarity 2)
        auto mutationCards = CardFactory::getInstance().getCards(Card::Type::MUTATION, 2, 2);
        deck.insert(deck.end(), mutationCards.begin(), mutationCards.end());
        
        // Add gear cards (rarity 1)
        auto gearCards = CardFactory::getInstance().getCards(Card::Type::GEAR, 1, 1);
        deck.insert(deck.end(), gearCards.begin(), gearCards.end());
        
        // Add action cards (rarity 3-4)
        auto actionCards = CardFactory::getInstance().getCards(Card::Type::ACTION, 3, 4);
        deck.insert(deck.end(), actionCards.begin(), actionCards.end());
    }
    
    shuffleDeck();
}

std::shared_ptr<Card> DeckManager::drawCard() {
    if (deck.empty()) {
        reshuffleDiscardPile();
        if (deck.empty()) {
            return nullptr;
        }
    }
    
    auto card = deck.back();
    deck.pop_back();
    return card;
}

void DeckManager::addToDiscardPile(std::shared_ptr<Card> card) {
    if (card) {
        discardPile.push_back(card);
    }
}

void DeckManager::shuffleDeck() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}

void DeckManager::reshuffleDiscardPile() {
    // Move all cards from discard pile to deck
    deck.insert(deck.end(), discardPile.begin(), discardPile.end());
    discardPile.clear();
    shuffleDeck();
} 