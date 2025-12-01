//
// Created by henrique on 11/16/25.
//

#include "XenoArm.h"

#include "../Game.h"
#include <algorithm>
#include <cmath>

XenoArm::XenoArm(Game* game, Xeno* owner)
  :Actor(game)
  ,mOwner(owner)
{
  // Define arm size (width = arm length, height = thickness)
  mArmWidth = mGame->GetGameScale() * 8.0f;
  mArmHeight = mGame->GetGameScale() * 2.0f;

  // Render arm slightly in front of the player (default player draw order is 100)
  mDrawComponent = new AnimatorComponent(
      this, "../Assets/Sprites/Xeno/XenoArm.png","",
      static_cast<int>(mArmWidth), static_cast<int>(mArmHeight), 99
  );
}
void XenoArm::OnUpdate(float deltaTime) {
  if (mOwner->IsAiming()) {
    mDrawComponent->SetVisible(true);
  } else {
    mDrawComponent->SetVisible(false);
  }
  // Keep arm scale in sync with owner so flip is honored by the draw component
  SetScale(mOwner->GetScale());
  // Update the sprite center so the arm visually pivots at the shoulder
  const Vector2 shoulderPos = ComputeShoulderPos();
  UpdateCenterFromRotation(GetRotation(), shoulderPos);
}

void XenoArm::OnProcessInput(const Uint8* keyState) {
  int mouseX = 0, mouseY = 0;
  Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
  // Only allow aiming/rotation while the owner is in aiming state
  if (!mOwner || !mOwner->IsAiming()) {
    return;
  }

  if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
    // Convert mouse screen coordinates to world coordinates
    Vector2 mouseWorld(static_cast<float>(mouseX) + mGame->GetCameraPos().x,
                       static_cast<float>(mouseY) + mGame->GetCameraPos().y);
    // Compute pivot (shoulder) and angle relative to that pivot
    const Vector2 shoulderPos = ComputeShoulderPos();
    const float angleRad = atan2f(mouseWorld.y - shoulderPos.y, mouseWorld.x - shoulderPos.x);

    // Facing correction: if flipped, treat facing as PI
    const float facing = (mOwner->GetScale().x < 0.0f) ? M_PI : 0.0f;

    // Localize, clamp, and reapply facing
    float localAngle = angleRad - facing;
    // Normalize angle to [-PI, PI] range
    while (localAngle > M_PI) localAngle -= 2.0f * M_PI;
    while (localAngle < -M_PI) localAngle += 2.0f * M_PI;
    const float clampedLocal = std::clamp(localAngle, kMinAngleRad, kMaxAngleRad);
    const float finalAngle = clampedLocal + facing;

    // Apply rotation and update center so the arm pivots at the shoulder
    SetRotation(finalAngle);
    UpdateCenterFromRotation(finalAngle, shoulderPos);
  }
}

Vector2 XenoArm::ComputeShoulderPos() const
{
  const float offsetX = mOwner->GetWidth() - 11.0f * mGame->GetGameScale();
  const float shoulderX = (mOwner->GetScale().x < 0.0f) ? (mOwner->GetPosition().x - offsetX) : (mOwner->GetPosition().x + offsetX);
  const float shoulderY = mOwner->GetPosition().y + 0.5f * mGame->GetGameScale();
  return Vector2(shoulderX, shoulderY);
}

void XenoArm::UpdateCenterFromRotation(float angle, const Vector2& shoulderPos)
{
  const float halfW = mArmWidth * 0.5f;
  const float cosA = cosf(angle);
  const float sinA = sinf(angle);
  Vector2 rotatedHalf;
  rotatedHalf.x = cosA * halfW;
  rotatedHalf.y = sinA * halfW;
  SetPosition(shoulderPos + rotatedHalf);
}

Vector2 XenoArm::GetHandPosition() const
{
  // The hand is at the tip of the arm, which is the full arm length from the shoulder
  const Vector2 shoulderPos = ComputeShoulderPos();
  const float angle = GetRotation();
  const float cosA = cosf(angle);
  const float sinA = sinf(angle);
  
  // Calculate the tip position by extending the full arm length from shoulder
  Vector2 handPos;
  handPos.x = shoulderPos.x + cosA * mArmWidth;
  handPos.y = shoulderPos.y + sinA * mArmWidth;
  
  return handPos;
}
