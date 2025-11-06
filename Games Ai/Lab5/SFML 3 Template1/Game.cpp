#include "Game.h"
#include <iostream>
#include <queue>

ViewMode m_viewMode = ViewMode::Normal;
sf::Font m_font;


// CONSTRUCTOR - Set up the window and game

Game::Game() :
	m_window{ sf::VideoMode{ {800U, 600U} },  "SFML Flow Field" },
	m_exitGame{ false }
{
	setupGrid();    // Create the 50x50 grid
	setupTexts();   // Load fonts
	setupSprites(); // (Empty for now)
	setupAudio();   // (Empty for now)
}

Game::~Game() {} 

// MAIN GAME LOOP - Runs at 60 FPS

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

// HANDLE INPUT - Mouse clicks, keyboard
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


// KEYBOARD CONTROLS
void Game::processKeys(const sf::Event::KeyPressed* t_keypress)
{
	
	if (sf::Keyboard::Key::Escape == t_keypress->code)
	{
		m_exitGame = true;
	}
	else if (sf::Keyboard::Key::Q == t_keypress->code)
	{
		switch (m_viewMode)
		{
		case ViewMode::Normal: 
			m_viewMode = ViewMode::Cost; 
			break;
		case ViewMode::Cost: 
			m_viewMode = ViewMode::Integration; 
			break;
		case ViewMode::Integration: 
			m_viewMode = ViewMode::HeatMap; 
			break;
		case ViewMode::HeatMap: 
			m_viewMode = ViewMode::VectorField; 
			break;
		default: 
			m_viewMode = ViewMode::Normal; 
			break;
		}
	}
	else if (sf::Keyboard::Key::Space == t_keypress->code)
	{
		for (auto& agent : m_agents)
		{
			agent.isMoving = !agent.isMoving; 
		}
	}
}

void Game::checkKeyboardState()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		m_exitGame = true;
	}
}


// UPDATE - Move agents

void Game::update(sf::Time t_deltaTime)
{
	checkKeyboardState();
	moveCharacter(t_deltaTime); 

	if (m_exitGame)
	{
		m_window.close();
	}
}


// RENDER - Draw everything to screen

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
					int simplifiedCost = tile.integrationCost / 10; 
					costText.setString(std::to_string(simplifiedCost));
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
	controlsBackground.setSize(sf::Vector2f(170.f, 185.f));
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
		"( Capped at 5) \n"
		"Middle Mouse: Toggle Wall\n"
		"Space: Toggle Agent Move\n"
		"Q: Switch View\n"
		"  - Normal\n"
		"  - Cost Field\n"
		"  - Integration Field\n"
		"  - Heat map\n"
		"  - Vector Field"
	);
	m_window.draw(controlsText);

	if (m_viewMode == ViewMode::VectorField)
	{
		m_window.draw(m_flowVectors);
	}

	m_window.display();
}


// SETUP FUNCTIONS

void Game::setupTexts() 
{
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
			tile.shape.setPosition({ 
				static_cast<float>(x * TILE_SIZE), 
				static_cast<float>(y * TILE_SIZE) 
			});
			tile.shape.setFillColor(sf::Color(0, 0, 100)); 
		}
	}
}


// MOUSE INPUT - Set goal, agents, walls
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
		const int MAX_AGENTS = 5;
		
		bool agentRemoved = false;
		for (int i = 0; i < m_agents.size(); ++i)
		{
			if (m_agents[i].position == sf::Vector2i(x, y))
			{
				m_agents.erase(m_agents.begin() + i);
				agentRemoved = true;
				AgentCount -= 1;
				break;
			}
		}

		
		if (!agentRemoved && AgentCount < MAX_AGENTS)
		{
			Agent newAgent;
			AgentCount += 1;
			newAgent.position = { x, y };
			newAgent.startPosition = { x, y };
			
			sf::Color agentColors[] = {
				sf::Color::Green, sf::Color::Cyan, sf::Color::Magenta,
				sf::Color::Black, sf::Color(255, 165, 0) 
			};
			newAgent.color = agentColors[m_agents.size() % 5];
			m_agents.push_back(newAgent);
		}
	}
	else if (event.button == sf::Mouse::Button::Middle)
	{
		if (sf::Vector2i(x, y) != m_goalPos)
		{
			Tile& t = m_grid[y][x];
			t.type = (t.type == TileType::Empty) ? TileType::Wall : TileType::Empty;
		}
	}
	
	updateGridColors();    
	calculateFlowField(); 
}


