#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#pragma once
class player
{
public:
    sf::Texture idleTexture;
    sf::Texture runTexture;
    std::unique_ptr<sf::Sprite> sprite;
    sf::Vector2f pos;
    sf::Vector2f velocity;

    bool isOnGround = true;

    float speed = 200.f;       
    float maxSpeed = 550.f;    
    float friction = 600.f;    
    float gravity = 50.f;
    float Groundlevel = 740.f;

    int m_frameNow{ 0 };
    float m_frameCount{ 0.0f };
    float m_framePlus{ 0.2f };

    void SetupPlayer()
    {
        if (!idleTexture.loadFromFile("ASSETS/IMAGES/Sprites/IDLE.png"))
        {
            throw std::runtime_error("Failed to load IDLE.png!");
        }
        if (!runTexture.loadFromFile("ASSETS/IMAGES/Sprites/RUN.png"))
        {
            throw std::runtime_error("Failed to load IDLE.png!");
        }

        idleAnim.texture = &idleTexture;
        idleAnim.frameCount = 10;
        idleAnim.frameWidth = 96;
        idleAnim.frameHeight = 96;

        runAnim.texture = &runTexture;
        runAnim.frameCount = 16; 
        runAnim.frameWidth = 96;
        runAnim.frameHeight = 96;


        sprite = std::make_unique<sf::Sprite>(idleTexture);
        sprite->setScale(sf::Vector2f(1.8f, 1.8f));

        pos = { 90.f, 750.f };
        sprite->setPosition(pos);
        sprite->setTextureRect(sf::IntRect{ {0, 0}, {96, 96} });
        sprite->setOrigin(sf::Vector2f(48.f, 48.f)); 
    }

    enum class PlayerState
    {
        Idle,
        Running,
        Jumping
    };

    struct Animation {
        sf::Texture* texture;
        int frameCount;
        int frameWidth;
        int frameHeight;
    };

    Animation idleAnim;
    Animation runAnim;
    Animation* currentAnim;
    bool facingRight = true;

    player()
    {
        pos = { 90.f, 750.f };
        velocity = { 0.f, 0.f };
       
    }

    PlayerState state = PlayerState::Idle;

    void Jump();
    void moveLeft();
    void moveRight();

    void AnimatePlayer();
    void UpdateAnimationTexture();

    void Update(float dt); 
};


