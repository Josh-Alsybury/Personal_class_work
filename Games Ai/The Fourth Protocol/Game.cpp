/// <summary>
/// author Pete Lowe May 2025
/// you need to change the above line or lose marks
/// </summary>
#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

/// <summary>
/// default constructor
/// setup the window properties
/// load and setup the texts
/// load and setup the images
/// load and setup the sounds
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ sf::Vector2u{800U, 600U}, 32U }, "SFML Game 3.0" },
	m_DELETEexitGame{false} //when true game will exit
{

	setupGrid();
	setupTexts(); // load font 
	setupSprites(); // load texture
}

/// <summary>
/// default destructor we didn't dynamically allocate anything
/// so we don't need to free it, but mthod needs to be here
/// </summary>
Game::~Game()
{
}


/// <summary>
/// main game loop
/// update 60 times per second,
/// process update as often as possible and at least 60 times per second
/// draw as often as possible but only updates are on time
/// if updates run slow then don't render frames
/// </summary>
void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame); //60 fps
		}
		render(); // as many as possible
	}
}
/// <summary>
/// handle user and system events/ input
/// get key presses/ mouse moves etc. from OS
/// and user :: Don't do game update here
/// </summary>
void Game::processEvents()
{
	
	while (const std::optional newEvent = m_window.pollEvent())
	{
		if ( newEvent->is<sf::Event::Closed>()) // close window message 
		{
			m_DELETEexitGame = true;
		}
		if (newEvent->is<sf::Event::KeyPressed>()) //user pressed a key
		{
			processKeys(newEvent);
		}
		if (newEvent->is<sf::Event::MouseButtonPressed>()) {
			const auto* mb = newEvent->getIf<sf::Event::MouseButtonPressed>();
			if (mb->button == sf::Mouse::Button::Left) {
				auto mp = sf::Mouse::getPosition(m_window);
				handleHumanClick(mp.x, mp.y);
			}
		}
	}


}


/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(const std::optional<sf::Event> t_event)
{
	const sf::Event::KeyPressed *newKeypress = t_event->getIf<sf::Event::KeyPressed>();
	if (sf::Keyboard::Key::Escape == newKeypress->code)
	{
		m_DELETEexitGame = true; 
	}
	if (sf::Keyboard::Key::Num1 == newKeypress->code)
	{
		m_human.selected = PieceType::Donkey;
		std::cout << "donkey picked" << std::endl;
	}
	if (sf::Keyboard::Key::Num2 == newKeypress->code)
	{
		m_human.selected = PieceType::Snake;
		std::cout << "snake picked" << std::endl;
	}
	if (sf::Keyboard::Key::Num3 == newKeypress->code)
	{
		m_human.selected = PieceType::Frog;
		std::cout << "frog picked" << std::endl;
	}
}

/// <summary>
/// Check if any keys are currently pressed
/// </summary>
void Game::checkKeyboardState()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		m_DELETEexitGame = true; 
	}
}

/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
void Game::update(sf::Time t_deltaTime)
{
	checkKeyboardState();

	// Handle NPC turn automatically
	checkTurn();

	if (m_DELETEexitGame)
	{
		m_window.close();
	}
}

/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(ULTRAMARINE);

	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Tile& tile = m_grid[y][x];
			m_window.draw(tile.shape);
		}
	}

	for (int y = 0; y < GRID_HEIGHT; ++y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			const Cell& c = m_board[y][x];
			if (c.type == PieceType::None) continue;

			sf::Vector2f pos{ x * tileWidth, y * tileHeight };
			sf::Color col = (c.owner == Owner::Human) ? sf::Color::Green : sf::Color::Red;

			if (c.type == PieceType::Donkey) {
				sf::CircleShape circ(std::min(tileWidth, tileHeight) * 0.35f);
				circ.setFillColor(col);
				circ.setPosition(pos + sf::Vector2f(tileWidth * 0.15f, tileHeight * 0.15f));
				m_window.draw(circ);
			}
			else if (c.type == PieceType::Snake) {
				sf::RectangleShape rect({ tileWidth * 0.7f, tileHeight * 0.7f });
				rect.setFillColor(col);
				rect.setPosition(pos + sf::Vector2f(tileWidth * 0.15f, tileHeight * 0.15f));
				m_window.draw(rect);
			}
			else if (c.type == PieceType::Frog) {
				sf::CircleShape tri(std::min(tileWidth, tileHeight) * 0.4f, 3);
				tri.setFillColor(col);
				tri.setPosition(pos + sf::Vector2f(tileWidth * 0.1f, tileHeight * 0.1f));
				m_window.draw(tri);
			}
		}
	}

	m_window.display();
}

