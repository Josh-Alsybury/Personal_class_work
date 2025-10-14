#include "Player.h"
#include <cmath> 
#include <aubio/aubio.h>


void player::Jump()
{
    if (isOnGround)
        velocity.y -= 80;
}

void player::moveLeft()
{
    velocity.x -= speed;
    if (facingRight) {
        facingRight = false;
        sprite->setScale(sf::Vector2f( - std::abs(sprite->getScale().x), sprite->getScale().y));
    }
}

void player::moveRight()
{
    velocity.x += speed;
    if (!facingRight) {
        facingRight = true;
        sprite->setScale(sf::Vector2f(std::abs(sprite->getScale().x), sprite->getScale().y));
    }
}

void player::Attack()
{
    if(velocity.x == 0.f)
        if (isOnGround)
            isAttack = true;
}

void player::Defend()
{
    if (velocity.x == 0.f)
        if (isOnGround)
            isDefend = true;
}

void player::AnimatePlayer()
{
    UpdateAnimationTexture();

    m_frameCount += m_framePlus;
    int frame = static_cast<int>(m_frameCount) % currentAnim->frameCount;

    if (frame != m_frameNow)
    {
        m_frameNow = frame;
        sf::IntRect rect(
            sf::Vector2i(frame * currentAnim->frameWidth, 0),             
            sf::Vector2i(currentAnim->frameWidth, currentAnim->frameHeight) 
        );
        sprite->setTextureRect(rect);
        sprite->setOrigin(sf::Vector2f(currentAnim->frameWidth / 2.f, currentAnim->frameHeight / 2.f));
    }

    if (state == PlayerState::Attack && m_frameNow == attackAnim.frameCount - 1|| state == PlayerState::Defend && m_frameNow == attackAnim.frameCount - 1)
        isAttack = false;
        isDefend = false;
}

void player::UpdateAnimationTexture()
{
    Animation* newAnim = nullptr;

    switch (state)
    {
    case PlayerState::Idle:  newAnim = &idleAnim; break;
    case PlayerState::Running: newAnim = &runAnim; break;
    case PlayerState::Attack: newAnim = &attackAnim; break;
    case PlayerState::Defend: newAnim = &defendAnim; break;
    case PlayerState::Jumping: /* future jump anim */ break;
    }

    if (currentAnim != newAnim && newAnim != nullptr)
    {
        currentAnim = newAnim;
        sprite->setTexture(*currentAnim->texture, true);

        m_frameNow = 0;
        m_frameCount = 0.f;

        sf::IntRect firstFrame(
            sf::Vector2i(0, 0),
            sf::Vector2i(currentAnim->frameWidth, currentAnim->frameHeight)
        );
        sprite->setTextureRect(firstFrame);
        sprite->setOrigin(sf::Vector2f(currentAnim->frameWidth / 2.f, currentAnim->frameHeight / 2.f));
    }
}

void player::Update(float dt)
{
    if (!isOnGround)
        state = PlayerState::Jumping;
    else if (velocity.x != 0)
        state = PlayerState::Running;
    else if (isAttack == true)
        state = PlayerState::Attack;
    else if (isDefend == true)
        state = PlayerState::Defend;
    else
        state = PlayerState::Idle;

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
    sprite->setPosition(pos);

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

    sprite->setPosition(pos);
}

