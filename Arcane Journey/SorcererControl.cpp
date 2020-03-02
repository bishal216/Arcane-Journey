#include "SorcererControl.h"

void SorcererControl::loadSorcererSprites()
{
}

void SorcererControl::drawSorcerer(Board board)
{
	sf::RectangleShape sorcerer(sf::Vector2f(50, 50));
	sorcerer.setPosition(board.TilePosition(i, j));
	sorcerer.setFillColor(sf::Color(0, 0, 255, 255));
	sorcerer.setOrigin(sf::Vector2f(25, 75));
	m_window.draw(sorcerer);
}

void SorcererControl::SorcererMove()
{


}

void SorcererControl::SorcererAttack()
{
}
