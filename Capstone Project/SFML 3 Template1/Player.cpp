#include "Player.h"
#include <cmath> 

void player::Jump()
{
    if (isOnGround)
        velocity.y -= 80;
}

void player::moveLeft()
{
    velocity.x -= speed;
}

void player::moveRight()
{
    velocity.x += speed;
}

void player::AnimatePlayer()
{
    const int FRAME_WIDTH = 96;
    const int FRAME_HEIGHT = 96;
    const int FRAME_COUNT = 8;

    m_frameCount += m_framePlus;
    int frame = static_cast<int>(m_frameCount) % FRAME_COUNT;

    if (frame != m_frameNow)
    {
        m_frameNow = frame;

        sf::IntRect rect;
        rect.position = { FRAME_WIDTH * frame, 0 };      // replaces left/top
        rect.size = { FRAME_WIDTH, FRAME_HEIGHT };       // replaces width/height

        sprite.setTextureRect(rect);

        // ✅ use rect.size in SFML 3
        sprite.setOrigin(sf::Vector2f(rect.size.x / 2.f, rect.size.y / 2.f));
    }
}

void player::Update(float dt)
{
    AnimatePlayer();

    if (velocity.x > maxSpeed) velocity.x = maxSpeed;
    if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;


    velocity.y += gravity * dt;

    if (isOnGround)
    {
        if (velocity.x > 0)
            velocity.x -= friction * dt;
        else if (velocity.x < 0)
            velocity.x += friction * dt;

        if (std::abs(velocity.x) < 1.f)
            velocity.x = 0.f;
    }

    pos += velocity * dt;
    sprite.setPosition(pos);

    if (pos.y >= Groundlevel)
    {
        pos.y = Groundlevel;
        velocity.y = 0;
        isOnGround = true;
    }
    else
    {
        isOnGround = false;
    }
}