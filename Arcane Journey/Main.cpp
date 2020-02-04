//C++ libraries
#include<iostream>

//SFML
#include <SFML/Graphics.hpp>
//#include<SFML/Audio.hpp>

//Classes
#include"SoundEff.h"

#include"OpenAnim.h"
#include"LoginPage.h"
#include"ModeSelect.h"
#include"Playerselect.h"
#include"game.h"
#include"character.h"
//Global Variables

//float framerate;
int main()
{
    //Variables 
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "!ARCANE!", sf::Style::Fullscreen | sf::Style::Default);
    int playerOption;
    int count = 0;
    sf::Vector2f spawnlocation = sf::Vector2f(150.0f, 540.0f);

    sf::Clock Clock;
    float deltaTime;
    
    //Objects
    OpenAnim *OpenAnimObj = new OpenAnim(window);
    LoginPage *LoginObj = new LoginPage(window);
    ModeSelect *ModSelObj = new ModeSelect(window);
    Playerselect *playerObj = new Playerselect(window);
    game gameobj(window,&spawnlocation);
    //charObj.load();
    //For Sound Effects
        //SoundEff OpenSound;
        //OpenSound.Openintro(OpenAnimObj.alpha==0 , OpenAnimObj.xoff==0);
    


      //Window
        while (window.isOpen())
        {
            //framerate
            deltaTime = Clock.restart().asSeconds();
            std::cout << "Framerate:" << 1/deltaTime << std::endl;
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
        /*if (OpenAnimObj->opendisp == true)
            OpenAnimObj->loadintro(deltaTime);

        else if (LoginObj->display == true)
            LoginObj->EnterDetails();


        else if (ModSelObj->display == true)
            ModSelObj->ModeOptions();


        else*/ if (playerObj->display == true)
            playerObj->selected();

        else
            gameobj.display(playerObj->x,count);

        window.display();
    }
    return 0;
}
