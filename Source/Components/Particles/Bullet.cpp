//
// Created by Lucas N. Ferreira on 04/11/25.
//

#include "Bullet.h"

#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/AABBColliderComponent.h"
#include "../Physics/RigidBodyComponent.h"

Bullet::Bullet(Game* game)
        :Particle(game)
{
  mRigidBody = new RigidBodyComponent(this, 1.0f, 0.0f);
  mRigidBody->SetVelocity(Vector2::Zero);
  mRigidBody->SetApplyGravity(false);
  mCollisionComponent = new AABBColliderComponent(this, 0.0f, 0.0f, 5.0f * mGame->GetGameScale(), 5.0f * mGame->GetGameScale(), ColliderLayer::Bullet);
}

Bullet::~Bullet()
{
}

void Bullet::Kill()
{
  mAnimatorComponent->SetVisible(false);
  mCollisionComponent->SetEnabled(false);
  mRigidBody->SetVelocity(Vector2::Zero);
  Particle::Kill();
}

void Bullet::Awake(const Vector2 &position, const float &rotation, float lifetime)
{
  Particle::Awake(position, rotation, lifetime);

  mCollisionComponent->SetEnabled(true);
  mAnimatorComponent->SetVisible(true);
}

void Bullet::Emit(const Vector2 &direction, float speed)
{
  mRigidBody->SetVelocity(direction * speed);
  mScale.x = direction.x > 0? 1: -1;
}

void Bullet::OnUpdate(float deltaTime)
{
  Particle::OnUpdate(deltaTime);
}

void Bullet::StartGraceTime() {
  mGraceTime=0.051;
}

bool Bullet::InGraceTime() {
  return mGraceTime>0;
}
