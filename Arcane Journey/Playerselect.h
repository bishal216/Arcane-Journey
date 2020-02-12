#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
class Playerselect
{
private:
	sf::RenderWindow& m_window;
	float opac = 0;
	int& statePS;
	int& x;

	sf::Font font1;
	sf::Texture mode_bg, melee_idle,melee_hover,r_idle,r_hover,mage_i,mage_h;



public:
	//Constructor
	Playerselect(sf::RenderWindow& window,int *state,int * option) :m_window(window),statePS(*state),x(*option) {};

	void loadonce();

	void selected();
	bool hovereffect(float xpos, float ypos, float width, float height);
	int choice(bool x, bool y, bool z);
};

