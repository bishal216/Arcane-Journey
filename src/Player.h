#pragma once

#include "Card.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class Player {
public:
    Player(const std::string& name, bool isBot);
    
    void addCardToHand(std::shared_ptr<Card> card);
    void playCard(int index);
    void addLimb(std::shared_ptr<LimbCard> limb);
    void removeLimb(LimbCard::LimbType type);
    void addMutation(std::shared_ptr<MutationCard> mutation);
    void removeMutation(size_t index);
    void equipGear(std::shared_ptr<GearCard> gear);
    void takeDamage(int amount);
    void startTurn();
    void updateStats();
    
    const std::string& getName() const { return name; }
    bool getIsBot() const { return isBot; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    bool hasLimb(LimbCard::LimbType type);
    const std::vector<std::shared_ptr<Card>>& getHand() const { return hand; }
    const std::map<LimbCard::LimbType, std::shared_ptr<LimbCard>>& getLimbs() const { return limbs; }
    const std::vector<std::shared_ptr<MutationCard>>& getMutations() const { return mutations; }
    const std::vector<std::shared_ptr<GearCard>>& getEquippedGear() const { return gearSlots; }

private:
    std::string name;
    bool isBot;
    int health;
    int attack;
    int defense;
    std::vector<std::shared_ptr<Card>> hand;
    std::map<LimbCard::LimbType, std::shared_ptr<LimbCard>> limbs;
    std::vector<std::shared_ptr<MutationCard>> mutations;
    std::vector<std::shared_ptr<GearCard>> gearSlots;
};