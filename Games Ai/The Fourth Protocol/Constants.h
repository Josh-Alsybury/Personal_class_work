#pragma once
#include <SFML/Graphics.hpp>

// Grid dimensions
constexpr int GRID_WIDTH = 5;
constexpr int GRID_HEIGHT = 5;

// Tile dimensions
constexpr float tileWidth = 800.0f / GRID_WIDTH;
constexpr float tileHeight = 600.0f / GRID_HEIGHT;

// Colors
const sf::Color ULTRAMARINE(20, 20, 80);

// Tile structure for rendering
struct Tile
{
	sf::RectangleShape shape;
};