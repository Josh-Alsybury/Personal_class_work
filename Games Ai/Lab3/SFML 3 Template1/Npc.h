#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include "Player.h"
#pragma once

struct  SterringOutput
{
	sf::Vector2f linear;
	float angualr = 0.0f;
};

class Npc
{
public:
	sf::Texture texture;
	sf::Sprite sprite{ texture };
	sf::Vector2f pos;

	sf::Vector2f velocity{ 0.f, 0.f };   
	sf::Vector2f acceleration{ 0.f, 0.f }; 
	float maxSpeed = 150.0f;             
	float maxAcceleration = 100.0f;
	float rotation = 0.0f;

	float angularVelocity = 0.0f;
	float maxRotation = 180.0f;      
	float maxAngularAcc = 90.0f;
	float wanderAngle = 0.f;

	float visionLength = 150.f;
	float visionAngle = 60.f;

	void SetupNpc()
	{
		if (!texture.loadFromFile("ASSETS/IMAGES/Alien.png"))
		{
			throw std::runtime_error("Failed to load Ship.png!");
		}

		sprite.setTexture(texture, true);
		sprite.setScale(sf::Vector2f{ 0.15f, 0.15f });

		pos = { 800, 300 };
		velocity = { 0, 0 };

		sprite.setPosition(pos);

		auto bounds = sprite.getLocalBounds();
		sprite.setOrigin(bounds.size / 2.0f);
	}


	 
	SterringOutput Seek(const sf::Vector2f& playerPos); //1 npc
	SterringOutput Arrive(const sf::Vector2f& playerPos, float slowRadius, float arrivalRadius); // 2 npcs 
	SterringOutput Wander();//1 npc
	SterringOutput pursue(const sf::Vector2f& playerPos, const sf::Vector2f& targetVelocity); // 1 npc
	SterringOutput Swarm(const std::vector<Npc>& all, size_t selfIndex, float neighborRadius, float A, float B, int N, int M);



	sf::ConvexShape Npc::getVisionCone(const sf::Vector2f& playerPos);
	bool isPlayerInVision(const sf::Vector2f& playerPos);
	void Update(const SterringOutput& steering, float deltaTime);
	void wrapAround(sf::Vector2f& pos, float screenWidth, float screenHeight);

private:
	float MapToRange(float angle);

	
};