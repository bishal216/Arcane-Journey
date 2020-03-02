#include "MainMenu.h"

void MainMenu::loadTextures()
{
	font.loadFromFile("Fonts/Enchanted.otf");
	font2.loadFromFile("Fonts/Chomsky.otf");
	bg.loadFromFile("Textures/MainMenu/BG.png");
	controls.loadFromFile("Textures/MainMenu/controls.png");
	player[0].loadFromFile("Textures/MainMenu/Melee.png");
	player[1].loadFromFile("Textures/MainMenu/Archer.png");
	player[2].loadFromFile("Textures/MainMenu/Mage.png");
	for (int i = 0; i < 3; i++)
		player[i].setSmooth(true);
	ch = 0;
	et = 0;
}

int MainMenu::loadScreen(float delTime)
{
	//ElapsedTime
	et += delTime;
	//BG
	sf::RectangleShape Bg(sf::Vector2f(1920, 1080));
	Bg.setTexture(&bg);
	Bg.setFillColor(sf::Color(169, 173, 168, 255));
	m_window.draw(Bg);

	//Control 
	sf::RectangleShape ctrl(sf::Vector2f(700, 700));
	ctrl.setTexture(&controls);
	ctrl.setPosition(sf::Vector2f(1220, 190));
	ctrl.setFillColor(sf::Color(255, 255, 255, 255));
	if(ch==3)
		m_window.draw(ctrl);

	std::string play[] = { "1.Melee","2.Archer","3.Mage","1.Melee&Archer","2.Melee&Mage","3.Archer&Mage" };
	
	
	sf::Text playertxt(play[0], font, 75);
	playertxt.setPosition(sf::Vector2f(800, 950));
	playertxt.setFillColor(sf::Color(168, 179, 185, 255));
	//Players
	if (ch == 0)
	{
		charactersel(0, 1100, 200);
		button(play[0], false, 500, 1225);
		charactersel(1, 1350, 200);
		button(play[1], false, 500, 1500);
		charactersel(2, 1600, 200);
		button(play[2], false, 500, 1725);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
			returnvalue = 1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
			returnvalue = 2;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
			returnvalue = 3;
	}

	if (ch == 1)
	{
		charactersel(0, 1100, 200);
		charactersel(1, 1100, 400);
		button(play[3], false, 800, 1225);
		charactersel(0, 1350, 400);
		charactersel(2, 1350, 200);
		button(play[4], false, 800, 1500);
		charactersel(1, 1600, 200);
		charactersel(2, 1600, 400);
		button(play[5], false, 800, 1725);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
			returnvalue = 4;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
			returnvalue = 5;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
			returnvalue = 6;
	}

	if (ch == 2)
	{
		
		charactersel(1, 1580, 300,true);
		charactersel(2, 1430, 300);
		charactersel(0, 1350, 300);
	}


	if (et >= 0.15)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			ch++;
			et = 0;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			ch--;
			et = 0;
		}
	}
	if (ch >= 5)
		ch = 0;
	if (ch < 0)
		ch = 4;
	for (int i = 0; i < 5; i++)
		active[i] = false;

	active[ch] = true;

	std::string str[] = { "SinglePlayer","Two Players","Three Players","Controls","Exit" };
	for (int i = 0; i < 5; i++)
		button(str[i], active[i], 300 + i * 150);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		switch(ch)
		{
			case 2:
				returnvalue = 7;
			break;

		case 4:
			m_window.close();
		}
	}

	sf::Text select("Enter Option(1,2,3) : ", font, 75);
	select.setPosition(sf::Vector2f(800, 950));
	select.setFillColor(sf::Color(168, 179, 185, 255));
	if (ch < 2)
		m_window.draw(select);

	sf::Text threetxt("Press Space to continue", font, 75);
	threetxt.setPosition(sf::Vector2f(800, 950));
	threetxt.setFillColor(sf::Color(168, 179, 185, 255));
	if (ch == 2)
		m_window.draw(threetxt);
	sf::Text txt("Press Space to exit", font, 75);
	txt.setPosition(sf::Vector2f(800, 950));
	txt.setFillColor(sf::Color(168, 179, 185, 255));

	if(ch==4)
		m_window.draw(txt);

	return returnvalue;
	
}





void MainMenu::button(std::string str,bool large,int ypos,int xpos)
{
	int size=20;
	if (large == true)
	{
		
		size = 125;
	}	
	else
	{
		size = 25;
	}
	sf::Text btntxt(str, font, size);
	btntxt.setPosition(sf::Vector2f(xpos, ypos));
	btntxt.setOrigin(sf::Vector2f(size/ 2, size / 2));
	btntxt.setFillColor(sf::Color(212, 175, 55, 255));
	m_window.draw(btntxt);
}

void MainMenu::charactersel(int choice, float xpos, float ypos,bool flip)
{
	sf::RectangleShape playerRect(sf::Vector2f(300, 300));
	playerRect.setTexture(&player[choice]);
	playerRect.setPosition(sf::Vector2f(xpos, ypos));
	if (flip == true)
		playerRect.setScale(-1, 1);
	m_window.draw(playerRect);
}
