#include "MainMenu.h"
#include <iostream> // optional, for error messages

MainMenu::MainMenu(float width, float height)
{
    if (!m_font.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
    {
        std::cerr << "Failed to load font!" << std::endl;
    }

    options.clear();
    options.reserve(4);

    std::string names[4] = { "Easy", "Medium", "Hard", "Exit" };

    for (int i = 0; i < 4; ++i)
    {
        options.emplace_back(m_font);        // now references member font
        options[i].setString(names[i]);
        options[i].setCharacterSize(40);
        options[i].setFillColor(sf::Color::White);
        options[i].setPosition(sf::Vector2f(width / 2.f - 60.f, 150.f + i * 80.f));
    }
}

void MainMenu::draw(sf::RenderWindow& window)
{
    for (auto& t : options)
        window.draw(t);
}

int MainMenu::handleClick(sf::Vector2f mousePos)
{
    for (size_t i = 0; i < options.size(); ++i)
    {
        if (options[i].getGlobalBounds().contains(mousePos))
            return static_cast<int>(i); // 0 = Easy, 1 = Medium, 2 = Hard, 3 = Exit
    }
    return -1;
}
