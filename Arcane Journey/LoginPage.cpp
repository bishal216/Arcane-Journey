#include "LoginPage.h"
//C++ headers
#include<iostream>

//SFML headers
#include <SFML/Graphics.hpp>

void LoginPage::loadonce()
{
    bg.loadFromFile("Texture/01LoginPage/BG.png");
    font.loadFromFile("Font/Font1.otf");
}

void LoginPage::EnterDetails(float et)
{
    //BG
    sf::Vector2u bgsize = bg.getSize();
    sf::RectangleShape Txt(sf::Vector2f(bgsize.x, bgsize.y));
    Txt.setTexture(&bg);
    Txt.setFillColor(sf::Color(255, 255, 255, alpha));

    //Text    
    sf::Text text("Add A  Login Page Here(Bishant)", font, 100);
    //text.setStyle(sf::Text::Bold);
    text.setPosition(250, 450);
    text.setFillColor(sf::Color(0, 0, 0, alpha));

    //Entry effect
    if (alpha < 255.0f)
        alpha += 51.0f*et;
    else if (alpha >= 255.0f)
        alpha = 255;
    if (alpha == 255)
        delay++;
    
        
    m_window.draw(Txt);
    m_window.draw(text);
    if (delay == 200)
    {
        stateLP = 3;
        delete this;
    }
}
