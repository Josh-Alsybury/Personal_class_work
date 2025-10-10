#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#pragma once
class player
{
public:
    sf::Texture texture;
    sf::Sprite sprite{ texture };
    sf::Vector2f pos;
    sf::Vector2f velocity;

    bool isOnGround = true;

    float speed = 200.f;       
    float maxSpeed = 550.f;    
    float friction = 500.f;    
    float gravity = 9.80665f;
    float Groundlevel = 750.f;

    void SetupPlayer()
    {
        if (!texture.loadFromFile("ASSETS/IMAGES/Ship.png"))
        {
            throw std::runtime_error("Failed to load Ship.png!");
        }

        sprite.setTexture(texture, true);
        sprite.setScale(sf::Vector2f{ 0.2f, 0.2f });

        pos = { 40.f, 750.f };
        sprite.setPosition(pos);

        auto bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.size / 2.0f);
    }

    void Jump();

    void moveLeft();
    void moveRight();

    void Update(float dt);  // <-- new: integrate velocity into pos
};
