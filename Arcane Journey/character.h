#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
class character
{

private:
	sf::RenderWindow& m_window;
	bool faceRight = true;
	bool walking = false;
	sf::RectangleShape playerrect;

	sf::Texture meleeidle[10], meleeatt[10], meleewalk[10];
	sf::Texture rangeidle[10], rangeatt[10], rangewalk[10];
	sf::Texture mageidle[10], mageatt[10], magewalk[10];
	sf::Texture arrow,aoe;


public:
	
	//Functions
public:
	character(sf::RenderWindow& window) : m_window(window){};
	~character();

	void loadonce();
	void load(int choice,int count,sf::Vector2f *start, sf::Vector2f* arrPos,bool *forward,float et,float *speed);

	
	void idleanim(int choice,int count,sf::Vector2f *start,float et, float* speed);
	void movement(int choice, int count,sf::Vector2f *start,float et, float* speed);
	void attack(int choice, int count, sf::Vector2f* start,float et, float* speed);
private:

};




