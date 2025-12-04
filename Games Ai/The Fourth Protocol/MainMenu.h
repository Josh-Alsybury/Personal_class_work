/**
 * @file MainMenu.h
 * @brief Main menu interface for game setup
 *
 * Provides a simple menu for selecting difficulty level
 * or exiting the game.
 */

#pragma once
#include <SFML/Graphics.hpp>

 /**
  * @class MainMenu
  * @brief Menu system for difficulty selection
  *
  * Displays four options (Easy, Medium, Hard, Exit) and
  * handles user selection via mouse clicks.
  */
class MainMenu
{
public:
	/**
	 * @brief Constructs the main menu
	 *
	 * @param width Window width in pixels
	 * @param height Window height in pixels
	 */
	MainMenu(float width, float height);

	/**
	 * @brief Draws all menu options
	 *
	 * @param window Reference to render window
	 */
	void draw(sf::RenderWindow& window);

	/**
	 * @brief Handles mouse click on menu options
	 *
	 * @param mousePos Mouse position in world coordinates
	 * @return int Menu choice (0=Easy, 1=Medium, 2=Hard, 3=Exit), or -1
	 */
	int handleClick(sf::Vector2f mousePos);

private:
	sf::Font m_font;                  ///< Font for menu text
	std::vector<sf::Text> options;    ///< Menu option texts
};
