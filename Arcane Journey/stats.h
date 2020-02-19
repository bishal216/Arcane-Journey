#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
class stats
{
	private:
		sf::RenderWindow& m_window;
		sf::Texture player[3];
		sf::Font font;
		int score;
		/*MAnage this*/float totalhealth = 390/*Something below 400*&&Use different value for different heroes*/;
		/*MAnage this*/int currenthealth = 120/*Insert logic*/;
		int deletethis = 390;
		/*Take this from login page*/std::string name="GetNameFromLogin";
		/*Manage Highscore*/std::string highscore;

	public:
		stats(sf::RenderWindow& window) :m_window(window) {};
		void loadonce();
		void showstats(int ch);
};

