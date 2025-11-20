//
// Created by henrique on 11/13/25.
//

#pragma once
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "Actor.h"

class MeleeRobot : public Actor {
  constexpr static float HIT_COOLDOWN = 1.0f;
  constexpr static float INTANGIBILITY_COOLDOWN = 1.0f;
  constexpr static float HIT_DURATION_TIMER = 0.2f;

 public:
  MeleeRobot(Game* game,  float width, float height);
  void OnHorizontalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;
  void OnVerticalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;
  void Kill() override;

 protected:
  void OnUpdate(float deltaTime) override;

private:
  void ManageAnimations();

  int mHealth;
  float mHitCooldown;
  float mIntangibilityTimer;
  float mHitDurationTimer;

  float mWidth;
  float mHeight;

  class AABBColliderComponent* mCollider;
  class AnimatorComponent* mAnimator;
  class RigidBodyComponent* mRigidBody;

} ;