// UPDATE COLORS - Set tile colors based on mode
void Game::updateGridColors()
{
	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Tile& t = m_grid[y][x];
			sf::Color color;
			if (t.type == TileType::Goal)
			{
				color = sf::Color::Red;
			}
			else if (t.type == TileType::Wall)
			{
				color = sf::Color::Black;
			}
			else
			{
				bool agentHere = false;
				for (const auto& agent : m_agents)
				{
					if (agent.position == sf::Vector2i(x, y))
					{
						color = agent.color;
						agentHere = true;
						break;
					}
				}
				
				if (!agentHere)
				{
					if (t.isOnPath)
					{
						color = sf::Color(255, 255, 100);
					}
					else if (m_viewMode == ViewMode::HeatMap && 
					         t.integrationCost < std::numeric_limits<int>::max())
					{
						int heat = std::clamp(255 - t.integrationCost / 4, 0, 255);
						color = sf::Color(255, heat, 0);
					}
					else if ((m_viewMode == ViewMode::Cost || m_viewMode == ViewMode::Integration) &&
						t.integrationCost < std::numeric_limits<int>::max())
					{
						int shade = 255 - std::min(255, t.integrationCost * 10);
						color = sf::Color(shade, shade, 255);
					}
					else
					{
						color = sf::Color(0, 0, 100);
					}
				}
			}
			t.shape.setFillColor(color);
		}
	}
}


// CALCULATE FLOW FIELD - The main algorithm!

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

	//Spreads costs from goal using BFS
	std::queue<sf::Vector2i> queue;
	m_grid[m_goalPos.y][m_goalPos.x].integrationCost = 0; 
	queue.push(m_goalPos);

	// 8 directions
	const sf::Vector2i dirs[] = {
		{ 0,-1 }, { 0,1 }, { -1,0 }, { 1,0 },          // Straight
		{ -1,-1 }, { -1,1 }, { 1,-1 }, { 1,1 }         // Diagonal
	};

	const int STRAIGHT_COST = 10;
	const int DIAGONAL_COST = 14;     

	while (!queue.empty())
	{
		auto pos = queue.front();
		queue.pop();
		
		int currentCost = m_grid[pos.y][pos.x].integrationCost;
		
		for (auto d : dirs)
		{
			sf::Vector2i next = pos + d;
			
			
			if (next.x < 0 || next.x >= GRID_WIDTH || 
			    next.y < 0 || next.y >= GRID_HEIGHT)
				continue;
			
			if (abs(d.x) == 1 && abs(d.y) == 1)
			{
				if (m_grid[pos.y][next.x].type == TileType::Wall ||
					m_grid[next.y][pos.x].type == TileType::Wall)
					continue;
			}
			
			Tile& neighbor = m_grid[next.y][next.x];
			if (neighbor.type == TileType::Wall) 
				continue;

			int stepCost = (abs(d.x) + abs(d.y) == 2) ? DIAGONAL_COST : STRAIGHT_COST;
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
			
			if (t.type == TileType::Wall || sf::Vector2i(x, y) == m_goalPos) 
				continue;
			
			float lowestCost = static_cast<float>(t.integrationCost);
			sf::Vector2f gradient(0.f, 0.f);
			
			for (auto d : dirs)
			{
				sf::Vector2i n = { x + d.x, y + d.y };
				
				if (n.x < 0 || n.x >= GRID_WIDTH || 
				    n.y < 0 || n.y >= GRID_HEIGHT)
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
	
	m_flowVectors.clear();
	m_flowVectors.setPrimitiveType(sf::PrimitiveType::Lines);

	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Tile& t = m_grid[y][x];
			if (t.type == TileType::Wall)
				continue;
			sf::Vector2f start = t.shape.getPosition() + 
			                     sf::Vector2f(TILE_SIZE / 2.f, TILE_SIZE / 2.f);
			sf::Vector2f dir = t.flowVector * (TILE_SIZE / 2.f);

			sf::Vertex lineStart;
			lineStart.position = start;
			lineStart.color = sf::Color::White;

			sf::Vertex lineEnd;
			lineEnd.position = start + dir;
			lineEnd.color = sf::Color::Cyan;

			m_flowVectors.append(lineStart);
			m_flowVectors.append(lineEnd);
		}
	}
	
	updateGridColors();
}


// MOVE AGENTS - Follow the flow field
void Game::moveCharacter(sf::Time t_deltaTime)
{
	if (m_goalPos.x == -1)  
		return;

	static sf::Clock moveClock;
	static const float moveDelay = 0.1f;

	if (moveClock.getElapsedTime().asSeconds() < moveDelay)
		return;

	moveClock.restart();
	for (auto& agent : m_agents)
	{
		if (!agent.isMoving || agent.position.x == -1)
			continue;

		if (agent.position == m_goalPos)
		{
			agent.position = agent.startPosition;
			agent.isMoving = false;
			continue;
		}

		Tile& tile = m_grid[agent.position.y][agent.position.x];
		sf::Vector2f dir = tile.flowVector;

		if (dir == sf::Vector2f(0.f, 0.f))
			continue;

		sf::Vector2i next = {
			agent.position.x + static_cast<int>(std::round(dir.x)),
			agent.position.y + static_cast<int>(std::round(dir.y))
		};

		if (next.x < 0 || next.x >= GRID_WIDTH || 
		    next.y < 0 || next.y >= GRID_HEIGHT)
			continue;

		m_grid[agent.position.y][agent.position.x].isOnPath = true;
		
		agent.position = next;
	}

	updateGridColors();
}