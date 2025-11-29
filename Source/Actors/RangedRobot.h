//
// Created by henrique on 11/28/25.
//

#pragma once
#include "../Components/Particles/ParticleSystemComponent.h"
#include "../Components/Particles/ShootBullet.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "Actor.h"

class RangedRobot : public Actor {
  constexpr static float HIT_COOLDOWN = 1.0f;
  constexpr static float INTANGIBILITY_COOLDOWN = 1.0f;
  constexpr static float HIT_DURATION_TIMER = 0.2f;
  constexpr static float SHOOT_COOLDOWN = 3.0f;
  constexpr static float BURST_DELAY = 0.3f; // Time between shots in burst
  constexpr static int BURST_COUNT = 3; // Number of bullets per burst

public:
  RangedRobot(Game* game, float width, float height);
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
  void Shoot();
  void AimAtPlayer();
  
  int mHealth;
  float mHitCooldown;
  float mIntangibilityTimer;
  float mHitDurationTimer;
  float mShootCooldown;
  float mBurstTimer;
  int mBurstShotsRemaining;

  float mWidth;
  float mHeight;
  float mAimAngle;

  class AABBColliderComponent* mCollider;
  class AnimatorComponent* mAnimator;
  class RigidBodyComponent* mRigidBody;
  ParticleSystemComponent<ShootBullet>* mGun;
};