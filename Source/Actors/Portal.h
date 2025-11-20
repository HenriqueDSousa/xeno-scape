//
// Created by henrique on 11/20/25.
//
#pragma once
#include "../Components/Particles/PortalBullet.h"
#include "Actor.h"

enum class PortalType;
class Portal : public Actor {

  constexpr static float COLLIDER_COOLDOWN_TIME = 1.0f;

 public:
  Portal(Game* game, XenoGun* owner, Vector2 position, PortalType portalType);
  void Kill() override;
  void OnHorizontalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;
  void OnVerticalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;
  void SetActive(bool active);

  void OnUpdate(float deltaTime) override;

  void SetCooldown(float cooldown) { mColliderCooldown = cooldown; }

private:
  PortalType mPortalType;
  XenoGun* mOwner;
  float mColliderCooldown;

  AABBColliderComponent* mCollider;
  AnimatorComponent* mAnimator;
};

// Portal wrapper classes
class BluePortal : public Portal {
public:
  BluePortal(class Game* game, XenoGun* owner, const Vector2 position) : Portal(game, owner, position, PortalType::BLUE) {}
};

class OrangePortal : public Portal {
public:
  OrangePortal(class Game* game, XenoGun* owner, const Vector2 position) : Portal(game, owner, position, PortalType::ORANGE) {}
};