//
// Created by Lucas N. Ferreira on 09/11/25.
//

#include "Particle.h"

Particle::Particle(class Game* game)
        : Actor(game)
        , mIsDead(true)
        , mLifeTime(1.0f)
{
  SetState(ActorState::Paused);
}

void Particle::Kill()
{
  mIsDead = true;
  SetState(ActorState::Paused);
}

void Particle::Awake(const Vector2 &position, const float &rotation, float lifetime)
{
  mLifeTime = lifetime;
  mIsDead = false;

  SetState(ActorState::Active);
  SetPosition(position);
  SetRotation(rotation);
}

void Particle::Emit(const Vector2 &direction, float speed)
{

}

void Particle::OnUpdate(float deltaTime)
{
  // Update lifetime
  mLifeTime -= deltaTime;
  if (mLifeTime <= 0)
  {
    Kill();
    return;
  }
}