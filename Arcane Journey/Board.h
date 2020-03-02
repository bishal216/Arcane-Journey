#pragma once
#include<SFML/Graphics.hpp>
class Board
{
private:
	
	
	sf::RenderWindow& m_window;
	

	sf::RectangleShape Tiles[22][22];
	//Size of a tile
	int sqsize = 150;

	sf::Texture boardTexture[3];
	sf::Texture castleTexture[3];
	float boardposx=0, boardposy=0;
public:

	Board(sf::RenderWindow& window) : m_window(window) {};
	void loadTexture();
	void loadBoard();
	void boardmove();

	sf::Vector2f TilePosition(int i, int j);
};

