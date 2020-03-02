#pragma once
#include<SFML/Graphics.hpp>
#include"stats.h"
#include"Board.h"
#include<iostream>
class RangerControl
{
private:
	sf::RenderWindow& m_window;
	int i = 1, j = 20;

public:
	RangerControl(sf::RenderWindow& window) : m_window(window) {};
	void loadRangerSprites();
	void drawRanger(Board board);
	void RangerMove();
	void RangerAttack();

};

