#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "HumanPlayer.h"
#include "AIPlayer.h"
#include "Board.h"
#include "Constants.h"
#include "MainMenu.h"

enum class GamePhase { Placement, Movement };

class Game
{
public:
	Game();
	~Game();
	void run();

private:
	MainMenu m_mainMenu;  // add this
	bool m_showMenu{ true };  // track whether menu is visible
	int m_difficulty{ 0 };   // 0 = Easy, 1 = Medium, 2 = Hard	
	int m_aiDepth = 2;   // actual depth for AI

	// Core game loop methods
	void processEvents();
	void processKeys(const std::optional<sf::Event> t_event);
	void checkKeyboardState();
	void resetGame();
	void update(sf::Time t_deltaTime);
	void render();

	// Setup methods
	void setupGrid();

	// Game logic methods
	void checkTurn();
	void handleNpcTurn();

	bool gameOver;
	Owner winner;
	sf::Font gameFont;

	// Helper functions
	bool checkWinCondition(Owner player);
	void displayWinScreen(sf::RenderWindow& window);

	// Rendering helper
	void renderPiece(int x, int y, const Cell& cell);

	// Window and rendering
	sf::RenderWindow m_window;
	std::vector<std::vector<Tile>> m_grid;
	sf::Color highlightColor = sf::Color(200, 200, 50, 180); // yellow
	void highlightTile(int x, int y, sf::Color color);

	// Game state
	bool winnerAnnounced;
	Board m_board;
	HumanPlayer m_human;
	AIPlayer m_ai;
	Owner m_turn = Owner::Human;
	bool m_inPlacement = true;
	bool m_DELETEexitGame;
	GamePhase m_phase = GamePhase::Placement;
};