#pragma once
#include <SFML/Graphics.hpp>
class LoginPage
{
private:
	sf::RenderWindow& m_window;
	int delay = 0,&stateLP;
public:
	LoginPage(sf::RenderWindow& window, int* state) :m_window(window),stateLP(*state) {};
	void EnterDetails();
	float alpha = 0;

};

