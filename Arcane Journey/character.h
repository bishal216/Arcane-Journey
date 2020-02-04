#pragma once
#include<SFML/Graphics.hpp>
class character
{

private:
	sf::RenderWindow& m_window;
	bool faceRight = true;
	bool walking = false;
	sf::RectangleShape playerrect;

public:
	
	//Functions
public:
	character(sf::RenderWindow& window) : m_window(window){};
	~character();
	void load(int choice,int count,sf::Vector2f *start);

	


	void idleanim(int choice,int count,sf::Vector2f *start);
	void movement(int choice, int count,sf::Vector2f *start);
	void attack(int choice, int count, sf::Vector2f* start);
private:

};




