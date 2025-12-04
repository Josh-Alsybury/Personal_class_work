/**
 * @file Constants.h
 * @brief Global constants and configuration
 *
 * Defines grid dimensions, tile sizes, colors, and rendering structures
 * used throughout the game.
 */

#pragma once
#include <SFML/Graphics.hpp>

constexpr int GRID_WIDTH = 5;   ///< Number of columns in game grid
constexpr int GRID_HEIGHT = 5;  ///< Number of rows in game grid

constexpr float tileWidth = 800.0f / GRID_WIDTH;   ///< Width of each tile in pixels
constexpr float tileHeight = 600.0f / GRID_HEIGHT; ///< Height of each tile in pixels

const sf::Color ULTRAMARINE(20, 20, 80); ///< Background color

/**
 * @struct Tile
 * @brief Visual representation of a grid tile
 */
struct Tile
{
	sf::RectangleShape shape; ///< SFML shape for rendering
};
