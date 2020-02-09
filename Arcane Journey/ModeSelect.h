#pragma once
#include<SFML/Graphics.hpp>
class ModeSelect
{
private:
	sf::RenderWindow& m_window;
	float opac = 0;
	bool warning = false;
	int &stateMS;
public:
	//Constructor
	ModeSelect(sf::RenderWindow& window,int *state) :m_window(window),stateMS(*state) {};

	void ModeOptions();
	sf::Color hovereffect(float xpos, float ypos, float width, float height);

};

