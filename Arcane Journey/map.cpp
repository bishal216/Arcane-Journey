#include "map.h"
#include<iostream>
map::~map()
{
}
void map::loadonce()
{
    for(int i=0;i<6;i++)
    junglemap[i].loadFromFile("Texture/04Maps/Jungle/" + std::to_string(i) + ".png");
}


void map::load(int mappos,int random)
{
    
    //map
    sf::RectangleShape skytxt(sf::Vector2f(1920, 1080));
    skytxt.setTexture(&junglemap[random]);
    skytxt.setPosition(sf::Vector2f(mappos, 0));

    m_window.draw(skytxt);
}
