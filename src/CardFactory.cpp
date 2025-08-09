#include "CardFactory.h"
#include <random>
#include <algorithm>

void CardFactory::initialize() {
    // Define all cards in the game
    std::vector<CardDefinition> definitions = {
        // Basic Limb Cards (Common)
        {
            "Basic Head",
            "A standard skull",
            Card::Type::LIMB,
            1,
            LimbCard::LimbType::HEAD,
            0, 1
        },
        {
            "Basic Spine",
            "A regular spine",
            Card::Type::LIMB,
            1,
            LimbCard::LimbType::SPINE,
            0, 1
        },
        {
            "Basic Arm",
            "A regular arm",
            Card::Type::LIMB,
            1,
            LimbCard::LimbType::ARM,
            1, 0
        },
        {
            "Basic Leg",
            "A regular leg",
            Card::Type::LIMB,
            1,
            LimbCard::LimbType::LEG,
            0, 1
        },

        // Special Limb Cards (Rare)
        {
            "Flaming Arm",
            "An arm that burns enemies",
            Card::Type::LIMB,
            3,
            LimbCard::LimbType::ARM,
            2, 0
        },
        {
            "Bone Wings",
            "Wings that let you dodge attacks",
            Card::Type::LIMB,
            3,
            LimbCard::LimbType::SPINE,
            0, 2
        },

        // Mutation Cards
        {
            "Acid Blood",
            "Your blood burns enemies",
            Card::Type::MUTATION,
            2,
            LimbCard::LimbType::HEAD,  // Not used for mutations
            0, 0,
            3  // duration
        },
        {
            "Bone Growth",
            "Your bones grow stronger",
            Card::Type::MUTATION,
            2,
            LimbCard::LimbType::HEAD,  // Not used for mutations
            0, 0,
            -1  // permanent duration
        },

        // Gear Cards
        {
            "Nice Hat",
            "A fancy hat that makes you look cool",
            Card::Type::GEAR,
            1,
            LimbCard::LimbType::HEAD,  // Not used for gear
            0, 0,
            0,  // duration not used for gear
            0, 1  // gear stats
        },
        {
            "Rusty Sword",
            "An old but effective weapon",
            Card::Type::GEAR,
            1,
            LimbCard::LimbType::HEAD,  // Not used for gear
            0, 0,
            0,  // duration not used for gear
            2, 0  // gear stats
        },

        // Action Cards
        {
            "Limb Larceny",
            "Steal a limb from another player",
            Card::Type::ACTION,
            3,
            LimbCard::LimbType::HEAD,  // Not used for actions
            0, 0,
            0,  // duration not used for actions
            0, 0  // gear stats not used for actions
        },
        {
            "Exploding Ribcage",
            "Blow up limbs from adjacent players",
            Card::Type::ACTION,
            4,
            LimbCard::LimbType::HEAD,  // Not used for actions
            0, 0,
            0,  // duration not used for actions
            0, 0  // gear stats not used for actions
        }
    };

    // Add all definitions to the maps
    for (const auto& def : definitions) {
        cardDefinitions[def.name] = def;
        cardsByType[def.type].push_back(def);
    }
}

std::shared_ptr<Card> CardFactory::createCard(const CardDefinition& def) {
    switch (def.type) {
        case Card::Type::LIMB:
            return std::make_shared<LimbCard>(def.name, def.description, 
                                            def.limbType, def.attackBonus, def.defenseBonus);
        case Card::Type::MUTATION:
            return std::make_shared<MutationCard>(def.name, def.description, def.duration);
        case Card::Type::GEAR:
            return std::make_shared<GearCard>(def.name, def.description, 
                                            def.gearAttackBonus, def.gearDefenseBonus);
        case Card::Type::ACTION:
            return std::make_shared<ActionCard>(def.name, def.description);
        default:
            return nullptr;
    }
}

std::shared_ptr<Card> CardFactory::getRandomCard(Card::Type type, int minRarity, int maxRarity) {
    auto& typeCards = cardsByType[type];
    if (typeCards.empty()) return nullptr;

    // Filter cards by rarity
    std::vector<CardDefinition> validCards;
    for (const auto& def : typeCards) {
        if (def.rarity >= minRarity && def.rarity <= maxRarity) {
            validCards.push_back(def);
        }
    }

    if (validCards.empty()) return nullptr;

    // Select a random card
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, validCards.size() - 1);
    
    return createCard(validCards[dis(gen)]);
}

std::vector<std::shared_ptr<Card>> CardFactory::getCards(Card::Type type, int minRarity, int maxRarity) {
    std::vector<std::shared_ptr<Card>> result;
    auto& typeCards = cardsByType[type];

    for (const auto& def : typeCards) {
        if (def.rarity >= minRarity && def.rarity <= maxRarity) {
            result.push_back(createCard(def));
        }
    }

    return result;
} 