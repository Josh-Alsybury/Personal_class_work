#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#pragma once
class Npc
{
public:
	sf::Texture texture;
	sf::Sprite sprite{ texture };
	sf::Vector2f pos;
	float speed = 2.0f;
	float rotation = 0.0f;

	void SetupNpc()
	{
		if (!texture.loadFromFile("ASSETS/IMAGES/Alien.png"))
		{
			throw std::runtime_error("Failed to load Ship.png!");
		}

		sprite.setTexture(texture, true);
		sprite.setScale(sf::Vector2f{ 0.5f, 0.5f });

		pos.x = 800;
		pos.y = 300;

		sprite.setPosition(pos);

		auto bounds = sprite.getLocalBounds();
		sprite.setOrigin(bounds.size / 2.0f);
	}

	void move(sf::Vector2f direction, sf::Time deltaTime)
	{
		
	}



	void NpcMove(const sf::Vector2f& playerPos);

private:

	

};

