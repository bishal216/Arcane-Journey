#pragma once
#include <SFML/Graphics.hpp>
#include"character.h"
#include"map.h"
class game
{
private:
	//Render Window Pointer
	sf::RenderWindow& m_window;
	float opac = 0,speed;
	int x = 0;
	sf::Vector2f start,arrPos;
	int prevpos,currpos,nextpos;
	int prand=0, crand=0, nrand=0;
	bool forward = false;

	character* CharObj = new character(m_window);
	map* prev = new map(m_window);
	map* current = new map(m_window);
	map* next = new map(m_window);
	

public:
	game(sf::RenderWindow& window,sf::Vector2f *spawn, sf::Vector2f* arrPos) :m_window(window),start(*spawn),arrPos(*arrPos) {};
	void loadonce();
	void display(int choice,int count,float et);

};

