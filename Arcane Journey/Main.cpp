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
    window.setFramerateLimit(100);

    int state = 4;
    int playerOption = 0;
    int count = 0;
    
    sf::Clock Clock;
    float deltaTime = 0;
    
    //Objects
    OpenAnim *OpenAnimObj = new OpenAnim(window,&state);
    OpenAnimObj->loadonce();

    LoginPage *LoginObj = new LoginPage(window,&state);
    LoginObj->loadonce();

    ModeSelect *ModSelObj = new ModeSelect(window, &state);
    ModSelObj->loadonce();

    Playerselect *playerObj = new Playerselect(window, &state,&playerOption);
    playerObj->loadonce();

    game gameobj(window);
    gameobj.loadonce();
   
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
        
        //Draw
        window.clear();
        if (state == 1)
            OpenAnimObj->loadintro(deltaTime);

        else if(state == 2)
            LoginObj->EnterDetails(deltaTime);
            
        else if (state == 3)
            ModSelObj->ModeOptions(deltaTime);


        else if (state == 4)
            playerObj->selected();

        else
            gameobj.display(playerOption,count,deltaTime);

        std::cout << 1 / deltaTime<<std::endl;
        window.display();
    }
    return 0;
}
