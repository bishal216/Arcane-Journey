#pragma once
#include <SFML/Graphics.hpp>
class LoginPage
{

//For variables

private:
	sf::RenderWindow& m_window;
	int delay = 0,&stateLP;
	sf::Texture bg;
	sf::Font font;

public:
	float alpha = 0;

//For functions


public:
	LoginPage(sf::RenderWindow& window, int* state) :m_window(window),stateLP(*state) {};
	void loadonce();
	void EnterDetails(float et);
	

};

