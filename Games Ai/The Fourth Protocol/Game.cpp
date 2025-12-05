/**
 * @file Game.cpp
 * @author [Your Name Here]
 * @date May 2025
 * @brief Main game implementation with refactored structure
 *
 * This file contains the core game loop, event handling, rendering, and game state management
 * for a strategy board game featuring human vs AI gameplay with difficulty settings.
 */

#include "Game.h"
#include <iostream>

 // ============================================================================
 // CONSTRUCTOR & DESTRUCTOR
 // ============================================================================

 /**
  * @brief Constructs the Game object and initializes all subsystems
  *
  * Sets up the game window, loads fonts, initializes the main menu,
  * and prepares the game grid for play.
  */
Game::Game() :
	m_window{ sf::VideoMode{ sf::Vector2u{1000U, 600U}, 32U }, "SFML Game 3.0" }, // Made wider for legend
	m_DELETEexitGame{ false },
	m_mainMenu(800.f, 600.f)
{
	gameOver = false;
	winner = Owner::None;
	winnerAnnounced = false;

	if (!gameFont.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
	{
		std::cerr << "Error loading font!\n";
	}

	setupGrid();
}

/**
 * @brief Destructor for Game class
 */
Game::~Game()
{
}

// ============================================================================
// MAIN GAME LOOP
// ============================================================================

/**
 * @brief Main game loop implementing fixed timestep updates
 *
 * Runs at 60 FPS with consistent update timing. Handles events,
 * updates game state, and renders each frame until the window closes.
 */
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps);

	while (m_window.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();

		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}

		render();
	}
}

// ============================================================================
// EVENT HANDLING
// ============================================================================

/**
 * @brief Processes all pending window events
 *
 * Handles window close events, menu interactions, keyboard input,
 * and mouse clicks for both placement and movement phases.
 */
void Game::processEvents()
{
	while (const std::optional newEvent = m_window.pollEvent())
	{
		if (newEvent->is<sf::Event::Closed>())
		{
			m_DELETEexitGame = true;
		}

		if (m_showMenu)
		{
			if (newEvent->is<sf::Event::MouseButtonPressed>())
			{
				const auto* mb = newEvent->getIf<sf::Event::MouseButtonPressed>();
				if (mb->button == sf::Mouse::Button::Left)
				{
					sf::Vector2f mousePos = m_window.mapPixelToCoords(mb->position);
					int choice = m_mainMenu.handleClick(mousePos);  // 0=Easy,1=Medium,2=Hard,3=Nightmare,4=Exit
					if (choice == 4) // Exit
						m_DELETEexitGame = true;
					else if (choice >= 0 && choice <= 3) // Difficulty options
					{
						m_difficulty = choice;

						// Set AI depth based on difficulty
						if (choice == 3) // Nightmare
							m_aiDepth = 5;
						else
							m_aiDepth = choice + 1;  // Easy=1, Medium=2, Hard=3

						m_showMenu = false;
						resetGame();
						std::cout << "Difficulty selected: " << m_difficulty
							<< " | AI Depth: " << m_aiDepth << std::endl;
					}
				}
			}
			if (gameOver && newEvent->is<sf::Event::KeyPressed>())
			{
				const sf::Event::KeyPressed* keypress = newEvent->getIf<sf::Event::KeyPressed>();
				if (sf::Keyboard::Key::Escape == keypress->code)
				{
					m_showMenu = true;  // Return to menu
					continue;
				}
			}
			continue;  // skip the rest of the event handling while menu is active
		}

		if (newEvent->is<sf::Event::KeyPressed>())
		{
			processKeys(newEvent);
		}

		if (newEvent->is<sf::Event::MouseButtonPressed>())
		{
			const auto* mb = newEvent->getIf<sf::Event::MouseButtonPressed>();

			if (mb->button == sf::Mouse::Button::Left)
			{
				auto mp = sf::Mouse::getPosition(m_window);

				if (m_phase == GamePhase::Placement && m_turn == Owner::Human)
				{
					if (m_human.handleClick(mp.x, mp.y, m_board))
					{
						m_turn = Owner::NPC;
					}
				}
				else if (m_phase == GamePhase::Movement && m_turn == Owner::Human)
				{
					if (m_human.handleMovementClick(mp.x, mp.y, m_board))
					{
						m_turn = Owner::NPC;
					}
				}
			}
		}
	}
}

