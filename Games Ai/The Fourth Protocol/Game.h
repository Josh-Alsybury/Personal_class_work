/// <summary>
/// author Pete Lowe May 2025
/// you need to change the above line or lose marks
/// Also don't have any member properties called Delete...
/// </summary>
#ifndef GAME_HPP
#define GAME_HPP
#pragma warning( push )
#pragma warning( disable : 4275 )
// ignore this warning
// C:\SFML - 3.0.0\include\SFML\System\Exception.hpp(41, 47) : 
// warning C4275 : non dll - interface class 'std::runtime_error' used as base for dll - interface class 'sf::Exception'

/// <summary>
/// include guards used so we don't process this file twice
/// same as #pragma once
/// Don't forget the #endif at the bottom
/// </summary>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Piece.h"
#include "Player.h"

const sf::Color ULTRAMARINE{ 5, 55,242,255 }; // const colour

const int GRID_WIDTH= 5;
const int GRID_HEIGHT = 5;
const int TILE_SIZE = 100;

struct Tile
{
	sf::RectangleShape shape;
	sf::Vector2f flowVector{ 0.f, 0.f };

	bool isOnPath = false;
};


class Game
{
public:
	Game();
	~Game();
	void run();


private:

	void processEvents();
	void processKeys(const std::optional<sf::Event> t_event);
	void checkKeyboardState();
	void update(sf::Time t_deltaTime);
	void render();

	void setupGrid();
	
	void setupTexts();
	void setupSprites();
	void setupAudio();

	float tileWidth = static_cast<float>(800) / GRID_WIDTH; // cell bounds
	float tileHeight = static_cast<float>(600) / GRID_HEIGHT; // cell bpunds

	std::vector<std::vector<Tile>> m_grid;
	std::array<std::array<Cell, GRID_WIDTH>, GRID_HEIGHT> m_board{}; // whats occupying an actual cell of the grid

	// players
	Player m_human;   // green
	Player m_ai;      // red 

	// turn and phase
	Owner m_turn{ Owner::Human };
	bool m_inPlacement{ true };

	// helpers
	bool placeAt(int gridx, int gridy, PieceType piecet, Owner who);
	void handleHumanClick(int pixelx, int pixely);

	//npc turn
	void handleNpcTurn();

	
	sf::RenderWindow m_window; // main SFML window
	sf::Font m_jerseyFont;// font used by message
	bool m_DELETEexitGame; // control exiting game

};

#pragma warning( pop ) 
#endif // !GAME_HPP

