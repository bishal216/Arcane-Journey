#include "character.h"
#include"SoundEff.h"

void character::loadonce()
{
    tombstone[0].loadFromFile("Texture/05Characters/Melee/dying.png");
    tombstone[1].loadFromFile("Texture/05Characters/Archer/dying.png");
    tombstone[2].loadFromFile("Texture/05Characters/Mage/dying.png");
    int i;
    for (i = 0; i < 10; i++)
    {
        meleeidle[i].loadFromFile("Texture/05Characters/Melee/Idle/" + std::to_string(i) + ".png");
        rangeidle[i].loadFromFile("Texture/05Characters/Archer/Idle/" + std::to_string(i) + ".png");
        mageidle[i].loadFromFile("Texture/05Characters/Mage/Idle/" + std::to_string(i) + ".png");

        meleeatt[i].loadFromFile("Texture/05Characters/Melee/Attack/" + std::to_string(i) + ".png");
        rangeatt[i].loadFromFile("Texture/05Characters/Archer/Attack/" + std::to_string(i) + ".png");
        mageatt[i].loadFromFile("Texture/05Characters/Mage/Attack/" + std::to_string(i) + ".png");

        meleewalk[i].loadFromFile("Texture/05Characters/Melee/Movement/" + std::to_string(i) + ".png");
        rangewalk[i].loadFromFile("Texture/05Characters/Archer/Movement/" + std::to_string(i) + ".png");
        magewalk[i].loadFromFile("Texture/05Characters/Mage/Movement/" + std::to_string(i) + ".png");
    }
         

        arrow.loadFromFile("Texture/05Characters/Archer/Attack/arrow.png");
        aoe.loadFromFile("Texture/05Characters/Mage/Idle/aoe.png");
}

void character::load(int choice,int count,sf::Vector2f *start, sf::Vector2f* arrPos,bool *forward,float et,float *speed)
{    
    
    faceRight = true;
    walking = false;
    static bool arrdisp;
    *forward = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) == true && death == false)
        faceRight = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D) == true && death == false)
        walking = true;

    playerrect.setScale(1, 1);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) == true && death==false)
    {
        if (!(choice == 2 && faceRight == false))
            attack(choice, count, start,et,speed);
        if ((choice == 2 && faceRight == false))
            movement(choice, count, start,et,speed);
    }  
    else if (walking == false && death == false)
            idleanim(choice, count, start,et, speed);
    else if (walking == true && death == false)
            movement(choice, count, start,et,speed);     
    else if (death == true)
    {
        playerrect.setOrigin(0, 300);
        playerrect.setPosition(*start);
        playerrect.setScale(1, 1);
        if (choice == 1)
        {
            tombstone[0].setSmooth(true);
            tombstone[0].setRepeated(false);
            playerrect.setTexture(&tombstone[0]);
        }

        if (choice == 2)
        {
            sf::Vector2u size = tombstone[0].getSize();
            playerrect.setSize(sf::Vector2f(size.x, size.y));
            playerrect.setTexture(&tombstone[1]);
        }

        if (choice == 3)
        {
            sf::Vector2u size = tombstone[0].getSize();
            playerrect.setSize(sf::Vector2f(size.x, size.y));
            playerrect.setTexture(&tombstone[2]);
        }
        playerrect.setSize(sf::Vector2f(300.0f, 300.0f));

        m_window.draw(playerrect);
    }
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
    if (choice == 3 && start->y <= 600)
    {
        start->y = 600;
    }

    if (start->x >= 1520)
    {
        //Errorsound
        start->x = 1520;
        if(walking==true&&faceRight==true&&death==false)
            *forward = true;
    }
        


    if (choice == 2 && faceRight == true&&death == false)
    {
        if ((count/10) % 10 == 0 && sf::Mouse::isButtonPressed(sf::Mouse::Left) )
            arrdisp = true;       
        
        sf::RectangleShape arrSh(sf::Vector2f(300.0f,300.0f));
        arrSh.setTexture(&arrow);
        arrSh.setOrigin(150, 280);
        arrSh.setPosition(*arrPos);
        if (arrdisp == false)
            *arrPos = *start;
        if (arrdisp == true )
        {
            arrPos->x += 700*et;
            if (arrSh.getPosition().x >= start->x + 400)
                arrdisp = false;           
            arrPos->y -= 30*et;
            m_window.draw(arrSh);          
        }    
    }
    if (count % 1000 == 0)
        death = true;
    
}

