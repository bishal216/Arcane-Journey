#include "character.h"
#include<iostream>
#include<SFML/Graphics.hpp>
character::~character()
{
}
void character::load(int choice,int count,sf::Vector2f *start)
{    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        faceRight = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        faceRight = true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        walking = true;

    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        attack(choice, count,start);
    else if (walking == false)
            idleanim(choice, count, start);
    else if (walking == true)
            movement(choice, count, start);     
    if (start->y <= 540)
        start->y = 540;
    if (start->x <= 50)
        start->x = 50;
        
}

void character::idleanim(int choice,int count,sf::Vector2f *start)
{
    sf::Texture player;
    std::string meleeidle = "Texture/05Characters/Melee/Idle/"+ std::to_string((count/5) % 10)+ ".png";
    std::string archeridle = "Texture/05Characters/Archer/Idle/" + std::to_string((count/4) % 10) + ".png";
    std::string mageidle = "Texture/05Characters/Mage/Idle/" + std::to_string((count / 10) % 10) + ".png";
    if (choice == 1)
    {
        playerrect.setOrigin(150, 300);
        player.loadFromFile(meleeidle);

        sf::RectangleShape rsh(sf::Vector2f(150, 2));
        rsh.setPosition(playerrect.getPosition());
        rsh.setFillColor(sf::Color(255, 255, 255, 255));
        m_window.draw(rsh);

    }

    if (choice == 2)
    {

        playerrect.setOrigin(150, 280);
        player.loadFromFile(archeridle);
        
    }

    if (choice == 3)
    {
        playerrect.setOrigin(150, 300);
        player.loadFromFile(mageidle);
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
        playerrect.setOrigin(150, 300);
        speed = 4;
    }

    if (choice == 2)
    {
        player.loadFromFile(archermvmt);
        playerrect.setOrigin(150, 280);
        speed = 5;    
    }

    if (choice == 3)
    {
        player.loadFromFile(magemvmt);
        playerrect.setOrigin(100, 300);
        speed = 1.0f;
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

        playerrect.setOrigin(150, 300);
        player.loadFromFile(meleemvmt);
    }

    if (choice == 2)
    {
        speed = 3;
        playerrect.setOrigin(150, 280);
        player.loadFromFile(archermvmt);

        
    }

    if (choice == 3)
    {
        speed = 0.25f;
        playerrect.setOrigin(150, 300);
        player.loadFromFile(magemvmt);
        
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

