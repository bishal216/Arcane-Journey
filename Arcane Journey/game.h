#pragma once
#include <SFML/Graphics.hpp>
#include"map.h"
#include"PlayerEnemyInteraction.h"

#include"Board.h"

class game
{
private:
	//Render Window Pointer
	sf::RenderWindow& m_window;
	sf::Font font;
	//For map
	map* prev = new map(m_window);
	map* current = new map(m_window);
	map* next = new map(m_window);

	//Player Enemy Interaction
	PlayerEnemyInteraction player[3] = { m_window ,m_window ,m_window };

	Board* board = new Board(m_window);
	//playerposition
	sf::Vector2f start = sf::Vector2f(500.0f, 540.0f);
	sf::Vector2f arrPos = start;

	//Map rotation
	int prevpos,currpos,nextpos;
	int prand=0, crand=0, nrand=0;
	bool forward = false;
	bool f[3] = { true,true,true };
	bool playerAlive[3] = { false,false,false };
	int fill=0;
	//Objects
	sf::RectangleShape prog;
	sf::Texture swordCross;

public:
	game(sf::RenderWindow& window) :m_window(window) {};
	void loadonce();
	void display(int *choice,int count,float et);
	void playerDead(int ch);

	void Progress();

};

