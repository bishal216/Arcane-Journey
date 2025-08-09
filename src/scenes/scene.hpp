#pragma once

#include <SFML/Graphics.hpp>

class Scene{
public:
    virtual ~Scene() {}

    virtual void handleEvent(sf::Event& event) = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;
};