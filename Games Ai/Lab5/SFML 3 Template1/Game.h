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


const sf::Color ULTRAMARINE{ 5, 55,242,255 }; // const colour

const int GRID_WIDTH = 50;
const int GRID_HEIGHT = 50;
const int TILE_SIZE = 16;
enum class TileType{Empty,Wall,Start,Goal};
enum class ViewMode { Normal, Cost, Integration, HeatMap, VectorField};
extern ViewMode m_viewMode;
extern sf::Font m_font;


struct Tile
{
	sf::RectangleShape shape;
	TileType type = TileType::Empty;
	int integrationCost = std::numeric_limits<int>::max(); 
	int terrainCost = 1; 
	sf::Vector2f flowVector{ 0.f, 0.f }; 

	bool isOnPath = false;
};

struct Agent
{
	sf::Vector2i position{ -1, -1 };
	sf::Vector2i startPosition{ -1, -1 };
	sf::Color color;

	bool isMoving = false;
};

class Game
{
public:
	Game();
	~Game();
	void run();

	int AgentCount = 0;
    sf::VertexArray m_flowVectors{ sf::PrimitiveType::Lines };


private:

	void processEvents();
	void processKeys(const sf::Event::KeyPressed* t_keypress);
	void checkKeyboardState();
	void update(sf::Time t_deltaTime);
	void render();
	
	void setupTexts();
	void setupSprites();
	void setupAudio();

	
	void setupGrid();
	void handleMouseInput(const sf::Event::MouseButtonPressed& event);
	void updateGridColors();
	void calculateFlowField(); 
	void moveCharacter(sf::Time t_deltaTime);

	std::vector<std::vector<Tile>> m_grid;
	sf::Vector2i m_goalPos{ -1, -1 };
	std::vector<Agent> m_agents;

	sf::RenderWindow m_window; // main SFML window
	sf::Font m_jerseyFont;// font used by message
	bool m_exitGame{ false };
	bool m_agentsMoving{ false };
};

#pragma warning( pop ) 
#endif // !GAME_HPP

