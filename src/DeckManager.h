#pragma once
#include <vector>
#include <memory>
#include <random>
#include "Card.h"

class DeckManager {
public:
    static DeckManager& getInstance() {
        static DeckManager instance;
        return instance;
    }

    void initializeDeck();
    std::shared_ptr<Card> drawCard();
    void addToDiscardPile(std::shared_ptr<Card> card);
    void shuffleDeck();
    void reshuffleDiscardPile();
    bool isDeckEmpty() const { return deck.empty(); }
    size_t getDeckSize() const { return deck.size(); }
    size_t getDiscardPileSize() const { return discardPile.size(); }

private:
    DeckManager() = default;
    ~DeckManager() = default;
    DeckManager(const DeckManager&) = delete;
    DeckManager& operator=(const DeckManager&) = delete;

    std::vector<std::shared_ptr<Card>> deck;
    std::vector<std::shared_ptr<Card>> discardPile;
}; 