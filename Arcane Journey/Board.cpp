#include "Board.h"
void Board::loadTexture()
{
	boardTexture[0].loadFromFile("Textures/Board/wood.jpg") ;
	boardTexture[1].loadFromFile("Textures/Board/grass0.jpg") ;
	boardTexture[2].loadFromFile("Textures/Board/grass1.jpg") ;

	castleTexture[0].loadFromFile("Textures/Board/castle_100.jpg");
	castleTexture[1].loadFromFile("Textures/Board/castle_50.jpg");
	castleTexture[2].loadFromFile("Textures/Board/castle_25.jpg");

}


void Board::loadBoard()
{
	
	boardmove();
	for (int i = 0; i < 22; i++)
	{
		for (int j = 0; j < 22; j++)
		{
			Tiles[i][j].setSize(sf::Vector2f(sqsize,sqsize));
			Tiles[i][j].setOrigin(sf::Vector2f(sqsize / 2, sqsize / 2));
			Tiles[i][j].setPosition(sf::Vector2f(-1000+i* sqsize +boardposx,-1500+j* sqsize + boardposy));

			if (i == 0 || i == 21 || j == 0 || j == 21)
				Tiles[i][j].setTexture(&boardTexture[0]);
				//setFillColor(sf::Color(210, 105, 30, 255));
			else if ((i + j) % 2 == 0)
				Tiles[i][j].setTexture(&boardTexture[1]);
				//setFillColor(sf::Color(229, 223, 136, 255));
			else
				Tiles[i][j].setTexture(&boardTexture[2]);
				//setFillColor(sf::Color(86, 125, 70, 255));
			m_window.draw(Tiles[i][j]);
		}
	}




}

void Board::boardmove()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		boardposy+=5;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		boardposy-=5;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		boardposx+=5;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		boardposx-=5;

	if (boardposx-sqsize/2 >= 1000)
		boardposx = 1000+sqsize/2;
	if (21.5 * sqsize + boardposx <= 2920)
		 boardposx = 2920 - 22 * sqsize + sqsize / 2;
	if ( boardposy - sqsize / 2 >= 1500)
		boardposy = 1500 + sqsize / 2;
	if (22 * sqsize - sqsize / 2 + boardposy <= 2580)
		boardposy = 2580 - 22*sqsize + sqsize / 2;

}

sf::Vector2f Board::TilePosition(int i, int j)
{
	sf::Vector2f temp;
	temp = Tiles[i][j].getPosition();
	return temp;
}
