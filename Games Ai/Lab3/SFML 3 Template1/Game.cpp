/// <summary>
/// author Pete Lowe May 2025
/// you need to change the above line or lose marks
/// </summary>
#include "Game.h"
#include <iostream>


/// <summary>
/// default constructor
/// setup the window properties
/// load and setup the texts
/// load and setup the images
/// load and setup the sounds
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ sf::Vector2u{1000U, 800U}, 32U }, "SFML Game 3.0" },
	m_DELETEexitGame{false} //when true game will exit
{
	setupSprites(); // load texture
	m_Player.SetupPlayer();
	initNPCs();
	setupTexts();   // load font
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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) m_visible[0] = !m_visible[0];
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) m_visible[1] = !m_visible[1];
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) m_visible[2] = !m_visible[2];
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4)) m_visible[3] = !m_visible[3];
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5)) m_visible[4] = !m_visible[4];
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

	sf::Vector2f direction{ 0.0f, 0.0f };


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
	{
		m_Player.moveUp();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
	{
		m_Player.moveDown();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		m_Player.moveLeft();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		m_Player.moveRight();
	}
	

	float dt = t_deltaTime.asSeconds();
	m_Player.Update(dt);

	if (m_npcs.empty()) return;

	for (size_t i = 0; i < m_npcs.size(); ++i)
	{
		if (!m_visible[i]) continue;
		
		SterringOutput swarmSteering = m_npcs[i].Swarm(m_npcs, i,700.f,1100.f,500.f,2,1);
		m_npcs[i].Update(swarmSteering, dt);
		
		m_npcs[i].wrapAround(m_npcs[i].pos, 1000U, 800U);

		m_npcTexts[i].setPosition(sf::Vector2f{ m_npcs[i].pos.x, m_npcs[i].pos.y - 30.f });
	}


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
	m_window.clear();

	m_window.draw(m_Player.sprite);

	for (size_t i = 0; i < m_npcs.size(); ++i)
	{
		if (m_visible[i]) 
		{
			//cone = m_npcs[i].getVisionCone(m_Player.pos);
			//m_window.draw(cone);
			m_window.draw(m_npcs[i].sprite);
			m_window.draw(m_npcTexts[i]);
		}
	}

	m_window.display();
}

/// <summary>
/// load the font and setup the text message for screen
/// </summary>
void Game::setupTexts()
{
	if (!m_jerseyFont.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
	{
		std::cout << "Error loading font!\n";
	}

	std::vector<std::string> labels = {
		"Key 1 Seek",
	};

	for (size_t i = 0; i < m_npcs.size(); ++i)
	{
		sf::Text text(m_jerseyFont);

		if (i < labels.size())
			text.setString(labels[i]);    // first 5 NPCs get custom labels
		else
			text.setString("Wander");     // all others default to Wander

		text.setCharacterSize(18);
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(2.f);

		m_npcTexts.push_back(std::move(text));
	}
}



/// <summary>
/// load the texture and setup the sprite for the logo
/// </summary>
void Game::setupSprites()
{

}

/// <summary>
/// load sound file and assign buffers
/// </summary>
void Game::setupAudio()
{

}

void Game::initNPCs()
{
	size_t npcCount = 50;
	m_npcs.resize(npcCount);
	m_visible.assign(npcCount, true);

	for (size_t i = 0; i < m_npcs.size(); ++i)
	{
		m_npcs[i].SetupNpc();

		
		float startX = 100.f + static_cast<float>(rand() % 800);
		float startY = 100.f + static_cast<float>(rand() % 600);
		m_npcs[i].pos = { startX, startY };
		m_npcs[i].sprite.setPosition(m_npcs[i].pos); 
	}
}
