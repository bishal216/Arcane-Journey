#include "map.h"

map::~map()
{
}
void map::load()
{
    int randomsky = std::rand() % 2;
    int randomgrass = std::rand() % 2;

    //sky
    sf::Texture sky;
    if (randomsky == 0)
        sky.loadFromFile("Texture/04Maps/Jungle/sky/1.png");
    else
        sky.loadFromFile("Texture/04Maps/Jungle/sky/2.png");
    sf::Vector2u skysize = sky.getSize();
    sf::RectangleShape skytxt(sf::Vector2f(skysize.x, skysize.y));
    skytxt.setTexture(&sky);
    //wall
    sf::Texture wall;
    wall.loadFromFile("Texture/04Maps/Jungle/wall.png");
    sf::RectangleShape walltxt(sf::Vector2f(skysize.x, skysize.y));
    walltxt.setPosition(0, skysize.y);
    walltxt.setTexture(&wall);
    //grass
    sf::Texture grass;
    if (randomgrass == 0)
        grass.loadFromFile("Texture/04Maps/Jungle/grass/1.png");
    else
        grass.loadFromFile("Texture/04Maps/Jungle/grass/2.png");
    sf::RectangleShape grasstxt(sf::Vector2f(skysize.x, skysize.y * 3));
    grasstxt.setPosition(0, skysize.y * 2);
    grasstxt.setTexture(&grass);
    m_window.draw(skytxt);
    m_window.draw(walltxt);
    m_window.draw(grasstxt);
}
