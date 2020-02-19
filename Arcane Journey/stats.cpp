#include "stats.h"
void stats::loadonce()
{
	player[0].loadFromFile("Texture/07Stats/meleeicon.png");
	player[1].loadFromFile("Texture/07Stats/rangeicon.png");
	player[2].loadFromFile("Texture/07Stats/mageicon.png");
	font.loadFromFile("Font/Font4.ttf");
	score = 0;
}
void stats::showstats(int ch)
{
	//Playername
	sf::Text pname(name,font,25);
	pname.setPosition(100, 20);
	pname.setFillColor(sf::Color(0, 0, 0, 255));
	
	score++;
	
	/*Dummy score<Logic not added>*/highscore = "Score : "+std::to_string((score%100));
	sf::Text scr(highscore, font, 25);
	scr.setPosition(100, 75);
	scr.setFillColor(sf::Color(0, 0, 0, 255));

	//Playericon
	sf::CircleShape icon(50);
	icon.setPosition(0, 0);
	icon.setFillColor(sf::Color(255, 255, 255, 255));
	if (ch == 1)
		icon.setTexture(&player[0]);
	if (ch == 2)
		icon.setTexture(&player[1]);
	if (ch == 3)
		icon.setTexture(&player[2]);
	
	//Background
	sf::Vertex vertices[] =
	{
		sf::Vertex(sf::Vector2f(0,   0), sf::Color(0, 192, 255, 220) , sf::Vector2f(0,  0)),
		sf::Vertex(sf::Vector2f(0, 150), sf::Color(192, 0, 255, 220) , sf::Vector2f(0, 10)),
		sf::Vertex(sf::Vector2f(400, 150), sf::Color(0, 0, 255, 220), sf::Vector2f(10, 10)),
		sf::Vertex(sf::Vector2f(400,   0), sf::Color(192, 192, 255, 220),sf::Vector2f(10,  0))
	};

	//Totalhealth
	sf::RectangleShape total_hth(sf::Vector2f(350, 25));
	total_hth.setFillColor(sf::Color(255, 255, 255, 255));
	total_hth.setPosition(30,120);

	sf::Text health (std::to_string(currenthealth)+"/"+std::to_string((int)totalhealth), font, 25);
	health.setPosition(170, 120);
	health.setFillColor(sf::Color(0, 0, 0, 255));


	//CurrentHealth
	if (deletethis == 390)
	{
		currenthealth--;
		if (currenthealth == 0)
			deletethis = 0;
	}
		
	if (deletethis == 0)
	{
		currenthealth++;
		if (currenthealth == 390)
			deletethis = 390;
	}
		
	sf::RectangleShape current_hth(sf::Vector2f(currenthealth/totalhealth*350, 25));

	if(currenthealth / totalhealth > .66)
		current_hth.setFillColor(sf::Color(0, 255, 0, 255));
	else if (currenthealth / totalhealth <.33)
		current_hth.setFillColor(sf::Color(255, 0, 0, 255));
	else
		current_hth.setFillColor(sf::Color(255, 255, 0, 255));
	current_hth.setPosition(30, 120);


	


	m_window.draw(vertices, 4, sf::Quads);
	m_window.draw(icon);
	m_window.draw(pname);
	m_window.draw(scr);
	m_window.draw(total_hth);
	m_window.draw(current_hth);
	m_window.draw(health);
}
