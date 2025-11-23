//
// Created by henrique on 11/13/25.
//

#pragma once
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Game.h"
#include "Actor.h"

class MeleeRobot : public Actor {
  constexpr static float HIT_COOLDOWN = 1.0f;
  constexpr static float INTANGIBILITY_COOLDOWN = 1.0f;
  constexpr static float HIT_DURATION_TIMER = 0.2f;
  constexpr static float MOVE_SPEED = 200.0f;
  constexpr static float EDGE_CHECK_DISTANCE = 10.0f;

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
  bool CanSeePlayer();
  bool IsGroundAhead(float direction);
  void MoveTowardsPlayer(float deltaTime);

  int mHealth;
  float mHitCooldown;
  float mIntangibilityTimer;
  float mHitDurationTimer;

  float mWidth;
  float mHeight;
  bool mIsMoving;
  float mPatrolDirection; // 1.0f for right, -1.0f for left

  class AABBColliderComponent* mCollider;
  class AnimatorComponent* mAnimator;
  class RigidBodyComponent* mRigidBody;

} ;

