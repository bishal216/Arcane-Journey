#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
class MainMenu
{
private:
	//Window
	sf::RenderWindow& m_window;
	//State
	int& MenuState;
	bool active[5] = {false,false,false,false,false};
	int ch=0;
	sf::Font font,font2;
	float et=0;
	sf::Texture controls,bg,player[3];
	int choice=0;
	int returnvalue=0;

public:
	MainMenu(sf::RenderWindow& window, int* state) :m_window(window), MenuState(*state) {};
	//Functions
	void loadTextures();

	int loadScreen(float delTime);
	void button(std::string str,bool large,int ypos,int xpos =200);
	void charactersel(int choice,float xpos, float ypos,bool flip = false);

};

