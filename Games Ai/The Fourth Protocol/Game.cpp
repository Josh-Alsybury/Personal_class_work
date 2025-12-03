/// <summary>
/// author: [Your Name Here] May 2025
/// Refactored game implementation
/// </summary>
#include "Game.h"
#include <iostream>

// ============================================================================
// CONSTRUCTOR & DESTRUCTOR
// ============================================================================

Game::Game() :
	m_window{ sf::VideoMode{ sf::Vector2u{800U, 600U}, 32U }, "SFML Game 3.0" },
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

Game::~Game()
{
}

// ============================================================================
// MAIN GAME LOOP
// ============================================================================

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
			// Menu mouse click
			if (newEvent->is<sf::Event::MouseButtonPressed>())
			{
				const auto* mb = newEvent->getIf<sf::Event::MouseButtonPressed>();
				if (mb->button == sf::Mouse::Button::Left)
				{
					sf::Vector2f mousePos = m_window.mapPixelToCoords(mb->position);
					int choice = m_mainMenu.handleClick(mousePos);  // 0=Easy,1=Medium,2=Hard,3=Exit
					if (choice == 3) // Exit
						m_DELETEexitGame = true;
					else if (choice >= 0 && choice <= 2)
					{
						m_difficulty = choice;       // store difficulty
						m_aiDepth = choice +1;      // Easy=1, Medium=2, Hard=3
						m_showMenu = false;          // hide menu and start game
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


void Game::processKeys(const std::optional<sf::Event> t_event)
{
	const sf::Event::KeyPressed* keypress = t_event->getIf<sf::Event::KeyPressed>();

	if (sf::Keyboard::Key::Escape == keypress->code)
	{
		if (gameOver)
		{
			m_showMenu = true;  //  Return to menu on win screen
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
		        // Draw win screen on top if game is over
        if (gameOver)
        {
            displayWinScreen(m_window);
        }
    
	}
	m_window.display();
}

// ============================================================================
// CheckWinCondition - 
// ============================================================================

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
// displayWinScreen - 
// ============================================================================

void Game::displayWinScreen(sf::RenderWindow & window)
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
	winnerText.setPosition(sf::Vector2f(window.getSize().x / 2.f - textBounds.size.x / 2.f,window.getSize().y / 2.f - 80.f));

	window.draw(winnerText);

	// Instruction text
	sf::Text instructionText(gameFont);  // SFML 3.0 constructor
	instructionText.setCharacterSize(30);
	instructionText.setFillColor(sf::Color(200, 200, 200));
	instructionText.setString("Press ESC to return to menu");

	sf::FloatRect instrBounds = instructionText.getGlobalBounds();
	instructionText.setPosition(sf::Vector2f(window.getSize().x / 2.f - instrBounds.size.x / 2.f,window.getSize().y / 2.f + 20.f));

	window.draw(instructionText);
}


// ============================================================================
// Resets game for rematch
// ============================================================================

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
			tile.shape.setFillColor(sf::Color(0, 0, 100));
		}
	}
}


// ============================================================================
// GAME LOGIC - TURN MANAGEMENT
// ============================================================================

void Game::checkTurn()
{
	// Check for wins FIRST before doing anything else
	if (checkWinCondition(Owner::Human))
	{
		gameOver = true;
		winner = Owner::Human;
		if (!winnerAnnounced)  // ADD THIS CHECK
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
		if (!winnerAnnounced)  // ADD THIS CHECK
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
			std::cout<<" phase changed" << std::endl;
			return;
		}

		if (m_turn == Owner::NPC)
			handleNpcTurn();
	}
	else
	{
		if (m_phase == GamePhase::Movement&& m_turn == Owner::Human)
		{
	
		}
		else
		{
			Move m = m_ai.findBestMoveMovement(m_board, m_human, m_aiDepth); // use selected depth
			m_ai.executeMovement(m, m_board);
			m_turn = Owner::Human;
		}
	}
}

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

