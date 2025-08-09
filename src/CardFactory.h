#pragma once
#include <memory>
#include <string>
#include <map>
#include "Card.h"

// Card definition structure
struct CardDefinition {
    std::string name;
    std::string description;
    Card::Type type;
    int rarity;  // 1-5, where 5 is rarest
    
    // Limb specific
    LimbCard::LimbType limbType;
    int attackBonus;
    int defenseBonus;
    
    // Mutation specific
    int duration;
    
    // Gear specific
    int gearAttackBonus;
    int gearDefenseBonus;
};

class CardFactory {
public:
    static CardFactory& getInstance() {
        static CardFactory instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    CardFactory(const CardFactory&) = delete;
    CardFactory& operator=(const CardFactory&) = delete;

    // Initialize the factory with card definitions
    void initialize();

    // Create a card from its definition
    std::shared_ptr<Card> createCard(const CardDefinition& def);

    // Get a random card of a specific type and rarity
    std::shared_ptr<Card> getRandomCard(Card::Type type, int minRarity = 1, int maxRarity = 5);

    // Get all cards of a specific type and rarity range
    std::vector<std::shared_ptr<Card>> getCards(Card::Type type, int minRarity = 1, int maxRarity = 5);

private:
    CardFactory() = default;
    ~CardFactory() = default;

    std::map<std::string, CardDefinition> cardDefinitions;
    std::map<Card::Type, std::vector<CardDefinition>> cardsByType;
}; 