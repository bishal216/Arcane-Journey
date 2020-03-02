#include "Enemies.h"

void Enemies::loadEnemySprites()
{
}

void Enemies::drawEnemy(sf::Vector2f playerPos,bool attack)
{
	sf::RectangleShape Enemy(sf::Vector2f(200,200));
	PlayerPosition = playerPos;
	std::cout << "PlayerPosition is : " << PlayerPosition.x << "\t" << PlayerPosition.y << std::endl;
	switch (EnemyType)
	{
	case 1:
		Enemy.setFillColor(sf::Color(255, 0, 255, 255));
		break;
	case 2:
		Enemy.setFillColor(sf::Color(0, 255, 255, 255));
		break;
	case 3:
		Enemy.setFillColor(sf::Color(0, 173, 255, 255));
		break;
	default:
		Enemy.setFillColor(sf::Color(17, 143, 255, 255));
		break;
	}

	Enemy.setPosition(EnemyPosition);
	Enemy.setOrigin(sf::Vector2f(100, 200));
	Enemymove();
	
		

	m_window.draw(Enemy);

}

void Enemies::Enemymove()
{
	if (PlayerPosition.x <= EnemyPosition.x - 150 || PlayerPosition.x >= EnemyPosition.x + 150)
	{
		if (PlayerPosition.x > EnemyPosition.x)
			EnemyPosition.x++;
		if (PlayerPosition.x < EnemyPosition.x)
			EnemyPosition.x--;
	}
	//dEnemyPosition.x = PlayerPosition.x + (EnemyPosition.x - PlayerPosition.x) / 10;
	EnemyPosition.y = PlayerPosition.y+(EnemyPosition.y - PlayerPosition.y) / 10;

}

bool Enemies::collision()
{
	bool collide;
	if (PlayerPosition.x >= EnemyPosition.x - 150 && PlayerPosition.x <= EnemyPosition.x + 150)
		collide = true;
	else
		collide = false;
	return collide;

}

void Enemies::pInteraction(int PlayerHealth,int EnemyHealth, bool attack)
{
	if (collision() == true)
	{
		PlayerHealth--;
		if (attack == true)
			EnemyHealth--;
	}
}