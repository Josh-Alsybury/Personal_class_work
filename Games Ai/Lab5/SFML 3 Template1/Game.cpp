/// <summary>
/// Author: [Your Name] - [Month Year]
/// (Change this line or you lose marks!)
/// </summary>

#include "Game.h"
#include <iostream>
#include <queue>

ViewMode m_viewMode = ViewMode::Normal;
sf::Font m_font;

Game::Game() :
	m_window{ sf::VideoMode{ {800U, 600U} },  "SFML Flow Field" },
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
	else if (sf::Keyboard::Key::Q == t_keypress->code)
	{
		if (m_viewMode == ViewMode::Normal)
			m_viewMode = ViewMode::Cost;
		else if (m_viewMode == ViewMode::Cost)
			m_viewMode = ViewMode::Integration;
		else
			m_viewMode = ViewMode::Normal;
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

	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Tile& tile = m_grid[y][x];
			m_window.draw(tile.shape);

			if ((m_viewMode == ViewMode::Cost || m_viewMode == ViewMode::Integration) &&
				tile.integrationCost < std::numeric_limits<int>::max() &&
				tile.type != TileType::Wall)
			{
				sf::Text costText(m_font);
				costText.setFont(m_font);
				costText.setCharacterSize(10);
				costText.setFillColor(sf::Color::White);

				if (m_viewMode == ViewMode::Cost)
				{
					int stepCost = tile.integrationCost / 10; 
					costText.setString(std::to_string(stepCost));
				}
				else if (m_viewMode == ViewMode::Integration)
				{
					
					costText.setString(std::to_string(tile.integrationCost));
				}

				costText.setPosition(sf::Vector2f(
					tile.shape.getPosition().x + 2.f,
					tile.shape.getPosition().y + 1.f
				));
				m_window.draw(costText);
			}
		}
	}

	sf::RectangleShape controlsBackground;
	controlsBackground.setSize(sf::Vector2f(170.f, 135.f));
	controlsBackground.setFillColor(sf::Color::Black);
	controlsBackground.setPosition(sf::Vector2f(620.f, 10.f));
	m_window.draw(controlsBackground);

	sf::Text controlsText(m_font);
	controlsText.setCharacterSize(14);
	controlsText.setFillColor(sf::Color::White);
	controlsText.setPosition(sf::Vector2f(630.f, 15.f));
	controlsText.setString(
		"Controls:\n"
		"Left Mouse: Set Goal\n"
		"Right Mouse: Set Start\n"
		"Middle Mouse: Toggle Wall\n"
		"Space: Toggle Agent Move\n"
		"Q: Switch View\n"
		"  - Normal\n"
		"  - Cost Field\n"
		"  - Integration Field"
	);
	m_window.draw(controlsText);

	m_window.display();
}

void Game::setupTexts() {

	if (!m_font.openFromFile("assets/fonts/Jersey20-Regular.ttf"))
	{
		std::cerr << "Error loading font\n";
	}
}
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
				color = sf::Color(255, 255, 100);

			else if (t.integrationCost < std::numeric_limits<int>::max() && t.type == TileType::Empty)
			{
				int shade = 255 - std::min(255, t.integrationCost * 10);
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
			t.integrationCost = std::numeric_limits<int>::max();
			t.flowVector = { 0.f, 0.f };
			t.isOnPath = false;
		}

	if (m_goalPos.x == -1) return;

	std::queue<sf::Vector2i> queue;
	m_grid[m_goalPos.y][m_goalPos.x].integrationCost = 0;
	queue.push(m_goalPos);

	const sf::Vector2i dirs[] = {
		{ 0,-1 }, { 0,1 }, { -1,0 }, { 1,0 },
		{ -1,-1 }, { -1,1 }, { 1,-1 }, { 1,1 }
	};

	const int TILE_STRAIGHT_COST = 10;
	const int TILE_DIAGONAL_COST = static_cast<int>(10.0f * std::sqrt(2.0f));

	while (!queue.empty())
	{
		auto pos = queue.front();
		queue.pop();
		int currentCost = m_grid[pos.y][pos.x].integrationCost;
		for (auto d : dirs)
		{
			sf::Vector2i next = pos + d;
			if (next.x < 0 || next.x >= GRID_WIDTH || next.y < 0 || next.y >= GRID_HEIGHT)
				continue;
			if (abs(d.x) == 1 && abs(d.y) == 1)
			{
				if (m_grid[pos.y][next.x].type == TileType::Wall ||
					m_grid[next.y][pos.x].type == TileType::Wall)
					continue;
			}
			Tile& neighbor = m_grid[next.y][next.x];
			if (neighbor.type == TileType::Wall) continue;
			int stepCost = (abs(d.x) + abs(d.y) == 2) ? TILE_DIAGONAL_COST : TILE_STRAIGHT_COST;
			int newCost = currentCost + stepCost;
			if (newCost < neighbor.integrationCost)
			{
				neighbor.integrationCost = newCost;
				queue.push(next);
			}
		}
	}

	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Tile& t = m_grid[y][x];
			if (t.type == TileType::Wall || sf::Vector2i(x, y) == m_goalPos) continue;
			float lowestCost = static_cast<float>(t.integrationCost);
			sf::Vector2f gradient(0.f, 0.f);
			for (auto d : dirs)
			{
				sf::Vector2i n = { x + d.x, y + d.y };
				if (n.x < 0 || n.x >= GRID_WIDTH || n.y < 0 || n.y >= GRID_HEIGHT)
					continue;
				float neighborCost = static_cast<float>(m_grid[n.y][n.x].integrationCost);
				if (neighborCost < lowestCost)
				{
					lowestCost = neighborCost;
					gradient = sf::Vector2f(static_cast<float>(d.x), static_cast<float>(d.y));
				}
			}
			float length = std::sqrt(gradient.x * gradient.x + gradient.y * gradient.y);
			if (length > 0.f)
				gradient /= length;
			t.flowVector = gradient;
		}
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
	sf::Vector2f dir = tile.flowVector;

	if (dir == sf::Vector2f(0.f, 0.f))
	{
		moving = false;
		return;
	}

	sf::Vector2i next = {
		m_currentAgentPos.x + static_cast<int>(std::round(dir.x)),
		m_currentAgentPos.y + static_cast<int>(std::round(dir.y))
	};

	if (next.x < 0 || next.x >= GRID_WIDTH || next.y < 0 || next.y >= GRID_HEIGHT)
	{
		moving = false;
		return;
	}

	m_grid[m_currentAgentPos.y][m_currentAgentPos.x].isOnPath = true;
	m_currentAgentPos = next;

	updateGridColors();
}
