#pragma once
#include<SFML/Graphics.hpp>
class map
{

private:
	sf::RenderWindow& m_window;
	sf::Texture junglemap[6];

	int r,g,b;

public:

	//Functions
public:
	map(sf::RenderWindow& window) : m_window(window) {};
	~map();
	void load(int mappos,int random);
	void loadonce();
private:

};




