#include<iostream>
#include <SFML/Graphics.hpp>
#include "OpenAnim.h"


//loadintro
void OpenAnim::loadintro(float x)
{
    sf::Texture intro_bg, intro_title, intro_arrow;
    
    //SFML BG-Texture
        
        intro_bg.loadFromFile("Texture/00OpeningIntro/OpenBg.png");
        sf::RectangleShape OpeningIntro(sf::Vector2f(1920, 1080));
        OpeningIntro.setTexture(&intro_bg);
        OpeningIntro.setFillColor(sf::Color(255, 255, 255, alpha));

    //Game Title
        intro_title.loadFromFile("Texture/00OpeningIntro/Title.png");
        sf::RectangleShape OpeningTitle(sf::Vector2f(1920, 1080));
        OpeningTitle.setTexture(&intro_title);
        OpeningTitle.setPosition(sf::Vector2f(0.0f, yoff));
        
    //Title Logo
        intro_arrow.loadFromFile("Texture/00OpeningIntro/titlearrow.png");
        sf::Sprite OpeningArrow;
        OpeningArrow.setTexture(intro_arrow);
        OpeningArrow.setPosition(sf::Vector2f(xoff, 0.0f));
    
        
        //BG-Effect
        if (alpha < 255.0f)
            alpha += 51.0f * x;
        else if (alpha >= 255.0f)
            alpha = 255.0f;

    //Title Entering
        if (alpha == 255)
        {
            if (yoff < 0.0f)
                yoff = (yoff + 54.0f);
            else if (yoff >= 0.0f)
                yoff = 0.0f;
            
        }
    //Arrow Entering
        if (yoff == 0.0f)
        {
            if (xoff < 0.0f)
                xoff = xoff + 384.0f;
            else if (xoff >= 0.0f)
                xoff = 0.0f;
        }


        if (xoff == 0)
        {
            stateOA = 2;
            delete this;
        }
            
    //Draw
        m_window.draw(OpeningIntro);
        m_window.draw(OpeningTitle);
        m_window.draw(OpeningArrow);
         
}



