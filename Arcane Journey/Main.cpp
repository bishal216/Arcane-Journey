//C++ libraries
#include<iostream>

//SFML
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>

//Classes
#include"SoundEff.h"
//Opening Animation
#include"OpenAnim.h"
//Select Players
#include"LoginPage.h"
#include"ModeSelect.h"
#include"Playerselect.h"
#include"game.h"

int main()
{
    //Variables 
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "!ARCANE!", sf::Style::Fullscreen | sf::Style::Default);
    window.setFramerateLimit(200);
    
    sf::Clock Clock;
    float deltaTime = 0;
    int state = 1;
    int playerOption = 0;
    //Objects
    Animation *OpenAnimObj = new Animation(window,&state);
    OpenAnimObj->loadTextures();

    Playerselect* playerObj = new Playerselect(window, &state, &playerOption);
    playerObj->loadonce();

    LoginPage *LoginObj = new LoginPage(window,&state);
    LoginObj->loadonce();
restart:
    game* gameobj;
    gameobj = new game(window);
    gameobj->loadonce();

    
    
    int count = 0;

    int retry = 0;
      //Window
        while (window.isOpen())
        {
            //framerate
            deltaTime = Clock.restart().asSeconds();

            //count
                count++;

        sf::Event event;
        
        window.setKeyRepeatEnabled(false);
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();
        
        //Draw
        window.clear();
        if (state == 1)
            OpenAnimObj->showAnimation(deltaTime);

        //else if(state == 2)
            //LoginObj->EnterDetails(deltaTime);

        else if (state == 2)
            playerObj->selected();

        else
            gameobj->display(&playerOption,count,deltaTime);      
        window.display();
    }
    return 0;
}
