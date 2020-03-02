#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
class stats
{
	protected:
		sf::RenderWindow& m_window;
		sf::Texture player[3];
		sf::Font font;
		int score;
		int playerChoice;
		float totalhealth[3] = {5000,4000,4500};
		int currenthealth[3] = {5000,4000,4500};
		std::string highscore;
		std::string name[3] = { "Paladin","Knight","Fighter" };

	public:
		stats();
		stats(sf::RenderWindow& window) :m_window(window) {};
		void loadonce();
		void showstats(int ch,float* pHth,float* eHth);
};

