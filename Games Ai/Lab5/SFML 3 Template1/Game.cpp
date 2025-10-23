/// <summary>
/// Author: [Your Name] - [Month Year]
/// (Change this line or you lose marks!)
/// </summary>

#include "Game.h"
#include <iostream>
#include <queue>


Game::Game() :
	m_window{ sf::VideoMode{ sf::Vector2u{800U, 600U}, 32U }, "SFML Flow Field" },
	m_exitGame{ false }
{
	setupGrid();
	setupTexts();
	setupSprites();
	setupAudio();
}

Game::~Game() {} 


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


void Game::processEvents()
{
	while (const std::optional newEvent = m_window.pollEvent())
	{
		if (const auto* closed = newEvent->getIf<sf::Event::Closed>())
		{
			m_exitGame = true;
		}
		else if (const auto* keypress = newEvent->getIf<sf::Event::KeyPressed>())
		{
			processKeys(keypress);
		}
		else if (const auto* mouseButton = newEvent->getIf<sf::Event::MouseButtonPressed>())
		{
			handleMouseInput(*mouseButton);
		}
	}
}

void Game::processKeys(const sf::Event::KeyPressed* t_keypress)
{
	if (sf::Keyboard::Key::Escape == t_keypress->code)
	{
		m_exitGame = true;
	}
}

void Game::checkKeyboardState()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		m_exitGame = true;
	}
}

void Game::update(sf::Time t_deltaTime)
{
	checkKeyboardState();
	moveCharacter(t_deltaTime);

	if (m_exitGame)
	{
		m_window.close();
	}
}

void Game::render()
{
	m_window.clear(ULTRAMARINE);

	for (auto& row : m_grid)
		for (auto& tile : row)
			m_window.draw(tile.shape);

	m_window.display();
}

void Game::setupTexts() {}
void Game::setupSprites() {}
void Game::setupAudio() {}


void Game::setupGrid()
{
	m_grid.resize(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));

	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Tile& tile = m_grid[y][x];
			tile.shape.setSize(sf::Vector2f(TILE_SIZE - 1, TILE_SIZE - 1));
			tile.shape.setPosition({ static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE) });
			tile.shape.setFillColor(sf::Color(0, 0, 100));
		}
	}
}


void Game::handleMouseInput(const sf::Event::MouseButtonPressed& event)
{
	int x = event.position.x / TILE_SIZE;
	int y = event.position.y / TILE_SIZE;


	if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT)
		return;

	if (event.button == sf::Mouse::Button::Left)
	{
		if (m_goalPos.x != -1)
			m_grid[m_goalPos.y][m_goalPos.x].type = TileType::Empty;

		m_goalPos = { x, y };
		m_grid[y][x].type = TileType::Goal;
	}

	else if (event.button == sf::Mouse::Button::Right)
	{
		if (m_startPos.x != -1)
			m_grid[m_startPos.y][m_startPos.x].type = TileType::Empty;

		m_startPos = { x, y };
		m_currentAgentPos = m_startPos;
		m_grid[y][x].type = TileType::Start;
	}

	else if (event.button == sf::Mouse::Button::Middle)
	{
		if (sf::Vector2i(x, y) != m_startPos && sf::Vector2i(x, y) != m_goalPos)
		{
			Tile& t = m_grid[y][x];
			t.type = (t.type == TileType::Empty) ? TileType::Wall : TileType::Empty;
		}
	}

	updateGridColors();
	calculateFlowField();
}


void Game::updateGridColors()
{
	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Tile& t = m_grid[y][x];
			sf::Color color;

			if (m_currentAgentPos == sf::Vector2i(x, y))
				color = sf::Color::Yellow;
			else if (t.isOnPath)
				color = sf::Color::Yellow;
			else if (t.cost < std::numeric_limits<int>::max() && t.type == TileType::Empty)
			{
				int shade = 255 - std::min(255, t.cost * 10);
				color = sf::Color(shade, shade, 255);
			}
			else
			{
				switch (t.type)
				{
				case TileType::Empty: color = sf::Color(0, 0, 100); break;
				case TileType::Wall:  color = sf::Color::Black; break;
				case TileType::Start: color = sf::Color::Green; break;
				case TileType::Goal:  color = sf::Color::Red; break;
				}
			}

			t.shape.setFillColor(color);
		}
	}
}

void Game::calculateFlowField()
{
	for (auto& row : m_grid)
		for (auto& t : row)
		{
			t.cost = std::numeric_limits<int>::max();
			t.flowDirection = Direction::None;
			t.isOnPath = false;
		}

	if (m_goalPos.x == -1) return;

	std::queue<sf::Vector2i> queue;
	m_grid[m_goalPos.y][m_goalPos.x].cost = 0;
	queue.push(m_goalPos);

	const sf::Vector2i dirs[] = { {0,-1}, {0,1}, {-1,0}, {1,0} };

	while (!queue.empty())
	{
		auto pos = queue.front();
		queue.pop();

		int currentCost = m_grid[pos.y][pos.x].cost;

		for (auto d : dirs)
		{
			sf::Vector2i next = pos + d;

			if (next.x < 0 || next.x >= GRID_WIDTH || next.y < 0 || next.y >= GRID_HEIGHT)
				continue;

			Tile& neighbor = m_grid[next.y][next.x];
			if (neighbor.type == TileType::Wall) continue;

			int newCost = currentCost + 1;
			if (newCost < neighbor.cost)
			{
				neighbor.cost = newCost;
				queue.push(next);
			}
		}
	}

	for (int y = 0; y < GRID_HEIGHT; ++y)
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			sf::Vector2i pos(x, y);
			Tile& t = m_grid[y][x];

			if (t.type == TileType::Wall || pos == m_goalPos || t.cost == std::numeric_limits<int>::max())
				continue;

			int bestCost = t.cost;
			Direction bestDir = Direction::None;

			for (int i = 0; i < 4; ++i)
			{
				sf::Vector2i next = pos + dirs[i];
				if (next.x < 0 || next.x >= GRID_WIDTH || next.y < 0 || next.y >= GRID_HEIGHT)
					continue;

				int neighborCost = m_grid[next.y][next.x].cost;
				if (neighborCost < bestCost)
				{
					bestCost = neighborCost;
					bestDir = static_cast<Direction>(i + 1);
				}
			}

			t.flowDirection = bestDir;
		}

	updateGridColors();
}


void Game::moveCharacter(sf::Time t_deltaTime)
{
	static bool moving = false;
	static bool spaceHeld = false;

	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
	{
		if (!spaceHeld)
		{
			moving = !moving;
			spaceHeld = true;
		}
	}
	else
	{
		spaceHeld = false;
	}

	if (!moving || m_currentAgentPos.x == -1 || m_goalPos.x == -1)
		return;

	if (m_currentAgentPos == m_goalPos)
	{
		moving = false;
		return;
	}

	Tile& tile = m_grid[m_currentAgentPos.y][m_currentAgentPos.x];
	Direction dir = tile.flowDirection;
	sf::Vector2i next = m_currentAgentPos;

	switch (dir)
	{
	case Direction::Up:    next.y -= 1; break;
	case Direction::Down:  next.y += 1; break;
	case Direction::Left:  next.x -= 1; break;
	case Direction::Right: next.x += 1; break;
	default: moving = false; return;
	}

	if (next.x < 0 || next.x >= GRID_WIDTH || next.y < 0 || next.y >= GRID_HEIGHT)
	{
		moving = false;
		return;
	}

	m_grid[m_currentAgentPos.y][m_currentAgentPos.x].isOnPath = true;
	m_currentAgentPos = next;
	updateGridColors();
}
