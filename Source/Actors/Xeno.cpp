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
  ,mIsRunning(false)
  ,mForwardSpeed(400.0f)
  ,mJumpSpeed(-750.0f)
{
  mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 2.00f, true);
  mRigidBodyComponent->SetMaxSpeedX(700.0f);

  mColliderComponent = new AABBColliderComponent(this,
        0.0f, 0.0f,
        mWidth - 1, mHeight - 1, ColliderLayer::Player);

  mDrawComponent = new AnimatorComponent(
  this, "../Assets/Sprites/Xeno/Xeno.png","../Assets/Sprites/Xeno/Xeno.json",
    mWidth, mHeight
  );
  mDrawComponent->AddAnimation("idle", std::vector{1});
  mDrawComponent->AddAnimation("run", std::vector{3,4,5,6,7,8});
  mDrawComponent->SetAnimation("idle");
  mDrawComponent->SetAnimFPS(10.0f);

  // Start grounded by default for this actor
  SetOnGround();
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
  float cameraX = GetGame()->GetCameraPos().x;
  if (mPosition.x < cameraX)
  {
    mPosition.x = cameraX;
  }
  if (mPosition.y < 0) {
    Kill();
  }

  ManageAnimations();
}

void Xeno::ManageAnimations() {
  if (IsOnGround()) {
    if (mIsRunning) {
      mDrawComponent->SetAnimation("run");
      mDrawComponent->SetAnimFPS(10.0f);
    }
    else {
      mDrawComponent->SetAnimation("idle");
      mDrawComponent->SetAnimFPS(10.0f);
    }
  }
}

void Xeno::OnProcessInput(const Uint8* state) {
  if (!mRigidBodyComponent) return;

  bool isMoving = false;
  if (state[SDL_SCANCODE_D]) {
    if (mScale.x < 0.0f) {
      mScale.x *= -1.0f;
    }
    mRigidBodyComponent->ApplyForce(Vector2(mForwardSpeed, 0.0f));
    isMoving = true;
  }
  if (state[SDL_SCANCODE_A]){
    if (mScale.x > 0.0f){
      mScale.x *= -1.0f;
    }
    mRigidBodyComponent->ApplyForce(Vector2(-mForwardSpeed, 0.0f));
    isMoving = true;
  }
  if (state[SDL_SCANCODE_W]) {
    if (IsOnGround()) {
      auto velocity = mRigidBodyComponent->GetVelocity();
      mRigidBodyComponent->SetVelocity(Vector2(velocity.x, mJumpSpeed));
      SetOffGround();
    }
  }
  mIsRunning = isMoving;
  if (!mIsRunning && IsOnGround()) {
    mRigidBodyComponent->SetVelocity(Vector2(0.0f, mRigidBodyComponent->GetVelocity().y));
  }
}