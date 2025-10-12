#include "Background.h"
#include <stdexcept>

Background::Background()
{
    if (!BGtexture.loadFromFile("ASSETS/IMAGES/Autumn Forest 2D Pixel Art/Background/2.png"))
    {
        throw std::runtime_error("Failed to load background texture!");
    }

    BGsprite.setTexture(BGtexture, true);
    sf::Vector2u texSize = BGtexture.getSize();
    float scaleX = 1000.f / texSize.x;
    float scaleY = 800.f / texSize.y;
    BGsprite.setScale(sf::Vector2f(scaleX, scaleY));
    BGsprite.setPosition(sf::Vector2f(0.f, 0.f));
}

void Background::render(sf::RenderWindow& window)
{
    window.draw(BGsprite);
}
