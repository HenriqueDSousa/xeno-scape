//
// Created by henrique on 11/13/25.
//

#pragma once
#include "Actor.h"

class XenoArm;
enum XenoState
{
  Alive,
  Dead
};

class Xeno : public Actor {
  constexpr static float DEATH_DURATION = 1.0f;

public:

  explicit Xeno(Game* game, float width, float height);
  void OnHorizontalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;
  void OnVerticalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;
  void Kill() override;
  bool IsDead() { return mXenoState == XenoState::Dead; }

  void OnUpdate(float deltaTime) override;
  void OnProcessInput(const Uint8* keyState) override;

  float GetHeight() const { return mHeight; }
  void SetHeight(float height) { mHeight = height; }
  float GetWidth() const { return mWidth; }
  void SetWidth(float width) { mWidth = width; }

  bool IsAiming() const { return mIsAiming; }
  XenoArm* GetAimArm() const { return mAimArm; }

private:
  void ManageAnimations();

  class XenoArm* mAimArm;
  class XenoGun* mXenoGun;

  float mHeight;
  float mWidth;
  float mForwardSpeed;
  float mJumpSpeed;
  bool mIsRunning;
  bool mIsAiming;
  XenoState mXenoState;

  float mDeathTimer = DEATH_DURATION;

  class RigidBodyComponent* mRigidBodyComponent;
  class AnimatorComponent* mDrawComponent;
  class AABBColliderComponent* mColliderComponent;

};
