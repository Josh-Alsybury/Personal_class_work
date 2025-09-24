#include "Npc.h"


SterringOutput Npc::Seek(const sf::Vector2f& playerPos)
{
	SterringOutput steering;

    sf::Vector2f direction = playerPos - pos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0)
        direction /= length;

    sf::Vector2f targetVelocity = direction * maxSpeed;
    steering.linear = targetVelocity - velocity;

    float steerLen = std::sqrt(steering.linear.x * steering.linear.x + steering.linear.y * steering.linear.y);
    if (steerLen > maxAcceleration)
        steering.linear *= (maxAcceleration / steerLen);

    if (velocity.x != 0 || velocity.y != 0)
    {
        float angleRadians = std::atan2(velocity.y, velocity.x);
        sprite.setRotation(sf::degrees(angleRadians * 180.f / 3.14159f));
    }

	return steering;
}

SterringOutput Npc::Wander()
{
    wanderAngle += ((rand() % 100) / 100.f - 0.5f) * 0.3f;

	SterringOutput steering;
    
    sf::Vector2f desriedDiection(std::cos(wanderAngle), std::sin(wanderAngle));
    sf::Vector2f targetVelocity = desriedDiection * maxSpeed;

    steering.linear = targetVelocity - velocity;

    float steerLen = std::sqrt(steering.linear.x * steering.linear.x + steering.linear.y * steering.linear.y);
    if (steerLen > maxAcceleration)
        steering.linear *= (maxAcceleration / steerLen);

    if (velocity.x != 0 || velocity.y != 0)
    {
        float angleRadians = std::atan2(velocity.y, velocity.x);
        sprite.setRotation(sf::degrees(angleRadians * 180.f / 3.14159f));
    }

	return steering;
}

SterringOutput Npc::Arrive(const sf::Vector2f& playerPos, float slowRadius, float arrivalRadius)
{
	SterringOutput steering;

    sf::Vector2f direction = playerPos - pos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance > 0)
        direction /= distance;

    float targetSpeed = 0.0f;
    if (distance < arrivalRadius)
        targetSpeed = 0.f;
    else if (distance > slowRadius)
        targetSpeed = maxSpeed;
    else
        targetSpeed = maxSpeed * (distance / slowRadius);

    sf::Vector2f targetVelocity = direction * targetSpeed;
    steering.linear = targetVelocity - velocity;


    float steerLen = std::sqrt(steering.linear.x * steering.linear.x + steering.linear.y * steering.linear.y);
    if (steerLen > maxAcceleration)
        steering.linear *= (maxAcceleration / steerLen);

    if (velocity.x != 0 || velocity.y != 0)
    {
        float angleRadians = std::atan2(velocity.y, velocity.x);
        sprite.setRotation(sf::degrees(angleRadians * 180.f / 3.14159f));
    }

	return steering;
}

SterringOutput Npc::pursue(const sf::Vector2f& playerPos, const sf::Vector2f& targetVelocity)
{
	SterringOutput steering;

    sf::Vector2f direction = playerPos - pos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    float predictTime = distance / (maxSpeed + 0.1f);
    sf::Vector2f predictTarget = playerPos + targetVelocity * predictTime;

    sf::Vector2f toPredict = predictTarget - pos;
    float length = std::sqrt(toPredict.x * toPredict.x + toPredict.y * toPredict.y);
    if (length > 0)
        toPredict /= length;

    sf::Vector2f targetVel = toPredict * maxSpeed;
    steering.linear = targetVel - velocity;

    float steerLen = std::sqrt(steering.linear.x * steering.linear.x + steering.linear.y * steering.linear.y);
    if (steerLen > maxAcceleration)
        steering.linear *= (maxAcceleration / steerLen);

    if (velocity.x != 0 || velocity.y != 0)
    {
        float angleRadians = std::atan2(velocity.y, velocity.x);
        sprite.setRotation(sf::degrees(angleRadians * 180.f / 3.14159f));
    }

	return steering;
}

sf::ConvexShape Npc::getVisionCone()
{
    const float coneAngle = 60.f;    
    const float coneLength = 150.f;  

    
    float facingAngle = sprite.getRotation().asDegrees() * (3.14159f / 180.f);

    float leftAngle = facingAngle - (coneAngle * 0.5f) * (3.14159f / 180.f);
    float rightAngle = facingAngle + (coneAngle * 0.5f) * (3.14159f / 180.f);

    sf::ConvexShape cone;
    cone.setPointCount(3);
    cone.setPoint(0, pos);
    cone.setPoint(1, pos + sf::Vector2f(std::cos(leftAngle), std::sin(leftAngle)) * coneLength);
    cone.setPoint(2, pos + sf::Vector2f(std::cos(rightAngle), std::sin(rightAngle)) * coneLength);

    cone.setFillColor(sf::Color(255, 255, 0, 100)); 
    return cone;
}


void Npc::Update(const SterringOutput& steering, float deltaTime)
{
    velocity += steering.linear * deltaTime;

    float velLen = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (velLen > maxSpeed)
        velocity *= (maxSpeed / velLen);

    pos += velocity * deltaTime;
    sprite.setPosition(pos);
}

void Npc::wrapAround(sf::Vector2f& pos, float screenWidth, float screenHeight)
{
 if (pos.x < 0) pos.x = screenWidth;
 if (pos.x > screenWidth) pos.x = 0;
 if (pos.y < 0) pos.y = screenHeight;
 if (pos.y > screenHeight) pos.y = 0;
}

float Npc::MapToRange(float angle)
{
	while (angle > 180.f) angle -= 360.f;
	while (angle < -180.f) angle += 360.f;
	return angle;
}




