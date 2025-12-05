/**
 * @file MainMenu.cpp
 * @brief Main menu implementation for difficulty selection
 *
 * Provides a simple menu interface for selecting game difficulty
 * (Easy, Medium, Hard) or exiting the game.
 */

#include "MainMenu.h"
#include <iostream>

 /**
  * @brief Constructs the main menu
  *
  * @param width Window width in pixels
  * @param height Window height in pixels
  *
  * Loads the font and creates four menu options (Easy, Medium, Hard, Exit)
  * positioned vertically in the center of the window.
  */
MainMenu::MainMenu(float width, float height)
{
    if (!m_font.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
    {
        std::cerr << "Failed to load font!" << std::endl;
    }

    options.clear();
    options.reserve(5);
    std::string names[5] = { "Easy", "Medium", "Hard", "Nightmare", "Exit" };

    for (int i = 0; i < 5; ++i)
    {
        options.emplace_back(m_font);
        options[i].setString(names[i]);
        options[i].setCharacterSize(40);
        options[i].setFillColor(sf::Color::White);
        options[i].setPosition(sf::Vector2f(width / 2.f - 60.f, 150.f + i * 80.f));
    }
}

/**
 * @brief Draws all menu options to the window
 *
 * @param window Reference to the render window
 */
void MainMenu::draw(sf::RenderWindow& window)
{
    for (auto& t : options)
        window.draw(t);
}

/**
 * @brief Handles mouse click on menu options
 *
 * @param mousePos Mouse position in world coordinates
 * @return int Menu choice index (0=Easy, 1=Medium, 2=Hard, 3=Exit), or -1 if no option clicked
 *
 * Checks if the mouse position intersects with any menu option's bounding box.
 */
int MainMenu::handleClick(sf::Vector2f mousePos)
{
    for (size_t i = 0; i < options.size(); ++i)
    {
        if (options[i].getGlobalBounds().contains(mousePos))
            return static_cast<int>(i); // 0 = Easy, 1 = Medium, 2 = Hard, 3 = Exit
    }
    return -1;
}