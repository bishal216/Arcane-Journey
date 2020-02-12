#pragma once
#include<SFML/Graphics.hpp>
class ModeSelect
{

private:
	sf::RenderWindow& m_window;
	float opac = 0;
	bool warning = false;
	int &stateMS;

	//Textures
	sf::Font font1,font2;
	sf::Texture mode_bg;

	//Calc
	sf::Vector2u bgsize;


public:
	//Constructor
	ModeSelect(sf::RenderWindow& window,int *state) :m_window(window),stateMS(*state) {};
	void loadonce();
	void ModeOptions(float et);
	sf::Color hovereffect(float xpos, float ypos, float width, float height);

};

