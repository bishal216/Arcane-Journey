#include "OpenAnim.h"
#include<iostream>

void Animation::loadTextures()
{
    bg.loadFromFile("Textures/Animation/openBg.png");
    Knight.loadFromFile("Textures/Animation/Knight.png");
    Hoptiles.loadFromFile("Textures/Animation/Hoptile.png");
    Halberdiers.loadFromFile("Textures/Animation/Halberdier.png");

    font.loadFromFile("Fonts/Enchanted.otf");
    font2.loadFromFile("Fonts/Confortaa.ttf");
}

//loadintro
void Animation::showAnimation(float delTime)
{
    m_window.setMouseCursorVisible(false);
    //SFML BG-Texture 
    sf::RectangleShape introBg(sf::Vector2f(1920, 1080));
    introBg.setTexture(&bg);
    introBg.setFillColor(sf::Color(255, 255, 255, alpha));
    //BG effects
    alpha += 2.5;
    if (alpha >= 255)
        alpha = 255;


    //SFML Characters
    sf::RectangleShape characters(sf::Vector2f(720, 400));
    characters.setPosition(sf::Vector2f(1220, scroll));
    //Character Effects
    if (alpha == 255)
        alphac += 2.5;
    //Character Scroll
    switch (ch)
    {
    case 1:
        characters.setTexture(&Hoptiles);
        if (alphac >= 255)
            ch = 2;
        break;
    case 2:
        characters.setTexture(&Knight);
        if (alphac >= 255)
            ch = 3;
        break;
    case 3:
        characters.setTexture(&Halberdiers);
        if (alphac >= 255)
            ch = 1;
        break;
    }
    characters.setFillColor(sf::Color(255, 255, 255, alphac));
    if (alphac >= 255)
        alphac = 0;
    scroll++;
    if (scroll >= 1080)
        scroll = -500;

    //Text    
    std::string str = "AR CANE\nJOURNEY";
    sf::Text text(str.substr(0, chaar), font, 200);
    text.setPosition(100, 250);
    text.setFillColor(sf::Color(168, 169, 173, alpha));
    //Text effect
    if (alpha == 255)
        ElpasedTime += 2 * delTime;
    chaar = (int)(ElpasedTime*4);

    sf::Text text2("Press Spacebar to continue", font2, 50);
    text2.setPosition(700, 850);
    text2.setFillColor(sf::Color(168, 169, 173, alpha3));


    if (ElpasedTime >= 20)
    {
        alpha3 += 2.5;
        if (alpha3 >= 255)
            alpha3 = 255;
    }

    //DRAW
    m_window.draw(introBg);
    m_window.draw(characters);
    m_window.draw(text);
    m_window.draw(text2);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        AnimationState = 2;
    }
}



