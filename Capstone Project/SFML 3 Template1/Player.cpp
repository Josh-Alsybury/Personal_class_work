#include "Player.h"
#include <cmath> 



void player::moveLeft()
{
    velocity.x -= speed; // apply acceleration left
}

void player::moveRight()
{
    velocity.x += speed; // apply acceleration right
}

void player::Update(float dt)
{

    if (velocity.x > maxSpeed) velocity.x = maxSpeed;
    if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;

    pos += velocity * dt;
    sprite.setPosition(pos);

    if (velocity.x > 0)
        velocity.x -= friction * dt;
    else if (velocity.x < 0)
        velocity.x += friction * dt;

    if (std::abs(velocity.x) < 1.f)
        velocity.x = 0.f;
}