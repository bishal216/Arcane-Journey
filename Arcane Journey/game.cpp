#include "game.h"
#include"character.h"
#include"map.h"
#include <SFML/Graphics.hpp>
void game::display(int choice,int count)
{
    character* CharObj = new character(m_window);
    map* mapObj = new map(m_window);
    



    
    
    mapObj->load();
    CharObj->load(choice, count, &start);
}