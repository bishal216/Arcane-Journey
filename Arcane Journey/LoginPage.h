#pragma once
#include <SFML/Graphics.hpp>
class LoginPage
{
private:
	sf::RenderWindow& m_window;
	int delay = 0;
public:
	LoginPage(sf::RenderWindow& window) :m_window(window) {};
	void EnterDetails();
	float alpha = 0;
	bool display = true;

};

