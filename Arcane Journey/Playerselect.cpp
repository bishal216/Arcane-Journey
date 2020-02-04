
//C++ headers
#include<iostream>

//SFML headers
#include <SFML/Graphics.hpp>
//C++ Classes
#include "Playerselect.h"

//Destructor
Playerselect::~Playerselect() {}

//ModeOptions
void Playerselect::selected()
{
    ////Load Font
    sf::Font font1;
    font1.loadFromFile("Font/Font1.otf");
    float ypos = 400.0f;

    //SFML BG-Texture
    sf::Texture mode_bg;
    mode_bg.loadFromFile("Texture/03CharSel/CharSel.png");
    sf::Vector2u bgsize = mode_bg.getSize();
    sf::RectangleShape ModeTxt(sf::Vector2f(bgsize.x, bgsize.y));
    ModeTxt.setTexture(&mode_bg);
    ModeTxt.setFillColor(sf::Color(255, 255, 255, opac));

    //Melee Rect
    sf::Texture melee;
    std::string meleeidle = "Texture/03CharSel/idle_melee.png";
    std::string meleehover = "Texture/03CharSel/hover_melee.png"; 
    if (hovereffect(300, ypos, 300, 300) == true)
        melee.loadFromFile(meleehover);
    else
        melee.loadFromFile(meleeidle);
    sf::RectangleShape meleerect(sf::Vector2f(300, 300));
    meleerect.setPosition(sf::Vector2f(300, ypos));
    meleerect.setTexture(&melee);



    //Range Rect
    sf::Texture range;
    std::string rangeidle = "Texture/03CharSel/arch_idle.png";
    std::string rangehover = "Texture/03CharSel/arch_hover.png";
    if (hovereffect(800, ypos, 300, 300) == true)
        range.loadFromFile(rangehover);
    else
        range.loadFromFile(rangeidle);
    sf::RectangleShape rangerect(sf::Vector2f(300, 300));
    rangerect.setPosition(sf::Vector2f(800, ypos));
    rangerect.setTexture(&range);

    //Mage Rect
    sf::Texture mage;
    std::string mageidle = "Texture/03CharSel/mage_idle.png";
    std::string magehover = "Texture/03CharSel/mage_hover.png";
    if(hovereffect(1300, ypos, 300, 300)==true)
        mage.loadFromFile(magehover);
    else
        mage.loadFromFile(mageidle);
    sf::RectangleShape magerect(sf::Vector2f(300, 300));
    magerect.setPosition(sf::Vector2f(1300, ypos));
    magerect.setTexture(&mage);
    
    //For animations


    //Entry effect
    if (opac < 255.0f)
        opac += 51.0f;
    else if (opac >= 255.0f)
        opac = 255;


    //Welcome Player
    sf::Text name("Welcome <PlayerName>", font1, 70);
    name.setPosition(300, 50);
    name.setFillColor(sf::Color(0, 0, 0, opac));

    //Choose text
    sf::Text choose("Choose Your Champion", font1, 70);
    choose.setPosition(700, 250);
    choose.setFillColor(sf::Color(0, 0, 0, opac));

    // Create melee text
    sf::Text text("Melee", font1, 50);
    text.setPosition(400, ypos+300);
    text.setFillColor(sf::Color(0, 0, 0, opac));

    //Ranged text
    sf::Text text2("Range", font1, 50);
    text2.setPosition(900, ypos+300);
    text2.setFillColor(sf::Color(0, 0, 0, opac));

    //Mage text
    sf::Text text3("Mage", font1, 50);
    text3.setPosition(1400, ypos+300);
    text3.setFillColor(sf::Color(0, 0, 0, opac));

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        x = choice(hovereffect(300, ypos, 300, 300), hovereffect(800, ypos, 300, 300), hovereffect(1300, ypos, 300, 300));

    //Draw
    m_window.draw(ModeTxt);
    m_window.draw(meleerect);
    m_window.draw(rangerect);
    m_window.draw(magerect);
    m_window.draw(name);
    m_window.draw(choose);
    m_window.draw(text);
    m_window.draw(text2);
    m_window.draw(text3);

}

bool Playerselect::hovereffect(float xpos, float ypos, float width, float height)
{
    if ((sf::Mouse::getPosition().x >= xpos && sf::Mouse::getPosition().x <= xpos + width) && (sf::Mouse::getPosition().y >= ypos && sf::Mouse::getPosition().y <= ypos + height))
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Playerselect::choice(bool x,bool y,bool z)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        
        if (x == true)
        {
            display = false;
            return 1;
        }
            
        else if (y == true)
        {
            display = false;
            return 2;
        }
        else if (z == true)
        {
            display = false;
            return 3;
        }
    }
    
}
