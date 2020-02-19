#include "game.h"
#include"map.h"
#include<iostream>



void game::loadonce()
{
    CharObj->loadonce();
    prev->loadonce();
    next->loadonce();
    current->loadonce();
    Stat->loadonce();

    prevpos = -1920;
    currpos = 0;
    nextpos = 1920;
}

void game::display(int choice,int count,float et)
{
   

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
        
    //For Map
    if (forward == true)
    {
        //prev-curr-next
        if (prevpos < currpos && currpos < nextpos)
        {
            prevpos -= speed;
            currpos = prevpos + 1920;
            nextpos = currpos +1920;
        }
        //curr-next-prev
        if (currpos < nextpos && nextpos < prevpos)
        {
            prevpos = nextpos+1920;
            currpos -= speed;
            nextpos = currpos + 1920;
        }
        //next-prev-curr
        if (nextpos < prevpos && prevpos < currpos)
        {
            nextpos -= speed;
            prevpos = nextpos + 1920;
            currpos = prevpos + 1920;
        }
        
    }

    
    
    prev->load(prevpos,prand);
    current->load(currpos,crand);
    next->load(nextpos,nrand);

    Stat->showstats(choice);
    CharObj->load(choice, count, &start,&arrPos,&forward,et,&speed);
}