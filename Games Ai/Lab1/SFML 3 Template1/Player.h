#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#pragma once
class player
{
public:
    
    sf::Texture texture;
    sf::Sprite sprite{ texture };
    sf::Vector2f pos;
    float speed = 2.0f;
    float rotation = 0.0f; 

    void SetupPlayer()
    {
        if (!texture.loadFromFile("ASSETS/IMAGES/Ship.png"))
        {
            throw std::runtime_error("Failed to load Ship.png!");
        }

        sprite.setTexture(texture, true);
        sprite.setScale(sf::Vector2f{ 0.5f, 0.5f });

        pos.x = 100;
        pos.y = 300;

        sprite.setPosition(pos);

        auto bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.size / 2.0f);
        
    }

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
};
