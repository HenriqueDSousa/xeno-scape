//
// Created by henrique on 11/13/25.
//

#include "Xeno.h"

#include <SDL.h>

#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Game.h"
#include "../UI/Screens/GameOver.h"
#include "XenoArm.h"
#include "XenoGun.h"

Xeno::Xeno(Game* game, float width, float height)
  :Actor(game)
  ,mWidth(width)
  ,mHeight(height)
  ,mIsRunning(false)
  ,mForwardSpeed(400.0f)
  ,mJumpSpeed(-750.0f)
  ,mXenoState(Alive)
{
  mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 2.00f, true);
  mRigidBodyComponent->SetMaxSpeedX(700.0f);

  mColliderComponent = new AABBColliderComponent(this,
        0.0f, 0.0f,
        mWidth - 1, mHeight - 1, ColliderLayer::Player);

  mDrawComponent = new AnimatorComponent(
  this, "../Assets/Sprites/Xeno/Xeno.png","../Assets/Sprites/Xeno/Xeno.json",
    mWidth, mHeight, 100
  );

  mAimArm = new XenoArm(game, this);
  mXenoGun = new XenoGun(game, this);

  mDrawComponent->AddAnimation("idle", std::vector{0});
  mDrawComponent->AddAnimation("aim", std::vector{1});
  mDrawComponent->AddAnimation("run", std::vector{2,3,4,5,6,7});
  mDrawComponent->AddAnimation("death", std::vector{8,9,10});
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

void Xeno::Kill() {
  mXenoState = Dead;
  mDrawComponent->SetAnimation("death");
  mDrawComponent->SetAnimFPS(3.0f);
  mRigidBodyComponent->SetEnabled(false);
  mColliderComponent->SetEnabled(false);
  mGame->GetHud()->SetPaused(true);
}

void Xeno::OnUpdate(float deltaTime) {
  if (mGame->IsPositionOutOfBounds(mPosition)) {
    Kill();
  }

  float cameraX = GetGame()->GetCameraPos().x;
  if (mPosition.x < cameraX)
  {
    mPosition.x = cameraX;
  }

  if (mXenoState == Dead) {
    mDeathTimer -= deltaTime;
    if (mDeathTimer > 0.0f) return;

    mDrawComponent->SetIsPaused(true);
    mDrawComponent->SetVisible(false);
    SetState(ActorState::Paused);
    new GameOver(mGame,"../Assets/Fonts/SMB.ttf");
    return;
  }

  ManageAnimations();
}

void Xeno::ManageAnimations() {
  if (mIsAiming) {
    mDrawComponent->SetAnimation("aim");
    mDrawComponent->SetAnimFPS(10.0f);
    return;
  }
  if (IsOnGround()) {
    if (mIsRunning) {
      mDrawComponent->SetAnimation("run");
      mDrawComponent->SetAnimFPS(10.0f);
    } else {
      mDrawComponent->SetAnimation("idle");
      mDrawComponent->SetAnimFPS(10.0f);
    }
  }
}

void Xeno::OnProcessInput(const Uint8* state) {
  if (!mRigidBodyComponent || mXenoState == Dead) return;

  bool isAiming = false;
  bool isMoving = false;

  int mouseX = 0, mouseY = 0;
  Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
  if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
    isAiming = true;
  }
  mIsAiming = isAiming;

  if (!mIsAiming) {
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