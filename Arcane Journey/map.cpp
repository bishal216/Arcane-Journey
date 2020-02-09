#include "map.h"
#include<iostream>
map::~map()
{
}
void map::load(int mappos)
{
    int random = std::rand() % 6+1;
    std::string mapno = "Texture/04Maps/Jungle/" +std::to_string(random)+ ".png";
    //map
    sf::Texture sky;
    sky.loadFromFile(mapno);
    sf::Vector2u skysize = sky.getSize();
    sf::RectangleShape skytxt(sf::Vector2f(skysize.x, skysize.y));
    skytxt.setTexture(&sky);
    skytxt.setPosition(sf::Vector2f(mappos, 0));
    m_window.draw(skytxt);
}
