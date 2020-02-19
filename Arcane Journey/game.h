#pragma once
#include <SFML/Graphics.hpp>
#include"character.h"
#include"map.h"
#include"stats.h"
class game
{
private:
	//Render Window Pointer
	sf::RenderWindow& m_window;
	float opac = 0,speed;
	int x = 0;

	//playerposition
	sf::Vector2f start = sf::Vector2f(500.0f, 540.0f);
	sf::Vector2f arrPos = start;

	//Map rotation
	int prevpos,currpos,nextpos;
	int prand=0, crand=0, nrand=0;
	bool forward = false;

	//Objects
	character* CharObj = new character(m_window);
	map* prev = new map(m_window);
	map* current = new map(m_window);
	map* next = new map(m_window);
	stats* Stat = new stats(m_window);
	

public:
	game(sf::RenderWindow& window) :m_window(window) {};
	void loadonce();
	void display(int choice,int count,float et);

};

