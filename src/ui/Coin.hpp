#pragma once
#include <SFML/Graphics.hpp>

struct Coin {
    sf::RectangleShape shape;
    bool collectedThisRun = false; // survives death within a run

    Coin(float x, float y) {
        shape.setSize({14.f, 14.f});
        shape.setOrigin({7.f, 7.f});
        shape.setPosition({x, y});
        shape.setRotation(sf::degrees(45.f));
        shape.setFillColor(sf::Color(255, 210, 40));
        shape.setOutlineColor(sf::Color(200, 160, 0));
        shape.setOutlineThickness(1.5f);
    }
};