#include "Player.h"
#include <cmath> 

void player::Jump()
{
    if (isOnGround)
        velocity.y -= 100;
}

void player::moveLeft()
{
    velocity.x -= speed;
}

void player::moveRight()
{
    velocity.x += speed;
}

void player::Update(float dt)
{
    // Clamp horizontal velocity
    if (velocity.x > maxSpeed) velocity.x = maxSpeed;
    if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;

    // Apply gravity (always pulling down)
    velocity.y += gravity * dt;

    // Apply friction only when grounded
    if (isOnGround)
    {
        if (velocity.x > 0)
            velocity.x -= friction * dt;
        else if (velocity.x < 0)
            velocity.x += friction * dt;

        // Snap small velocities to zero
        if (std::abs(velocity.x) < 1.f)
            velocity.x = 0.f;
    }

    // Update position
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