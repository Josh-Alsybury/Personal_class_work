#include "Player.h"
#include <cmath> 

void player::moveUp()
{
    float radians = rotation * (3.14159f / 180.0f);
    velocity.x = std::cos(radians) * speed;
    velocity.y = std::sin(radians) * speed;
}

void player::moveDown()
{
    float radians = rotation * (3.14159f / 180.0f);
    velocity.x = -std::cos(radians) * speed;
    velocity.y = -std::sin(radians) * speed;

    sprite.setPosition(pos);
}

void player::moveLeft()
{
    rotation -= 2.0f;
    sprite.setRotation(sf::degrees(rotation));
}

void player::moveRight()
{
    rotation += 2.0f;
    sprite.setRotation(sf::degrees(rotation));
}

void player::Update(float dt)
{
    pos += velocity * dt;
    sprite.setPosition(pos);

    velocity = { 0.f, 0.f };
}
