#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#pragma once
class player
{
public:
    sf::Texture texture;
    sf::Sprite sprite{ texture };
    sf::Vector2f pos;
    sf::Vector2f velocity{ 0.f, 0.f };   // <-- new velocity
    float speed = 200.0f;              // max speed (pixels/sec)
    float rotation = 0.0f;

    void SetupPlayer()
    {
        if (!texture.loadFromFile("ASSETS/IMAGES/Ship.png"))
        {
            throw std::runtime_error("Failed to load Ship.png!");
        }

        sprite.setTexture(texture, true);
        sprite.setScale(sf::Vector2f{ 0.2f, 0.2f });

        pos = { 100.f, 300.f };
        sprite.setPosition(pos);

        auto bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.size / 2.0f);
    }

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    void Update(float dt);  // <-- new: integrate velocity into pos
};
