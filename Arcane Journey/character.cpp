#include "character.h"
#include"SoundEff.h"
#include<iostream>
#include<SFML/Graphics.hpp>
character::~character()
{
}
void character::load(int choice,int count,sf::Vector2f *start, sf::Vector2f* arrPos,bool *forward)
{    
    static bool arrdisp;
    *forward = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        faceRight = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        faceRight = true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        walking = true;

    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (!(choice == 2 && faceRight == false))
            attack(choice, count, start);
        if ((choice == 2 && faceRight == false))
            movement(choice, count, start);
    }  
    else if (walking == false)
            idleanim(choice, count, start);
    else if (walking == true)
            movement(choice, count, start);     
    if (start->y <= 470)
    {
        start->y = 470;
         //Errorsound
    }
        
    if (start->y >= 1080)
    {
        //Errorsound
        start->y = 1080;
    }

    if (start->x <= 50)
    {
        //Errorsound
        start->x = 50;
    }

    if (start->x >= 1520)
    {
        //Errorsound
        start->x = 1520;
        *forward = true;
    }
        


    if (choice == 2 && faceRight == true)
    {
        sf::Texture arr;
        if ((count) % 10 == 0 && sf::Mouse::isButtonPressed(sf::Mouse::Left) )
            arrdisp = true;       
        arr.loadFromFile("Texture/05Characters/Archer/Attack/arrow.png");
        sf::RectangleShape arrSh(sf::Vector2f(300.0f,300.0f));
        arrSh.setTexture(&arr);
        arrSh.setOrigin(150, 280);
        arrSh.setPosition(*arrPos);
        if (arrdisp == false)
            *arrPos = *start;
        if (arrdisp == true )
        {
            arrPos->x += 70;
            if (arrSh.getPosition().x >= start->x + 400)
                arrdisp = false;           
            arrPos->y -= 2;
            m_window.draw(arrSh);
                   
        }    
    }
    
}

void character::idleanim(int choice,int count,sf::Vector2f *start)
{
    sf::Texture player;
    std::string meleeidle = "Texture/05Characters/Melee/Idle/"+ std::to_string((count/5) % 10)+ ".png";
    std::string archeridle = "Texture/05Characters/Archer/Idle/" + std::to_string((count/4) % 10) + ".png";
    std::string mageidle = "Texture/05Characters/Mage/Idle/" + std::to_string((count / 10) % 10) + ".png";
    if (choice == 1)
    {
        playerrect.setOrigin(150, 230);
        player.loadFromFile(meleeidle);

    }

    if (choice == 2)
    {

        playerrect.setOrigin(150, 280);
        player.loadFromFile(archeridle);
        
    }

    if (choice == 3)
    {
        playerrect.setOrigin(110, 300);
        player.loadFromFile(mageidle);

        sf::Texture aoe;
        aoe.loadFromFile("Texture/05Characters/Mage/Idle/aoe.png");
        sf::RectangleShape ModeTxt(sf::Vector2f(500.0f, 350.0f));
        ModeTxt.setTexture(&aoe);
        ModeTxt.setPosition(*start);
        ModeTxt.setFillColor(sf::Color(255, 255, 255, 100));
        ModeTxt.setOrigin(250, 195);
        m_window.draw(ModeTxt);
    }
     
    playerrect.setPosition(*start);
    playerrect.setSize(sf::Vector2f(300.0f, 300.0f));
    
    playerrect.setTexture(&player);

    if(faceRight == false)
        playerrect.setScale(-1, 1);
    
    m_window.draw(playerrect);
    
}

void character::movement(int choice, int count,sf::Vector2f *start)
{
    float speed;
    //player
    sf::Texture player;
    std::string meleemvmt = "Texture/05Characters/Melee/Movement/" + std::to_string((count ) % 15) + ".png";
    std::string archermvmt = "Texture/05Characters/Archer/Movement/" + std::to_string((count) % 10) + ".png";
    std::string magemvmt = "Texture/05Characters/Mage/Movement/" + std::to_string((count / 2) % 20) + ".png";

    //archerrange

    if (choice == 1)
    {
        player.loadFromFile(meleemvmt);
        playerrect.setOrigin(150, 230);
        speed = 4;
    }

    if (choice == 2)
    {
        player.loadFromFile(archermvmt);
        playerrect.setOrigin(150, 280);
        speed = 6;    
    }

    if (choice == 3)
    {
        player.loadFromFile(magemvmt);
        playerrect.setOrigin(110, 300);
        speed = 3.0f;

        sf::Texture aoe;
        aoe.loadFromFile("Texture/05Characters/Mage/Idle/aoe.png");
        sf::RectangleShape ModeTxt(sf::Vector2f(500.0f, 350.0f));
        ModeTxt.setTexture(&aoe);
        ModeTxt.setPosition(*start);
        ModeTxt.setFillColor(sf::Color(255, 255, 255, 100));
        ModeTxt.setOrigin(250, 195);
        m_window.draw(ModeTxt);
    }

    playerrect.setSize(sf::Vector2f(300.0f, 300.0f));
    
    playerrect.setTexture(&player);
    playerrect.setPosition(*start);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        playerrect.setScale(-1, 1);
        *start += sf::Vector2f(-4.0f * speed, 0.0f);
    }
        
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        
        *start += sf::Vector2f(4.0f* speed, 0.0f);
    }
        
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        *start += sf::Vector2f(0.0f, 2.0f * speed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        *start += sf::Vector2f(0.0f, -2.0f * speed);
        

    m_window.draw(playerrect);
}

void character::attack(int choice, int count, sf::Vector2f* start)
{
    float speed;
    sf::Texture player;
    std::string meleemvmt = "Texture/05Characters/Melee/Attack/" + std::to_string((count) % 15) + ".png";
    std::string archermvmt = "Texture/05Characters/Archer/Attack/" + std::to_string((count) % 10) + ".png";
    std::string magemvmt = "Texture/05Characters/Mage/Attack/" + std::to_string((count) % 20) + ".png";


    if (choice == 1)
    {
        speed = 4;

        playerrect.setOrigin(150, 230);
        player.loadFromFile(meleemvmt);
    }

    if (choice == 2)
    {
        speed = 2;
        playerrect.setOrigin(150, 280);
        player.loadFromFile(archermvmt);
        
    }

    if (choice == 3)
    {
        speed = 0.25f;
        playerrect.setOrigin(110, 300);
        player.loadFromFile(magemvmt);

        sf::Texture aoe;
        aoe.loadFromFile("Texture/05Characters/Mage/Idle/aoe.png");
        sf::RectangleShape ModeTxt(sf::Vector2f(500.0f, 350.0f));
        ModeTxt.setTexture(&aoe);
        ModeTxt.setPosition(*start);
        ModeTxt.setFillColor(sf::Color(255, 255, 255, 255));
        ModeTxt.setOrigin(250, 195);
        m_window.draw(ModeTxt);
        
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        playerrect.setScale(-1.0f, 1);
        *start += sf::Vector2f(-4.0f* speed, 0.0f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {

        *start += sf::Vector2f(4.0f*speed, 0.0f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        *start += sf::Vector2f(0.0f, 2.0f*speed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        *start += sf::Vector2f(0.0f, -2.0f*speed);
    playerrect.setPosition(*start);
    playerrect.setSize(sf::Vector2f(300.0f, 300.0f));
    playerrect.setTexture(&player);
        m_window.draw(playerrect);

       
}

