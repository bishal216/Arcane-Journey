#pragma once
#include<SFML/Graphics.hpp>
#include"stats.h"
#include"Board.h"
#include<iostream>
class KnightControl
{
private:
	sf::RenderWindow& m_window;
	int i = 1, j = 20;
	
public:
	KnightControl(sf::RenderWindow& window) : m_window(window) {};
	void loadKnightSprites();
	void drawKnight(Board board);
	void KnightMove();
	void KnightAttack();

};

