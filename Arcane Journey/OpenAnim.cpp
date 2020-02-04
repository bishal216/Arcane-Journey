//C++ headers
#include<iostream>

//SFML headers
#include <SFML/Graphics.hpp>

//Class files
#include "OpenAnim.h"

//Global Variables
float intro_x, intro_y;

//Destructor
OpenAnim::~OpenAnim() {}

//loadintro
void OpenAnim::loadintro(float x)
{
    //SFML BG-Texture
        sf::Texture intro_bg;
        intro_bg.loadFromFile("Texture/00OpeningIntro/OpenBg.png");
        sf::Vector2u openbgsize = intro_bg.getSize();
        intro_x = openbgsize.x;
        intro_y = openbgsize.y;
        sf::RectangleShape OpeningIntro(sf::Vector2f(intro_x, intro_y));
        OpeningIntro.setTexture(&intro_bg);
       OpeningIntro.setFillColor(sf::Color(255, 255, 255, alpha));

    //Game Title
        sf::Texture intro_title;
        intro_title.loadFromFile("Texture/00OpeningIntro/Title.png");
        sf::RectangleShape OpeningTitle(sf::Vector2f(intro_x, intro_y));
        OpeningTitle.setTexture(&intro_title);
        OpeningTitle.setPosition(sf::Vector2f(0.0f, yoff));
        
    //Title Logo
        sf::Texture intro_arrow;
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

    //Draw
        m_window.draw(OpeningIntro);
        m_window.draw(OpeningTitle);
        m_window.draw(OpeningArrow);
        if (xoff == 0)
        {
            opendisp = false;
        }     
}



