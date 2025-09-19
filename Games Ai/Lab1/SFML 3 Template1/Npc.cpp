#include "Npc.h"

void Npc::NpcMove(const sf::Vector2f& playerPos)
{
    sf::Vector2f toPlayer = playerPos - pos;

    float angleRadians = std::atan2(toPlayer.y, toPlayer.x);
    rotation = angleRadians * 180.0f / 3.14159f;
    sprite.setRotation(sf::degrees(rotation));


    float length = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
    if (length != 0)
    {
        toPlayer /= length; 
    }

    pos += toPlayer * speed;
    sprite.setPosition(pos);
}