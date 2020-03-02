#pragma once
#include<SFML/Graphics.hpp>
class Animation
{
private:
	//Window
	sf::RenderWindow& m_window;
	//State
	int& AnimationState;
	//Opacity
	float alpha = 0, alphac = 0, alpha3 = 0;
	float scroll = 0;
	//Textures
	sf::Texture bg, Knight, Hoptiles, Halberdiers;
	sf::Font font, font2;

	//Others
	int ch = 1, chaar = 0;
	float ElpasedTime = 0;
public:
	//Constructor
	Animation(sf::RenderWindow& window, int* state) :m_window(window), AnimationState(*state) {};
	//Functions
	void loadTextures();
	void showAnimation(float delTime);
};

