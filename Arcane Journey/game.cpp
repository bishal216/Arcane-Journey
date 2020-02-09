#include "game.h"
#include"character.h"
#include"map.h"
#include <SFML/Graphics.hpp>
#include<iostream>



void game::display(int choice,int count)
{
    character* CharObj = new character(m_window);
    map prev(m_window);
    map current(m_window);
    map next(m_window);
    if (forward == true)
    {
        mappos -= 51;
    }

    
    prev.load(mappos);
    current.load(1920+mappos);
    next.load(1920*2+mappos);
    CharObj->load(choice, count, &start,&arrPos,&forward);

    std::cout << forward << std::endl;
}