


//C++ headers
#include<iostream>

//SFML headers
#include <SFML/Graphics.hpp>
//C++ Classes
#include "ModeSelect.h"

// Declare and load a font


//Destructor
ModeSelect::~ModeSelect(){}

//ModeOptions
void ModeSelect::ModeOptions()
{
    ////Load Font
    sf::Font font1;
    font1.loadFromFile("Font/Font1.otf");
    sf::Font font;
    font.loadFromFile("Font/Font2.ttf");
    

    //SFML BG-Texture
    sf::Texture mode_bg;
    mode_bg.loadFromFile("Texture/02ModeSel/sm_bg.png");
    sf::Vector2u bgsize = mode_bg.getSize();
    sf::RectangleShape ModeTxt(sf::Vector2f(bgsize.x, bgsize.y));
    ModeTxt.setTexture(&mode_bg);
    ModeTxt.setFillColor(sf::Color(255, 255, 255, opac));

    //Single Player Rect
    sf::Texture single;
    sf::RectangleShape singlerect(sf::Vector2f(640, 128));
    singlerect.setPosition(sf::Vector2f(300, 200));
    singlerect.setFillColor(hovereffect(300,200,640,128));
    singlerect.setTexture(&single);

    
    
    //Multi Player Rect
    sf::Texture multi;
    sf::RectangleShape multirect(sf::Vector2f(640, 128));
    multirect.setPosition(sf::Vector2f(300, 400));
    multirect.setTexture(&multi);
    multirect.setFillColor(hovereffect(300, 400, 640, 128));

    //For animations
   sf::Clock clk;
   float ElapsedTime = 1.0 + clk.getElapsedTime().asSeconds();
    clk.restart();


    //Entry effect
    if (opac < 255.0f)
       opac += 17.0f * ElapsedTime;
    else if (opac >= 255.0f)
        opac = 255;
  

    //Welcome Player
    sf::Text name("Welcome <PlayerName>", font1,70);
    name.setPosition(400, 50);
    name.setFillColor(sf::Color(255, 255, 255, opac));



    // Create single player text
        sf::Text text("SinglePlayer", font,72);
        /*text.setStyle(sf::Text::Bold);*/
        text.setPosition(450, 210);
        text.setFillColor(sf::Color(255,255,255,opac));

    //MultiPlayer text
        sf::Text text2("MultiPlayer", font,72);
        //text2.setStyle(sf::Text::Bold);
        text2.setPosition(450, 410);
        text2.setFillColor(sf::Color(255, 255, 255, opac));


        //Warning Text
        sf::Text incomplete("Add Multiplayer Option(Bishwash)", font, 70);
        incomplete .setPosition(300, 0);
        incomplete.setFillColor(sf::Color(255, 255, 255, 255));
        //If Clicked 
        if (multirect.getFillColor() == sf::Color(sf::Color(0, 179,0, opac)) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            warning = true;
        }
        if (singlerect.getFillColor() == sf::Color(sf::Color(0, 179, 0, opac)) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            display = false;
        }
         
       
               
    //Draw
        m_window.draw(ModeTxt);
        m_window.draw(singlerect);
        m_window.draw(multirect);
        m_window.draw(name);
        m_window.draw(text);
        m_window.draw(text2);
        if (warning == true)
            m_window.draw(incomplete);
       
}

sf::Color ModeSelect::hovereffect(float xpos, float ypos, float width, float height) 
{
    if ((sf::Mouse::getPosition().x >= xpos && sf::Mouse::getPosition().x <= xpos+width) && (sf::Mouse::getPosition().y >= ypos && sf::Mouse::getPosition().y <= ypos+height))
    {
        //return true;
        return sf::Color(0, 179, 0,opac);
    }
    else
    {
        //return false;
        return sf::Color(sf::Color(255, 0, 0, opac));
    }
}