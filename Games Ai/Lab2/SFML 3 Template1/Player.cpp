#include "Player.h"
#include <cmath> 

void player::moveUp()
{
    float radians = rotation * (3.14159f / 180.0f);
    pos.x += std::cos(radians) * speed;
    pos.y += std::sin(radians) * speed;

    sprite.setPosition(pos);
}

void player::moveDown()
{
    float radians = rotation * (3.14159f / 180.0f);
    pos.x -= std::cos(radians) * speed;
    pos.y -= std::sin(radians) * speed;

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
