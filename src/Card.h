#pragma once
#include <string>
#include <memory>
#include <vector>

// Forward declarations
class Player;
class Game;

// Base Card class
class Card {
public:
    enum class Type {
        LIMB,
        MUTATION,
        GEAR,
        ACTION
    };

    Card(const std::string& name, const std::string& description, Type type);
    virtual ~Card() = default;

    // Getters
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    Type getType() const { return type; }

    // Core functionality
    virtual bool canPlay(const Player& player) const = 0;
    virtual void play(Game& game, Player& player) = 0;
    virtual void endTurnEffect(Game& game, Player& player) = 0;

protected:
    std::string name;
    std::string description;
    Type type;
};

// Limb Card
class LimbCard : public Card {
public:
    enum class LimbType {
        HEAD,
        SPINE,
        ARM,
        LEG
    };

    LimbCard(const std::string& name, const std::string& description, 
             LimbType limbType, int attackBonus = 0, int defenseBonus = 0);
    
    bool canPlay(const Player& player) const override;
    void play(Game& game, Player& player) override;
    void endTurnEffect(Game& game, Player& player) override;

    // Getters
    LimbType getLimbType() const { return limbType; }
    int getAttackBonus() const { return attackBonus; }
    int getDefenseBonus() const { return defenseBonus; }
    int getDurability() const { return durability; }
    void decreaseDurability() { if (durability > 0) durability--; }

private:
    LimbType limbType;
    int attackBonus;
    int defenseBonus;
    int durability; // For special limbs that wear out
};

// Mutation Card
class MutationCard : public Card {
public:
    MutationCard(const std::string& name, const std::string& description,
                 int duration = -1); // -1 means permanent
    
    bool canPlay(const Player& player) const override;
    void play(Game& game, Player& player) override;
    void endTurnEffect(Game& game, Player& player) override;

    // Getters
    int getDuration() const { return duration; }
    int getAttackBonus() const { return 0; } // Mutations can modify this
    int getDefenseBonus() const { return 0; } // Mutations can modify this

private:
    int duration; // Number of turns the mutation lasts
};

// Gear Card
class GearCard : public Card {
public:
    GearCard(const std::string& name, const std::string& description,
             int attackBonus = 0, int defenseBonus = 0);
    
    bool canPlay(const Player& player) const override;
    void play(Game& game, Player& player) override;
    void endTurnEffect(Game& game, Player& player) override;

    // Getters
    int getAttackBonus() const { return attackBonus; }
    int getDefenseBonus() const { return defenseBonus; }

private:
    int attackBonus;
    int defenseBonus;
};

// Action Card
class ActionCard : public Card {
public:
    ActionCard(const std::string& name, const std::string& description);
    
    bool canPlay(const Player& player) const override;
    void play(Game& game, Player& player) override;
    void endTurnEffect(Game& game, Player& player) override;
}; 