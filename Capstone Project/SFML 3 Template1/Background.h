#pragma once
#include <SFML/Graphics.hpp>

class Background
{
public:
    Background();
    void render(sf::RenderWindow& window);

private:
    sf::Texture BGtexture;
    sf::Sprite BGsprite{ BGtexture };
};
