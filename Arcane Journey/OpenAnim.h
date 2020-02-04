#pragma once
#include<SFML/Graphics.hpp>
class OpenAnim
{
private:
	//Render Window Pointer
	sf::RenderWindow& m_window;
	float deltaTime;
public:
	//Constructor
	OpenAnim(sf::RenderWindow& window) :m_window(window){};
	//Deconstructor
	~OpenAnim();

	void loadintro( float x);
	bool opendisp = true;
	float xoff = -1920.0f, yoff = -1080.0f, alpha = 0;
};