void character::idleanim(int choice,int count,sf::Vector2f *start,float et, float* speed)
{
    //Animation speed<increase value for slower transition>
    int imgrate = 10;


    if (choice == 1)
    {
        playerrect.setOrigin(150, 230);
        playerrect.setTexture(&meleeidle[(count/imgrate)%10]);
    }

    if (choice == 2)
    {
        playerrect.setOrigin(150, 280);
        playerrect.setTexture(&rangeidle[(count/ imgrate) % 10]);
        
    }

    if (choice == 3)
    {
        playerrect.setOrigin(110, 300);
        playerrect.setTexture(&mageidle[(count/ imgrate) % 10]);
       
        sf::RectangleShape ModeTxt(sf::Vector2f(500.0f, 350.0f));
        ModeTxt.setTexture(&aoe);
        ModeTxt.setPosition(*start);
        ModeTxt.setFillColor(sf::Color(255, 255, 255, 100));
        ModeTxt.setOrigin(250, 195);
        m_window.draw(ModeTxt);
    }
     
    playerrect.setPosition(*start);
    playerrect.setSize(sf::Vector2f(300.0f, 300.0f));

    m_window.draw(playerrect);
    
}

void character::movement(int choice, int count,sf::Vector2f *start,float et, float* speed)
{
    //Animation speed<increase value for slower transition>
    int imgrate = 10;
    //archerrange

    if (choice == 1)
    {
        playerrect.setTexture(&meleewalk[(count/imgrate)%10]);
        playerrect.setOrigin(150, 230);
        *speed = 400*et;
    }

    if (choice == 2)
    {
        playerrect.setTexture(&rangewalk[(count / imgrate) % 10]);
        playerrect.setOrigin(150, 280);
        *speed = 600*et;    
    }

    if (choice == 3)
    {
        playerrect.setTexture(&magewalk[(count / imgrate) % 10]);
        playerrect.setOrigin(110, 300);
        *speed = 300*et;

        sf::RectangleShape ModeTxt(sf::Vector2f(500.0f, 350.0f));
        ModeTxt.setTexture(&aoe);
        ModeTxt.setPosition(*start);
        ModeTxt.setFillColor(sf::Color(255, 255, 255, 100));
        ModeTxt.setOrigin(250, 195);
        m_window.draw(ModeTxt);
    }

    playerrect.setSize(sf::Vector2f(300.0f, 300.0f));
    
    
    playerrect.setPosition(*start);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        playerrect.setScale(-1, 1);
        *start += sf::Vector2f(-*speed, 0.0f);
    }
        
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) 
        *start += sf::Vector2f( *speed, 0.0f);
        
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        *start += sf::Vector2f(0.0f, *speed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        *start += sf::Vector2f(0.0f, -*speed);
    m_window.draw(playerrect);
}

void character::attack(int choice, int count, sf::Vector2f* start,float et, float* speed)
{
    //Animation speed<increase value for slower transition>
    int imgrate = 10;
 
    if (choice == 1)
    {
        *speed = 300*et;
        playerrect.setTexture(&meleeatt[(count / imgrate)%10]);
        playerrect.setOrigin(150, 230);
    }

    if (choice == 2)
    {
        *speed = 50*et;
        playerrect.setTexture(&rangeatt[(count / imgrate)  % 10]);
        playerrect.setOrigin(150, 280);
        
    }

    if (choice == 3)
    {
        *speed = 25*et;
        playerrect.setTexture(&mageatt[((count / imgrate)) % 10]);
        playerrect.setOrigin(110, 300);


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
        *start += sf::Vector2f(-*speed, 0.0f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {

        *start += sf::Vector2f(*speed, 0.0f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        *start += sf::Vector2f(0.0f, *speed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        *start += sf::Vector2f(0.0f, -*speed);
    playerrect.setPosition(*start);
    playerrect.setSize(sf::Vector2f(300.0f, 300.0f));
    
        m_window.draw(playerrect);

       
}


