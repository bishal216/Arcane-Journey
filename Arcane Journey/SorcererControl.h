#pragma once
#pragma once
#include<SFML/Graphics.hpp>
#include"stats.h"
#include"Board.h"
#include<iostream>
class SorcererControl
{
private:
	sf::RenderWindow& m_window;
	int i = 1, j = 20;

public:
	SorcererControl(sf::RenderWindow& window) : m_window(window) {};
	void loadSorcererSprites();
	void drawSorcerer(Board board);
	void SorcererMove();
	void SorcererAttack();

};