void Game::setupGrid()
{
	int windowWidth = m_window.getSize().x;
	int windowHeight = m_window.getSize().y;

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


/// <summary>
/// load the font and setup the text message for screen
/// </summary>
void Game::setupTexts()
{


}

/// <summary>
/// load the texture and setup the sprite for the logo
/// </summary>
void Game::setupSprites()
{

}

void Game::checkTurn()
{
	if (m_turn == Owner::Human)
	{
		//nothing needs to happen here as its just players input
	}
	else if (m_turn == Owner::NPC)
	{
		handleNpcTurn();
	}
}


bool Game::placeAt(int gridx, int gridy, PieceType piecet, Owner who)
{
	// Check cell is within the bounds of the  board
	if (gridx < 0 || gridy < 0 || gridx >= GRID_WIDTH || gridy >= GRID_HEIGHT) return false;
	// Check if cell is already occupied
	if (m_board[gridy][gridx].type != PieceType::None) return false;
	m_board[gridy][gridx] = { piecet, who };// Save piece and owner board state

	std::cout << "Placed " << static_cast<int>(piecet)
		<< " by " << static_cast<int>(who)
		<< " at (" << gridx << ", " << gridy << ")" << std::endl;

	return true;
}

void Game::handleHumanClick(int pixelx, int pixely)
{
	if (!m_inPlacement || m_turn != Owner::Human) return;

	int gridx = static_cast<int>(pixelx / tileWidth);
	int gridy = static_cast<int>(pixely / tileHeight);

	// Determine pointer to stock for the currently selected piece
	int* stock = nullptr; ///////////////////////stock is total amount of pieces///////////////////////////

	if (m_human.selected == PieceType::Donkey) stock = &m_human.donkeys;
	else if (m_human.selected == PieceType::Snake) stock = &m_human.snake;
	else if (m_human.selected == PieceType::Frog)  stock = &m_human.frog;

	// No stock? refuse placement
	if (!stock || *stock <= 0) {
		std::cout << "No pieces of this type left. Select another." << std::endl;
		return;
	}

	// Try to place; only decrement stock on success
	if (placeAt(gridx, gridy, m_human.selected, Owner::Human)) {
		--(*stock);
		m_turn = Owner::NPC; // swaps to npc
	}
	else {
		std::cout << "Cell occupied. Pick another cell." << std::endl;
	}
}

void Game::handleNpcTurn()
{
	if (!m_inPlacement || m_turn != Owner::NPC) return;
	if (m_ai.empty()) return;
	Move bestMove = { -1, -1, -99999 };

	// Try all empty cells
	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			// Check if cell is empty
			if (m_board[y][x].type == PieceType::None)
			{
				// Simulate placing AI's piece
				m_board[y][x] = { m_ai.selected, Owner::NPC };

				// Evalate this move
				int aiScore = evaluateBoard(Owner::NPC);
				int humanScore = evaluateBoard(Owner::Human);

				// Minimax maximize AI score, minimize human score
				int moveScore = aiScore - humanScore;

				// Undo the simulated move
				m_board[y][x] = { PieceType::None, Owner::None };

				// Keep track of best move
				if (moveScore > bestMove.score)
				{
					bestMove.x = x;
					bestMove.y = y;
					bestMove.score = moveScore;
				}
			}
		}
	}
	// If  found a valid move, place it
	if (bestMove.x != -1 && bestMove.y != -1)
	{
		if (placeAt(bestMove.x, bestMove.y, m_ai.selected, Owner::NPC))
		{
			m_ai.take(); // Consume one piece from AI's stock

			std::cout << "AI placed at (" << bestMove.x << ", " << bestMove.y
				<< ") with score: " << bestMove.score << std::endl;

			// Switch to next piece type if current is depleted
			if (m_ai.selected == PieceType::Donkey && m_ai.donkeys == 0)
				m_ai.selected = PieceType::Snake;
			else if (m_ai.selected == PieceType::Snake && m_ai.snake == 0)
				m_ai.selected = PieceType::Frog;

			// After AI places, switch turn back to human
			m_turn = Owner::Human;
			return;
		}
	}
	std::cout << "AI couldn't find empty cell after 100 attempts" << std::endl;
}

int Game::evaluateBoard(Owner player)
{
	int score = 0;
	// Check all occupied positions
	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			if (m_board[y][x].owner == player && m_board[y][x].type != PieceType::None)
			{
				// Check all 4 directions: horizontal, vertical, diagonal down-right, diagonal down-left
				int horizontal = countInRow(x, y, 1, 0, player);
				int vertical = countInRow(x, y, 0, 1, player);
				int diagonalRight = countInRow(x, y, 1, 1, player);
				int diagonalLeft = countInRow(x, y, -1, 1, player);

				// Take the maximum count in any direction from this position
				int maxInRow = std::max({ horizontal, vertical, diagonalRight, diagonalLeft });

				// Score increases exponentially with more in a row
				// 2 in a row: 10 points, 3 in a row: 100 points, 4 in a row: 1000 points, 5+ in a row: 10000 points
				if (maxInRow >= 5) score += 10000;
				else if (maxInRow == 4) score += 1000;
				else if (maxInRow == 3) score += 100;
				else if (maxInRow == 2) score += 10;
				else score += 1;
			}
		}
	}
	return score;
}

int Game::countInRow(int x, int y, int dx, int dy, Owner player)
{
	int count = 0;
	int nx = x;
	int ny = y;

	// Count in the positive direction
	while (nx >= 0 && ny >= 0 && nx < GRID_WIDTH && ny < GRID_HEIGHT)
	{
		if (m_board[ny][nx].owner == player && m_board[ny][nx].type != PieceType::None)
		{
			count++;
			nx += dx;
			ny += dy;
		}
		else
		{
			break;
		}
	}

	nx = x - dx;
	ny = y - dy;
	// count negative direction
	while (nx >= 0 && ny >= 0 && nx < GRID_WIDTH && ny < GRID_HEIGHT)
	{
		if (m_board[ny][nx].owner == player && m_board[ny][nx].type != PieceType::None)
		{
			count++;
			nx -= dx;
			ny -= dy;
		}
		else
		{
			break;
		}
	}

	return count;
}
