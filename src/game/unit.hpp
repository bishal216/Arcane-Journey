#pragma once
#include <string>
#include <iostream>

enum class TriggerType {
    None,
    OnAttack,
    OnFaint,
    OnStartOfBattle
};

class Unit {
private:
    // Unit name
    std::string name;
    // Unit attributes
    int attack;
    int health;
    int level;
    int maxHealth;
    int tier;
    TriggerType trigger;
    std::string abilityDescription;

public:
    Unit(std::string name, int attack, int health, int tier,
         TriggerType trigger = TriggerType::None,
         std::string abilityDesc = "");

    // Accessors
    const std::string& getName() const;
    int getAttack() const;
    int getHealth() const;
    int getLevel() const;
    int getTier() const;
    TriggerType getTrigger() const;
    const std::string& getAbilityDescription() const;

    // Mutators
    void setAttack(int value);
    void setHealth(int value);
    void takeDamage(int dmg);
    bool isAlive() const;
    void levelUp();

    // Debug/print
    void printInfo() const;
};
