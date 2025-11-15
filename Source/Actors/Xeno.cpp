//
// Created by henrique on 11/13/25.
//

#include "Xeno.h"

#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Game.h"
Xeno::Xeno(Game* game, float width, float height)
  :Actor(game)
  ,mWidth(width)
  ,mHeight(height)
  ,mIsOnGround(true)
  ,mIsRunning(false)
{
  mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 1.25f, true);
  mRigidBodyComponent->SetMaxSpeedX(700.0f);

  mColliderComponent = new AABBColliderComponent(this,
        0.0f, 0.0f,
        mWidth - 1, mHeight - 1, ColliderLayer::Player);

  mDrawComponent = new AnimatorComponent(
  this, "../Assets/Sprites/Xeno/Xeno.png","../Assets/Sprites/Xeno/Xeno.json",
    mWidth, mHeight
  );
  mDrawComponent->AddAnimation("idle", std::vector{1});
  mDrawComponent->SetAnimation("idle");
  mDrawComponent->SetAnimFPS(10.0f);
}

void Xeno::OnHorizontalCollision(const float minOverlap,
                                 AABBColliderComponent* other) {
  Actor::OnHorizontalCollision(minOverlap, other);
}

void Xeno::OnVerticalCollision(const float minOverlap,
                               AABBColliderComponent* other) {
  Actor::OnVerticalCollision(minOverlap, other);
}

void Xeno::Kill() { Actor::Kill(); }

void Xeno::OnUpdate(float deltaTime) {
  ManageAnimations();
  Actor::OnUpdate(deltaTime);
}

void Xeno::ManageAnimations() {
  if (mIsOnGround) {
    mIsRunning ? mDrawComponent->SetAnimation("run") : mDrawComponent->SetAnimation("idle");
  }
}

void Xeno::OnProcessInput(const Uint8* keyState) {
  Actor::OnProcessInput(keyState);
}