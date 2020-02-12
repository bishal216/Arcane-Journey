#pragma once
#include<SFML/Graphics.hpp>
class OpenAnim
{
private:
	sf::RenderWindow& m_window;
	int &stateOA;
	float xoff = -1920.0f, yoff = -1080.0f, alpha = 0;
	sf::Texture intro_bg, intro_title, intro_arrow;

public:
	//Constructor
	OpenAnim(sf::RenderWindow& window, int* state) :m_window(window), stateOA(*state) {};
	void loadonce();
	void loadintro( float x);
	
};

