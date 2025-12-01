//
// Created by henrique on 11/16/25.
//

#include "../Components/Drawing/AnimatorComponent.h"
#include "Actor.h"
#include "Xeno.h"

// Clamp angles to +/- 45 degrees
#pragma once
static constexpr float kMinAngleRad = -75.0f * (M_PI / 180.0f);
#pragma once
static constexpr float kMaxAngleRad = 75.0f * (M_PI / 180.0f);

class XenoArm : public Actor {
 public:
  XenoArm(Game* game, Xeno* owner);
  void OnUpdate(float deltaTime) override;
  void OnProcessInput(const Uint8* keyState) override;
  
  //Returns the position of the hand (tip of the arm) for gun placement
  Vector2 GetHandPosition() const;

  Vector2 ComputeShoulderPos() const;

private:
  AnimatorComponent* mDrawComponent;
  Xeno* mOwner;
  // Arm size in world units (used to compute pivot)
  float mArmWidth;
  float mArmHeight;
  // Helpers for pivot math
  void UpdateCenterFromRotation(float angle, const Vector2& shoulderPos);
};

