#pragma once
#include<SFML/Graphics.hpp>
class ModeSelect
{
private:
	//Render Window Pointer
	sf::RenderWindow& m_window;
	float opac = 0;
	bool warning = false;
public:
	//Constructor
	ModeSelect(sf::RenderWindow& window) :m_window(window) {};
	//Deconstructor
	~ModeSelect();

	void ModeOptions();
	sf::Color hovereffect(float xpos, float ypos, float width, float height);

	bool display = true;
};

