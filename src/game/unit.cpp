#include "unit.hpp"
// Constructor for the Unit class
// Initializes the unit's attributes such as name, attack, health, level, tier, trigger type, and ability description.
Unit::Unit(std::string name, int attack, int health, int tier,
           TriggerType trigger, std::string abilityDesc)
    : name(std::move(name)), attack(attack), health(health),
      level(1), tier(tier), trigger(trigger),
      abilityDescription(std::move(abilityDesc)) {}

//   Accessors
const std::string& Unit::getName() const { return name; }
int Unit::getAttack() const { return attack; }
int Unit::getHealth() const { return health; }
int Unit::getLevel() const { return level; }
int Unit::getTier() const { return tier; }
TriggerType Unit::getTrigger() const { return trigger; }
const std::string& Unit::getAbilityDescription() const { return abilityDescription; }

//  Mutators
void Unit::setAttack(int value) { attack = value; }
void Unit::setHealth(int value) { health = value; }
void Unit::takeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
        std::cout << name << " takes " << dmg << " damage! Health: " << health << "\n";
}
bool Unit::isAlive() const { return health > 0; }

void Unit::levelUp() {
    if (level < 3) {
        ++level;
        attack += 1;
        health += 1;
    }
}

// Print information about the unit, including its name, tier, level, attack, health, and ability description.
void Unit::printInfo() const {
    std::cout << name << " (Tier " << tier << ", Lvl " << level << ") - "
              << attack << "/" << health << " - Ability: "
              << abilityDescription << "\n";
}
