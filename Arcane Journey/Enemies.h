#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
#include"Board.h"
#include"stats.h"
class Enemies
{
private:
	sf::RenderWindow& m_window;
	int i = 1, j = 20;
	sf::Vector2f EnemyPosition = sf::Vector2f(2200, 2600);
	sf::Vector2f PlayerPosition;
	int EnemyType = 2;
	int EnemyHealth = 2500;
public:
	Enemies(sf::RenderWindow& window) : m_window(window) {};
	void loadEnemySprites();
	void drawEnemy(sf::Vector2f playerPos,bool attack);
	void Enemymove();
	bool collision();
	void pInteraction(int playerHealth, int EnemyHealth, bool attack);
};