/**
 * @brief Handles keyboard key press events
 *
 * @param t_event The keyboard event to process
 *
 * Processes Escape key for menu/exit functionality and number keys (1-3)
 * for piece selection during placement phase.
 */
void Game::processKeys(const std::optional<sf::Event> t_event)
{
	const sf::Event::KeyPressed* keypress = t_event->getIf<sf::Event::KeyPressed>();

	if (sf::Keyboard::Key::Escape == keypress->code)
	{
		if (gameOver)
		{
			m_showMenu = true;  // Return to menu on win screen
		}
		else
		{
			m_DELETEexitGame = true;  // Exit during normal gameplay
		}
	}
	else if (sf::Keyboard::Key::Num1 == keypress->code)
	{
		m_human.selectPiece(PieceType::Donkey);
	}
	else if (sf::Keyboard::Key::Num2 == keypress->code)
	{
		m_human.selectPiece(PieceType::Snake);
	}
	else if (sf::Keyboard::Key::Num3 == keypress->code)
	{
		m_human.selectPiece(PieceType::Frog);
	}
}

/**
 * @brief Checks continuous keyboard state for held keys
 *
 * Currently monitors Escape key for exit during gameplay.
 */
void Game::checkKeyboardState()
{
	if (!gameOver && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		m_DELETEexitGame = true;
	}
}

// ============================================================================
// UPDATE & RENDER
// ============================================================================

/**
 * @brief Updates game state each frame
 *
 * @param t_deltaTime Time elapsed since last update
 *
 * Checks keyboard state, manages turn logic, and handles window closing.
 */
void Game::update(sf::Time t_deltaTime)
{
	checkKeyboardState();
	checkTurn();

	if (!gameOver)
	{
		checkTurn();
	}

	if (m_DELETEexitGame)
	{
		m_window.close();
	}
}

/**
 * @brief Renders all game elements to the window
 *
 * Draws the game grid, pieces, valid move highlights, and overlays
 * the win screen when game is over. Displays menu when active.
 */
void Game::render()
{
	m_window.clear(ULTRAMARINE);

	if (m_showMenu)
	{
		m_mainMenu.draw(m_window);
	}
	else
	{
		// Draw grid tiles
		for (int y = 0; y < GRID_HEIGHT; ++y)
		{
			for (int x = 0; x < GRID_WIDTH; ++x)
			{
				m_window.draw(m_grid[y][x].shape);
			}
		}

		// Draw pieces
		for (int y = 0; y < GRID_HEIGHT; ++y)
		{
			for (int x = 0; x < GRID_WIDTH; ++x)
			{
				const Cell& cell = m_board[y][x];
				if (cell.type != PieceType::None)
				{
					renderPiece(x, y, cell);
				}
			}
		}

		for (auto& p : m_human.validMoves)
		{
			highlightTile(p.first, p.second, highlightColor);
		}

		// Draw legend panel
		drawLegend();

		// Draw win screen on top if game is over
		if (gameOver)
		{
			displayWinScreen(m_window);
		}
	}
	m_window.display();
}

// ============================================================================
// LEGEND RENDERING
// ============================================================================

/**
 * @brief Draws the legend panel on the right side of the screen
 *
 * Displays piece types, counts for both players, and current phase information
 */
void Game::drawLegend()
{
	float legendX = 800.f;
	float legendWidth = 200.f;

	// Background panel
	sf::RectangleShape panel(sf::Vector2f(legendWidth, 600.f));
	panel.setPosition({ legendX, 0.f });
	panel.setFillColor(sf::Color(30, 30, 50, 220));
	m_window.draw(panel);

	// Title
	sf::Text title(gameFont);
	title.setString("LEGEND");
	title.setCharacterSize(24);
	title.setFillColor(sf::Color::White);
	title.setPosition({ legendX + 55.f, 10.f });
	m_window.draw(title);

	// Phase indicator
	sf::Text phaseText(gameFont);
	phaseText.setString(m_phase == GamePhase::Placement ? "Phase: SETUP" : "Phase: BATTLE");
	phaseText.setCharacterSize(16);
	phaseText.setFillColor(sf::Color::Yellow);
	phaseText.setPosition({legendX + 20.f, 45.f});
	m_window.draw(phaseText);

	float yPos = 80.f;

	// Human Player Section
	Game::drawPlayerSection("YOU (Green)", legendX, yPos, m_human, sf::Color::Green);
	yPos += 180.f;

	// AI Player Section
	drawPlayerSection("AI (Red)", legendX, yPos, m_ai, sf::Color::Red);
	yPos += 180.f;

	// Turn indicator
	sf::Text turnText(gameFont);
	turnText.setString(m_turn == Owner::Human ? "YOUR TURN" : "AI THINKING...");
	turnText.setCharacterSize(18);
	turnText.setFillColor(m_turn == Owner::Human ? sf::Color::Green : sf::Color::Red);
	turnText.setPosition({ legendX + 25.f, yPos });
	m_window.draw(turnText);
}

