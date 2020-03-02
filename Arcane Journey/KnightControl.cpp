#include "KnightControl.h"

void KnightControl::loadKnightSprites()
{

}

void KnightControl::drawKnight(Board board)
{
	sf::RectangleShape knight(sf::Vector2f(50, 50));
	knight.setPosition(board.TilePosition(i,j));
	knight.setFillColor(sf::Color(255, 0, 0, 255));
	knight.setOrigin(sf::Vector2f(25, -25));
	m_window.draw(knight);
}

void KnightControl::KnightMove()
{
}

void KnightControl::KnightAttack()
{
}
