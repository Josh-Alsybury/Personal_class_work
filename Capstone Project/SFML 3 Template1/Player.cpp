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
    int frame = 0; int const FRAME_TALL = 64;
    m_frameCount += m_framePlus;
    frame = static_cast<int>(m_frameCount);
    frame = frame % 8;
    if (frame != m_frameNow)
    {
        sf::IntRect rect;
        rect.position.x = 96 * frame; 
        rect.position.y = 0;           
        rect.size.x = 96;          
        rect.size.y = 96;

        sprite.setTextureRect(rect);
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