/**
 * @brief Draws a player's piece information section
 *
 * @param playerName Name to display for the player
 * @param x X position for the section
 * @param y Y position for the section
 * @param player Player object containing piece counts
 * @param color Color representing the player
 */
void Game::drawPlayerSection(const std::string& playerName, float x, float y,
	const Player& player, const sf::Color& color)
{
	// Player name
	sf::Text nameText(gameFont);
	nameText.setString(playerName);
	nameText.setCharacterSize(18);
	nameText.setFillColor(color);
	nameText.setPosition({ x + 20.f, y });
	m_window.draw(nameText);

	y += 30.f;

	// Donkey
	drawPieceInfo("Circle (Donkey)", player.donkeys, x + 30.f, y, color, PieceType::Donkey);
	y += 40.f;

	// Snake
	drawPieceInfo("Square (Snake)", player.snake, x + 30.f, y, color, PieceType::Snake);
	y += 40.f;

	// Frog
	drawPieceInfo("Triangle (Frog)", player.frog, x + 30.f, y, color, PieceType::Frog);
}

/**
 * @brief Draws information for a single piece type
 *
 * @param name Piece name to display
 * @param count Number remaining
 * @param x X position
 * @param y Y position
 * @param color Player color
 * @param type Piece type for rendering the shape
 */
void Game::drawPieceInfo(const std::string& name, int count, float x, float y,
	const sf::Color& color, PieceType type)
{
	// Draw small icon of the piece
	float iconSize = 20.f;
	sf::Vector2f iconPos(x - 25.f, y + 5.f);

	if (type == PieceType::Donkey)
	{
		sf::CircleShape icon(iconSize * 0.4f);
		icon.setFillColor(color);
		icon.setPosition(iconPos);
		m_window.draw(icon);
	}
	else if (type == PieceType::Snake)
	{
		sf::RectangleShape icon(sf::Vector2f(iconSize * 0.8f, iconSize * 0.8f));
		icon.setFillColor(color);
		icon.setPosition(iconPos);
		m_window.draw(icon);
	}
	else if (type == PieceType::Frog)
	{
		sf::CircleShape icon(iconSize * 0.5f, 3);
		icon.setFillColor(color);
		icon.setPosition(iconPos);
		m_window.draw(icon);
	}

	// Text info
	sf::Text text(gameFont);
	text.setString(name + ": " + std::to_string(count));
	text.setCharacterSize(14);
	text.setFillColor(sf::Color::White);
	text.setPosition({ x, y });
	m_window.draw(text);
}

// ============================================================================
// CheckWinCondition
// ============================================================================

/**
 * @brief Checks if the specified player has won the game
 *
 * @param player The player to check for win condition
 * @return true if player has 4 or more pieces in a row
 * @return false otherwise
 *
 * Scans entire board checking horizontal, vertical, and diagonal lines
 * for 4-in-a-row victories.
 */
bool Game::checkWinCondition(Owner player)
{
	// Scan the entire board
	for (int row = 0; row < GRID_HEIGHT; ++row)
	{
		for (int col = 0; col < GRID_WIDTH; ++col)
		{
			// If this cell belongs to the player
			if (m_board[row][col].owner == player && m_board[row][col].type != PieceType::None)
			{
				// Check all 4 directions for 4-in-a-row
				int horizontal = m_ai.countInRow(m_board, col, row, 1, 0, player);
				int vertical = m_ai.countInRow(m_board, col, row, 0, 1, player);
				int diagonalRight = m_ai.countInRow(m_board, col, row, 1, 1, player);
				int diagonalLeft = m_ai.countInRow(m_board, col, row, -1, 1, player);

				// If any direction has 4+ pieces, player wins!
				if (horizontal >= 4 || vertical >= 4 ||
					diagonalRight >= 4 || diagonalLeft >= 4)
				{
					return true;
				}
			}
		}
	}
	return false;
}

