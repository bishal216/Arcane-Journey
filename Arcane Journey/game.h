#pragma once
#include <SFML/Graphics.hpp>
class game
{
private:
	//Render Window Pointer
	sf::RenderWindow& m_window;
	float opac = 0;
	int x = 0;
	sf::Vector2f start;
public:
	game(sf::RenderWindow& window,sf::Vector2f *spawn) :m_window(window),start(*spawn) {};

	void display(int choice,int count);

};

