//C++ libraries
#include<iostream>

//SFML
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>

//Classes
#include"SoundEff.h"

#include"OpenAnim.h"
#include"LoginPage.h"
#include"ModeSelect.h"
#include"Playerselect.h"
#include"game.h"

int main()
{
    //Variables 
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "!ARCANE!", sf::Style::Fullscreen | sf::Style::Default);
    int state = 4;
    int playerOption = 0;
    int count = 0;
    sf::Vector2f spawnlocation = sf::Vector2f(500.0f, 540.0f);
    sf::Vector2f arrowlocation = spawnlocation;
    sf::Clock Clock;
    float deltaTime = 0;
    
    //Objects
    OpenAnim *OpenAnimObj = new OpenAnim(window,&state);
    LoginPage *LoginObj = new LoginPage(window,&state);
    ModeSelect *ModSelObj = new ModeSelect(window, &state);
    Playerselect *playerObj = new Playerselect(window, &state,&playerOption);
    game gameobj(window,&spawnlocation,&arrowlocation);
   
      //Window
        while (window.isOpen())
        {
            

            //framerate
            deltaTime = Clock.restart().asSeconds();

            //count
                count++;

        sf::Event event;
        sf::Vector2f arrowlocation = spawnlocation;
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
        
        //Draw
        window.clear();
        /*if (state == 1)
            OpenAnimObj->loadintro(deltaTime);

        else if(state == 2)
        {
            LoginObj->EnterDetails();
        }
            
        else if (state == 3)
            ModSelObj->ModeOptions();


        else */if (state == 4)
            playerObj->selected();

        else
            gameobj.display(playerOption,count);
        std::cout << 1/deltaTime << std::endl;
        //std::cout << state << std::endl;
        //std::cout << playerOption << std::endl;
        window.display();
    }
    return 0;
}
