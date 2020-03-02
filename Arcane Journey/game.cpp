#include "game.h"
#include<iostream>



void game::loadonce()
{
    //Maps
    prev->loadonce();
    next->loadonce();
    current->loadonce();
    board->loadTexture();
    swordCross.loadFromFile("Textures/progress.png");
    prevpos = -1920;
    currpos = 0;
    nextpos = 1920;
    font.loadFromFile("Fonts/Enchanted.otf");
    //Character
    for (int i = 0; i < 3; i++)
        player[i].loadonce();
}

void game::display(int *choice,int count,float deltaTime)
{
   //For Map
    m_window.setMouseCursorVisible(true);
    if (prevpos <= -1920)
    {
        prevpos += 3 * 1920;
        prand = std::rand() % 6;
    }
        
    if (currpos <= -1920)
    {
        crand = std::rand() % 6;
        currpos += 3 * 1920;
    }
        
    if (nextpos <= -1920)
    {
        nextpos += 3 * 1920;
        nrand = std::rand() % 6;
    }
    if (forward == true)
    {
        //prev-curr-next
        if (prevpos < currpos && currpos < nextpos)
        {
            prevpos -= 400*deltaTime;
            currpos = prevpos + 1920;
            nextpos = currpos +1920;
        }
        //curr-next-prev
        if (currpos < nextpos && nextpos < prevpos)
        {
            currpos -= 400 * deltaTime;
            nextpos = currpos + 1920;
            prevpos = nextpos+1920;   
        }
        //next-prev-curr
        if (nextpos < prevpos && prevpos < currpos)
        {
            nextpos -= 400 * deltaTime;
            prevpos = nextpos + 1920;
            currpos = prevpos + 1920;
        }
    } 
    prev->load(prevpos,prand);
    current->load(currpos,crand);
    next->load(nextpos,nrand);

    //For Player
    for (int i = 0; i < *choice; i++)
    {
       playerAlive[i] = true;
       playerAlive[i]= player[i].loadCharacters(i,count ,&f[i],deltaTime);
       if (playerAlive[i] == false)
       {
           f[i] = true;
           playerDead(i);
       }
       
    }
    std::cout << f[0] << "/" << f[1] << "/" << f[2] << std::endl;
    if (f[0] == true&&f[1]==true&&f[2]==true)
        forward = true;
    else
        forward = false;

    if (forward == true)
        fill++;
    Progress();
    if (playerAlive[0]==false&& playerAlive[1] == false && playerAlive[2] == false)
    {
        sf::RectangleShape gameover(sf::Vector2f(1920, 1080));
        gameover.setFillColor(sf::Color(0, 0, 0, 255));
        m_window.draw(gameover);
        std::string str = "----Game  Over----\nPress Escape to Exit";
        sf::Text txt(str, font, 130);
        txt.setPosition(sf::Vector2f(450, 450));
        m_window.draw(txt);   
        board->loadBoard();
    }
}

void game::playerDead(int ch)
{
    std::string str = "Player"+std::to_string(ch+1)+" has been killed";
    sf::Text txt(str,font,30);
    txt.setFillColor(sf::Color(255,0,0,255));
    txt.setPosition(sf::Vector2f(150,100*ch+50));
    m_window.draw(txt);
}

void game::Progress()
{
    //OBJECTIVE
    sf::Text objective("REACH THE CASTLE", font, 40);
    objective.setFillColor(sf::Color(0, 0, 0, 255));
    objective.setPosition(775, 75);
    m_window.draw(objective);

    //TotalDestination
    sf::RectangleShape destination(sf::Vector2f(720, 15));
    destination.setFillColor(sf::Color(255, 255, 255, 200));
    destination.setPosition(600, 125);
    m_window.draw(destination);


    prog.setTexture(&swordCross);
    prog.setPosition(sf::Vector2f(600+(float)(fill/720),135));
    prog.setSize(sf::Vector2f(50,50));
    prog.setOrigin(sf::Vector2f(25,25));
   
    //Progress
    sf::RectangleShape progress(sf::Vector2f((float)(fill/720), 15));

    if (fill > .66)
        progress.setFillColor(sf::Color(0, 255, 0, 200));
    else if (fill < .33)
        progress.setFillColor(sf::Color(255, 0, 0, 200));
    else
        progress.setFillColor(sf::Color(255, 255, 0, 200));
    progress.setPosition(600,125);
    m_window.draw(progress);
    m_window.draw(prog);
}