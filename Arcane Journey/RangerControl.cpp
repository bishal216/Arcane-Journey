#include "RangerControl.h"

void RangerControl::loadRangerSprites()
{
}

void RangerControl::drawRanger(Board board)
{
	sf::RectangleShape ranger(sf::Vector2f(50, 50));
	ranger.setPosition(board.TilePosition(i, j));
	ranger.setFillColor(sf::Color(0, 255, 0, 255));
	ranger.setOrigin(sf::Vector2f(25, 25));
	m_window.draw(ranger);
}

void RangerControl::RangerMove()
{
}

void RangerControl::RangerAttack()
{
}
