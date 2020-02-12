#include<iostream>
//#include <SFML/Graphics.hpp>
#include "OpenAnim.h"

void OpenAnim::loadonce ()
{
    intro_bg.loadFromFile("Texture/00OpeningIntro/OpenBg.png");
    intro_title.loadFromFile("Texture/00OpeningIntro/Title.png");
    intro_arrow.loadFromFile("Texture/00OpeningIntro/titlearrow.png");
}

//loadintro
void OpenAnim::loadintro(float x)
{   
    //SFML BG-Texture 
        sf::RectangleShape OpeningIntro(sf::Vector2f(1920, 1080));
        OpeningIntro.setTexture(&intro_bg);
        OpeningIntro.setFillColor(sf::Color(255, 255, 255, alpha));

    //Game Title
        sf::RectangleShape OpeningTitle(sf::Vector2f(1920, 1080));
        OpeningTitle.setTexture(&intro_title);
        OpeningTitle.setPosition(sf::Vector2f(0.0f, yoff));
        
    //Title Logo
        sf::Sprite OpeningArrow;
        OpeningArrow.setTexture(intro_arrow);
        OpeningArrow.setPosition(sf::Vector2f(xoff, 0.0f));
    
        
   //BG-Effect
        if (alpha < 255.0f)
            alpha += 255.0f * x;
        else if (alpha >= 255.0f)
            alpha = 255.0f;

    //Title Entering
        if (alpha == 255)
        {
            if (yoff < 0.0f)
                yoff = yoff + 384.0f*x;
            else if (yoff >= 0.0f)
                yoff = 0.0f;         
        }
    //Arrow Entering
        if (yoff == 0.0f)
        {
            if (xoff < 0.0f)
                xoff = xoff + 512.0f*x;
            else if (xoff >= 0.0f)
                xoff = 0.0f;
        }   
    //Draw
        m_window.draw(OpeningIntro);
        m_window.draw(OpeningTitle);
        m_window.draw(OpeningArrow);
        if (xoff == 0)
        {
            stateOA = 2;
            delete this;
        }        
}



