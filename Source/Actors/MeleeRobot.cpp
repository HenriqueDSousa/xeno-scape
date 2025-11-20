//
// Created by henrique on 11/13/25.
//

#include "MeleeRobot.h"

#include "../Components/Physics/AABBColliderComponent.h"
#include "Xeno.h"
MeleeRobot::MeleeRobot(Game* game,  float width, float height)
  :Actor(game)
  ,mWidth(width)
  ,mHeight(height)
  ,mHealth(3)
  ,mHitCooldown(HIT_COOLDOWN)
  ,mIntangibilityTimer(INTANGIBILITY_COOLDOWN)
{

  mRigidBody = new RigidBodyComponent(this, 1.0f, 2.00f, true);
  mRigidBody->SetMaxSpeedX(700.0f);

  mCollider = new AABBColliderComponent(this,
        0.0f, 0.0f,
        mWidth - 1, mHeight - 1, ColliderLayer::Player);

  mAnimator = new AnimatorComponent(
  this, "../Assets/Sprites/MeleeRobot/MeleeRobot.png","../Assets/Sprites/MeleeRobot/MeleeRobot.json",
    mWidth, mHeight, 100);
  mAnimator->AddAnimation("idle", std::vector{0,1,2,3});
  mAnimator->AddAnimation("walk", std::vector{4,5,6,7,8,9,10,11});
  mAnimator->AddAnimation("hit", std::vector{12});
  mAnimator->SetAnimation("idle");
  mAnimator->SetAnimFPS(10.0f);

}

void MeleeRobot::OnHorizontalCollision(const float minOverlap,
                                       AABBColliderComponent* other) {

  switch (other->GetLayer()) {
    case ColliderLayer::Player:
      mHitCooldown = HIT_COOLDOWN;
      other->GetOwner()->Kill();
      break;
    case ColliderLayer::Bullet:
      Kill();
      other->GetOwner()->Kill();
      break;
    default:
      break;
  }
}
void MeleeRobot::OnVerticalCollision(const float minOverlap,
                                     AABBColliderComponent* other) {
  Actor::OnVerticalCollision(minOverlap, other);
}

void MeleeRobot::Kill() {
  mHealth--;
  if (mHealth == 0) {
    SetState(ActorState::Destroy);
    return;
  } else {
    mAnimator->SetAnimation("hit");
    mHitDurationTimer = 0.2f;
  }
  mIntangibilityTimer = INTANGIBILITY_COOLDOWN;
}

void MeleeRobot::OnUpdate(float deltaTime) {
  mHitCooldown -= deltaTime;
  if (mHitCooldown <= 0) {
    mHitCooldown = 0.0f;
  }

  mIntangibilityTimer -= deltaTime;
  if (mIntangibilityTimer <= 0) {
  mIntangibilityTimer = 0.0f;
  }

  mHitDurationTimer -= deltaTime;
  ManageAnimations();
}

void MeleeRobot::ManageAnimations() {
  if (mHitDurationTimer > 0.0f) {
    return;
  }

  mAnimator->SetAnimation("idle");
  mAnimator->SetAnimFPS(10.0f);
}