// ============================================================================
// displayWinScreen
// ============================================================================

/**
 * @brief Displays the victory screen overlay
 *
 * @param window Reference to the render window
 *
 * Shows semi-transparent overlay with winner announcement and
 * instructions to return to menu.
 */
void Game::displayWinScreen(sf::RenderWindow& window)
{
	// Semi-transparent dark overlay
	sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
	overlay.setFillColor(sf::Color(0, 0, 0, 180));
	window.draw(overlay);

	// Winner text - using gameFont member variable
	sf::Text winnerText(gameFont);  // SFML 3.0 constructor
	winnerText.setCharacterSize(60);
	winnerText.setFillColor(sf::Color::White);

	if (winner == Owner::Human)
		winnerText.setString("YOU WIN!");
	else if (winner == Owner::NPC)
		winnerText.setString("AI WINS!");

	// Center the text
	sf::FloatRect textBounds = winnerText.getGlobalBounds();
	winnerText.setPosition(sf::Vector2f(window.getSize().x / 2.f - textBounds.size.x / 2.f, window.getSize().y / 2.f - 80.f));

	window.draw(winnerText);

	// Instruction text
	sf::Text instructionText(gameFont);  // SFML 3.0 constructor
	instructionText.setCharacterSize(30);
	instructionText.setFillColor(sf::Color(200, 200, 200));
	instructionText.setString("Press ESC to return to menu");

	sf::FloatRect instrBounds = instructionText.getGlobalBounds();
	instructionText.setPosition(sf::Vector2f(window.getSize().x / 2.f - instrBounds.size.x / 2.f, window.getSize().y / 2.f + 20.f));

	window.draw(instructionText);
}

// ============================================================================
// Resets game for rematch
// ============================================================================

/**
 * @brief Resets the game to initial state for a new match
 *
 * Clears the board, reinitializes both players, and resets all
 * game state flags to starting values.
 */
void Game::resetGame()
{
	// Clear the board
	m_board.clear();

	// Reset players
	m_human = HumanPlayer();
	m_ai = AIPlayer();

	// Reset game state
	gameOver = false;
	winner = Owner::None;
	winnerAnnounced = false;
	m_inPlacement = true;
	m_phase = GamePhase::Placement;
	m_turn = Owner::Human;

	std::cout << "Game reset - starting new match!\n";
}

/**
 * @brief Renders a game piece at the specified grid position
 *
 * @param x Grid x-coordinate
 * @param y Grid y-coordinate
 * @param cell Cell data containing piece type and owner
 *
 * Draws different shapes for different piece types: circles for Donkeys,
 * rectangles for Snakes, and triangles for Frogs. Color indicates ownership.
 */
void Game::renderPiece(int x, int y, const Cell& cell)
{
	sf::Vector2f pos{ x * tileWidth, y * tileHeight };
	sf::Color color = (cell.owner == Owner::Human) ? sf::Color::Green : sf::Color::Red;

	if (cell.type == PieceType::Donkey)
	{
		sf::CircleShape circle(std::min(tileWidth, tileHeight) * 0.35f);
		circle.setFillColor(color);
		circle.setPosition(pos + sf::Vector2f(tileWidth * 0.15f, tileHeight * 0.15f));
		m_window.draw(circle);
	}
	else if (cell.type == PieceType::Snake)
	{
		sf::RectangleShape rect({ tileWidth * 0.7f, tileHeight * 0.7f });
		rect.setFillColor(color);
		rect.setPosition(pos + sf::Vector2f(tileWidth * 0.15f, tileHeight * 0.15f));
		m_window.draw(rect);
	}
	else if (cell.type == PieceType::Frog)
	{
		sf::CircleShape triangle(std::min(tileWidth, tileHeight) * 0.4f, 3);
		triangle.setFillColor(color);
		triangle.setPosition(pos + sf::Vector2f(tileWidth * 0.1f, tileHeight * 0.1f));
		m_window.draw(triangle);
	}
}

