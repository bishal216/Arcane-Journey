#include<iostream>
#include "Playerselect.h"

//load once
void Playerselect::loadonce()
{
    font1.loadFromFile("Font/Font1.otf");
    mode_bg.loadFromFile("Texture/03CharSel/CharSel.png");

    melee_hover.loadFromFile("Texture/03CharSel/hover_melee.png");
    melee_idle.loadFromFile("Texture/03CharSel/idle_melee.png");

    r_hover.loadFromFile("Texture/03CharSel/arch_hover.png");
    r_idle.loadFromFile("Texture/03CharSel/arch_idle.png");

    mage_i.loadFromFile("Texture/03CharSel/mage_idle.png");
    mage_h.loadFromFile("Texture/03CharSel/mage_hover.png");

}


//ModeOptions
void Playerselect::selected()
{
    //SFML BG-Texture
    sf::RectangleShape ModeTxt(sf::Vector2f(1920, 1080));
    ModeTxt.setTexture(&mode_bg);
    ModeTxt.setFillColor(sf::Color(255, 255, 255, opac));

    //Melee Rect
    sf::RectangleShape meleerect(sf::Vector2f(300, 300));
    meleerect.setPosition(sf::Vector2f(300, 400));   
    if (hovereffect(300, 400, 300, 300) == true)
        meleerect.setTexture(&melee_hover);
    else
        meleerect.setTexture(&melee_idle);

    //Range Rect
    sf::RectangleShape rangerect(sf::Vector2f(300, 300));
    rangerect.setPosition(sf::Vector2f(800, 400));
    if (hovereffect(800, 400, 300, 300) == true)
        rangerect.setTexture(&r_hover);
    else
        rangerect.setTexture(&r_idle);
    
    

    //Mage Rect
    sf::RectangleShape magerect(sf::Vector2f(300, 300));
    magerect.setPosition(sf::Vector2f(1300, 400));
    if(hovereffect(1300, 400, 300, 300)==true)
        magerect.setTexture(&mage_h);
    else
        magerect.setTexture(&mage_i);
    
    
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
    text.setPosition(400, 400+300);
    text.setFillColor(sf::Color(0, 0, 0, opac));

    //Ranged text
    sf::Text text2("Range", font1, 50);
    text2.setPosition(900, 400+300);
    text2.setFillColor(sf::Color(0, 0, 0, opac));

    //Mage text
    sf::Text text3("Mage", font1, 50);
    text3.setPosition(1400, 400+300);
    text3.setFillColor(sf::Color(0, 0, 0, opac));

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        x = choice(hovereffect(300, 400, 300, 300), hovereffect(800, 400, 300, 300), hovereffect(1300, 400, 300, 300));

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

    if (x != 0)
    {
        statePS = 5;
        delete this;
    }

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
            return 1;
        else if (y == true)
            return 2;
        else if (z == true)
            return 3;
        else
            return 0;
    }
    
}
