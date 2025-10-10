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
    float friction = 600.f;    
    float gravity = 50.f;
    float Groundlevel = 750.f;

    int m_frameNow{ 0 };
    float m_frameCount{ 0.0f };
    float m_framePlus{ 0.2f };

    void SetupPlayer()
    {
        if (!texture.loadFromFile("ASSETS/IMAGES/Sprites/IDLE.png"))
        {
            throw std::runtime_error("Failed to load IDLE.png!");
        }

        sprite.setTexture(texture, true);
        sprite.setScale(sf::Vector2f{ 1.8f, 1.8f });

        pos = { 40.f, 750.f };
        sprite.setPosition(pos);

        auto bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.size / 2.0f);
    }

    enum class PlayerState
    {
        Idle,
        Running,
        Jumping
    };

    PlayerState state = PlayerState::Idle;

    void Jump();
    void moveLeft();
    void moveRight();

    void AnimatePlayer();

    void Update(float dt);  // <-- new: integrate velocity into pos
};
