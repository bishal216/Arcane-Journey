#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "systems/Achievement.hpp"

class AchievementManager {
   public:
    AchievementManager();

    // Called at end of every completed run
    void onRunComplete(const RunResult& result);

    // Called when player talks to Chronicler — returns id of claimed reward artifact (-1 if none)
    int claimNext();
    bool hasUnclaimed() const;

    // Serialisation helpers (used by SaveData)
    std::vector<bool> completedBits() const;
    std::vector<bool> claimedBits() const;
    void restoreBits(const std::vector<bool>& completed, const std::vector<bool>& claimed);

    // Chronicler UI draw
    void drawChronicler(sf::RenderWindow& window, const sf::Font& font, float bestTime,
                        int& cursor) const;

    const std::vector<Achievement>& achievements() const { return m_achievements; }

   private:
    std::vector<Achievement> m_achievements;
    void buildList();
};

extern AchievementManager g_achievements;