/**
 * @brief Highlights a grid tile with the specified color
 *
 * @param x Grid x-coordinate
 * @param y Grid y-coordinate
 * @param color Color to use for highlighting
 *
 * Used to show valid move destinations during movement phase.
 */
void Game::highlightTile(int x, int y, sf::Color color)
{
	sf::RectangleShape rect;
	rect.setSize({ tileWidth - 2.f, tileHeight - 2.f });
	rect.setPosition(sf::Vector2f(x * tileWidth, y * tileHeight));
	rect.setFillColor(color);

	m_window.draw(rect);
}

// ============================================================================
// SETUP METHODS
// ============================================================================

/**
 * @brief Initializes the game grid with tiles
 *
 * Creates a 2D grid of visual tiles for rendering the game board.
 * Each tile is positioned and colored appropriately.
 */
void Game::setupGrid()
{
	m_grid.resize(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));

	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Tile& tile = m_grid[y][x];
			tile.shape.setSize({ tileWidth - 1.0f, tileHeight - 1.0f });
			tile.shape.setPosition(sf::Vector2f{ x * tileWidth, y * tileHeight });
			tile.shape.setFillColor(sf::Color(255, 155, 155));
		}
	}
}

// ============================================================================
// GAME LOGIC - TURN MANAGEMENT
// ============================================================================

/**
 * @brief Manages turn logic and phase transitions
 *
 * Checks for win conditions first, then handles turn execution based
 * on current game phase (placement vs movement) and whose turn it is.
 * Automatically transitions from placement to movement phase when all
 * pieces are placed.
 */
void Game::checkTurn()
{
	// Check for wins FIRST before doing anything else
	if (checkWinCondition(Owner::Human))
	{
		gameOver = true;
		winner = Owner::Human;
		if (!winnerAnnounced)
		{
			std::cout << "Human wins!\n";
			winnerAnnounced = true;
		}
		return;
	}

	if (checkWinCondition(Owner::NPC))
	{
		gameOver = true;
		winner = Owner::NPC;
		if (!winnerAnnounced)
		{
			std::cout << "AI wins!\n";
			winnerAnnounced = true;
		}
		return;
	}

	if (m_inPlacement)
	{
		if (m_human.allPiecesPlaced() && m_ai.allPiecesPlaced())
		{
			m_phase = GamePhase::Movement;    // change phase here
			m_inPlacement = false;
			std::cout << " phase changed" << std::endl;
			return;
		}

		if (m_turn == Owner::NPC)
			handleNpcTurn();
	}
	else
	{
		if (m_phase == GamePhase::Movement && m_turn == Owner::Human)
		{
			// Human's movement turn - handled by mouse clicks
		}
		else
		{
			Move m = m_ai.findBestMoveMovement(m_board, m_human, m_aiDepth); // use selected depth
			m_ai.executeMovement(m, m_board);
			m_turn = Owner::Human;
		}
	}
}

/**
 * @brief Handles AI turn during placement phase
 *
 * AI evaluates the board, finds the best placement move, executes it,
 * and automatically selects the next available piece type if current
 * type is depleted.
 */
void Game::handleNpcTurn()
{
	if (m_ai.empty()) return;

	// AI finds and executes best move
	Move bestMove = m_ai.findBestMove(m_board, m_human);

	if (bestMove.x != -1 && bestMove.y != -1)
	{
		if (m_board.placePiece(bestMove.x, bestMove.y, m_ai.selected, Owner::NPC))
		{
			m_ai.take();

			std::cout << "AI placed at (" << bestMove.x << ", " << bestMove.y
				<< ") with score: " << bestMove.score << std::endl;

			// Auto-select next available piece type
			if (m_ai.selected == PieceType::Donkey && m_ai.donkeys == 0)
				m_ai.selected = PieceType::Snake;
			else if (m_ai.selected == PieceType::Snake && m_ai.snake == 0)
				m_ai.selected = PieceType::Frog;

			m_turn = Owner::Human;
		}
	}
	else
	{
		std::cout << "AI couldn't find valid move" << std::endl;
	}
}