#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "HumanPlayer.h"
#include "AIPlayer.h"
#include "Board.h"
#include "Constants.h"

enum class GamePhase { Placement, Movement };

class Game
{
public:
	Game();
	~Game();
	void run();

private:
	// Core game loop methods
	void processEvents();
	void processKeys(const std::optional<sf::Event> t_event);
	void checkKeyboardState();
	void update(sf::Time t_deltaTime);
	void render();

	// Setup methods
	void setupGrid();

	// Game logic methods
	void checkTurn();
	void handleNpcMovement();
	void handleNpcTurn();

	// Rendering helper
	void renderPiece(int x, int y, const Cell& cell);

	// Window and rendering
	sf::RenderWindow m_window;
	std::vector<std::vector<Tile>> m_grid;
	sf::Color highlightColor = sf::Color(200, 200, 50, 180); // yellow
	void highlightTile(int x, int y, sf::Color color);

	// Game state
	Board m_board;
	HumanPlayer m_human;
	AIPlayer m_ai;
	Owner m_turn = Owner::Human;
	bool m_inPlacement = true;
	bool m_DELETEexitGame;
	GamePhase m_phase = GamePhase::Placement;
};