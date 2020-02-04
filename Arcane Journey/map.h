#pragma once
#include<SFML/Graphics.hpp>
class map
{

private:
	sf::RenderWindow& m_window;

public:

	//Functions
public:
	map(sf::RenderWindow& window) : m_window(window) {};
	~map();
	void load();
private:

};




