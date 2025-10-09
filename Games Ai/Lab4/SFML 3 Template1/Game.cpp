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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) m_formationMode = !m_formationMode;
	
		

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

	if (m_formationMode)
	{
		updateFormation(dt);
	}
	else
	{
		for (auto& npc : m_npcs)
		{
			SterringOutput steering = npc.Wander();
			npc.Update(steering, dt);
			npc.wrapAround(npc.pos, 1000U, 800U);
		}
	}



	if (m_DELETEexitGame)
	{
		m_window.close();
	}
}

void Game::updateFormation(float dt)
{
	float radians = m_Player.rotation * (3.14159f / 180.0f);
	sf::Vector2f forward(-std::cos(radians), -std::sin(radians));
	sf::Vector2f right(forward.y, -forward.x);

	

	for (size_t i = 0; i < m_npcs.size() && (i + 1) < m_formationOffsets.size(); ++i)
	{
		sf::Vector2f slotPos = m_Player.pos +right * m_formationOffsets[i + 1].x +forward * m_formationOffsets[i + 1].y;
		slotCircle[i].setFillColor(sf::Color::Green);
		slotCircle[i].setRadius(5.5f);
		slotCircle[i].setPosition(slotPos);
		
		////////////
	
		//m_npcs[i].pos = slotPos;
		//m_npcs[i].rotation = m_Player.rotation;


		// dk if its meant to be like this ////

		SterringOutput steering = m_npcs[i].ArriveToSlot(slotPos, 200, 5);
		m_npcs[i].Update(steering, dt);
		m_npcs[i].wrapAround(m_npcs[i].pos, 1000U, 800U);
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
			m_window.draw(m_npcs[i].sprite);
			m_window.draw(slotCircle[i]);
		}
	}
	
	m_window.draw(m_formationHintText);
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
	

	m_formationHintText.setFont(m_jerseyFont);          
	m_formationHintText.setString("Press F to toggle Formation");
	m_formationHintText.setCharacterSize(24);
	m_formationHintText.setFillColor(sf::Color::White);
	m_formationHintText.setOutlineColor(sf::Color::Black);
	m_formationHintText.setOutlineThickness(2.f);
	m_formationHintText.setPosition({ 20.f, 10.f });

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
	m_npcs.resize(4);
	m_visible = { true, true, true, true, true };

	for (size_t i = 0; i < m_npcs.size(); ++i)
	{
		m_npcs[i].SetupNpc();
	}

	m_npcs[0].pos = { 200.f, 100.f };
	m_npcs[1].pos = { 400.f, 200.f };
	m_npcs[2].pos = { 600.f, 200.f };
	m_npcs[3].pos = { 800.f, 100.f };
}
