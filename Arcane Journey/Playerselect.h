#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
class Playerselect
{
private:
	//Render Window Pointer
	sf::RenderWindow& m_window;
	float opac = 0;
	

public:
	int x = 0;

public:
	//Constructor
	Playerselect(sf::RenderWindow& window) :m_window(window) {};
	//Deconstructor
	~Playerselect();

	void selected();
	bool hovereffect(float xpos, float ypos, float width, float height);
	int choice(bool x, bool y, bool z);
	bool display = true;
};

