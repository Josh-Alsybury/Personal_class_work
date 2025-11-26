#pragma once
#include <SFML/Graphics.hpp>

class MainMenu
{
public:
    MainMenu(float width, float height);
    void draw(sf::RenderWindow& window);
    int handleClick(sf::Vector2f mousePos);

private:
    sf::Font  m_font;
    std::vector<sf::Text> options;
};
