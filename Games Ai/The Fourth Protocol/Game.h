/**
 * @file Game.h
 * @brief Main game controller and state manager
 *
 * Orchestrates the entire game including menu, gameplay phases,
 * player turns, rendering, and win condition checking.
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "HumanPlayer.h"
#include "AIPlayer.h"
#include "Board.h"
#include "Constants.h"
#include "MainMenu.h"

 /**
  * @enum GamePhase
  * @brief Represents the current phase of gameplay
  */
enum class GamePhase {
	Placement, ///< Initial phase where players place pieces
	Movement   ///< Main phase where players move pieces
};

/**
 * @class Game
 * @brief Main game class managing all game systems
 *
 * Handles:
 * - Game loop (update/render)
 * - Event processing
 * - Turn management
 * - Phase transitions
 * - Win condition checking
 * - Menu system
 */
class Game
{
public:
	/**
	 * @brief Constructs the game and initializes all systems
	 */
	Game();

	/**
	 * @brief Destructor
	 */
	~Game();

	/**
	 * @brief Starts the main game loop
	 *
	 * Runs at 60 FPS with fixed timestep updates until window closes.
	 */
	void run();

private:
	MainMenu m_mainMenu;       ///< Main menu for difficulty selection
	bool m_showMenu{ true };   ///< Whether menu is currently visible
	int m_difficulty{ 0 };     ///< Difficulty level (0=Easy, 1=Medium, 2=Hard)
	int m_aiDepth = 2;         ///< Minimax search depth for AI

	/**
	 * @brief Processes all pending window events
	 */
	void processEvents();

	/**
	 * @brief Handles keyboard key press events
	 *
	 * @param t_event The keyboard event
	 */
	void processKeys(const std::optional<sf::Event> t_event);

	/**
	 * @brief Checks continuous keyboard state
	 */
	void checkKeyboardState();

	/**
	 * @brief Resets game to initial state for new match
	 */
	void resetGame();

	/**
	 * @brief Updates game state
	 *
	 * @param t_deltaTime Time elapsed since last update
	 */
	void update(sf::Time t_deltaTime);

	/**
	 * @brief Renders all game elements
	 */
	void render();

	/**
	* @brief Renders kegend for pieces
	*/
	void drawLegend();

	/**
	* @brief drawPlayerSection for pieces
	*/
	void drawPlayerSection(const std::string& playerName, float x, float y,
		const Player& player, const sf::Color& color);

	/**
	* @brief drawPieceInfo for player
	*/
	void drawPieceInfo(const std::string& name, int count, float x, float y,
		const sf::Color& color, PieceType type);

	/**
	 * @brief Initializes the game grid
	 */
	void setupGrid();

	/**
	 * @brief Manages turn logic and phase transitions
	 */
	void checkTurn();

	/**
	 * @brief Handles AI turn during placement phase
	 */
	void handleNpcTurn();

	bool gameOver;             ///< Whether game has ended
	Owner winner;              ///< Winner of the game
	sf::Font gameFont;         ///< Font for text rendering

	/**
	 * @brief Checks if specified player has won
	 *
	 * @param player Player to check
	 * @return true if player has 4+ in a row
	 * @return false otherwise
	 */
	bool checkWinCondition(Owner player);

	/**
	 * @brief Displays victory screen overlay
	 *
	 * @param window Reference to render window
	 */
	void displayWinScreen(sf::RenderWindow& window);

	/**
	 * @brief Renders a game piece
	 *
	 * @param x Grid x-coordinate
	 * @param y Grid y-coordinate
	 * @param cell Cell data containing piece info
	 */
	void renderPiece(int x, int y, const Cell& cell);

	sf::RenderWindow m_window;                  ///< Game window
	std::vector<std::vector<Tile>> m_grid;      ///< Visual grid tiles
	sf::Color highlightColor = sf::Color(200, 200, 50, 180); ///< Valid move highlight

	/**
	 * @brief Highlights a grid tile
	 *
	 * @param x Grid x-coordinate
	 * @param y Grid y-coordinate
	 * @param color Highlight color
	 */
	void highlightTile(int x, int y, sf::Color color);

	bool winnerAnnounced;      ///< Prevents duplicate win announcements
	Board m_board;             ///< Game board
	HumanPlayer m_human;       ///< Human player
	AIPlayer m_ai;             ///< AI opponent
	Owner m_turn = Owner::Human;           ///< Current turn
	bool m_inPlacement = true;             ///< Whether in placement phase
	bool m_DELETEexitGame;                 ///< Exit flag
	GamePhase m_phase = GamePhase::Placement; ///< Current game phase
};