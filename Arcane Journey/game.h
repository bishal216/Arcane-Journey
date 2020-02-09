#pragma once
#include <SFML/Graphics.hpp>
class game
{
private:
	//Render Window Pointer
	sf::RenderWindow& m_window;
	float opac = 0;
	int x = 0;
	sf::Vector2f start,arrPos;
	int mapno = 0;
	int mappos = mapno * 1920;
	bool forward = false;
public:
	game(sf::RenderWindow& window,sf::Vector2f *spawn, sf::Vector2f* arrPos) :m_window(window),start(*spawn),arrPos(*arrPos) {};

	void display(int choice,int count);

};

