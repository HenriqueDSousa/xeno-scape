//
// Created by henrique on 11/20/25.
//
#pragma once
#include "../Components/Particles/PortalBullet.h"
#include "Actor.h"

class Portal : public Actor {

  constexpr static float COLLIDER_COOLDOWN_TIME = 0.05f;
  constexpr static float OFFSET_AMOUNT = 3.0f;
  constexpr static float VELOCITY_SCALE_FACTOR = 1.0f;
  constexpr static float MIN_OUT_VELOCITY = 50.0f;

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
  void SetDirection(PortalDirection direction);
  PortalDirection GetDirection() const { return mDirection; }

private:
  // Collision handling helpers
  bool ShouldIgnoreCollision(AABBColliderComponent* other) const;
  Portal* GetLinkedPortal() const;
  void TeleportActor(Actor* actor, Portal* exitPortal);

  // Direction checking helpers
  bool IsHorizontalDirection(PortalDirection direction) const;
  bool IsVerticalDirection(PortalDirection direction) const;
  bool ShouldFlipScale(Portal* exitPortal) const;

  // Velocity conversion
  Vector2 ConvertVelocity(const Vector2& velocity, PortalDirection exitDirection) const;

  // Position offset to prevent double teleports
  Vector2 AddOffset(const Vector2& position, PortalDirection direction) const;
  void UpdateColliderForDirection();
  Vector2 DirectionToUnitVector(PortalDirection dir) const;
  Vector2 ApplyExitTransform(const Vector2& velocity,
                             PortalDirection exitDir) const;

  PortalType mPortalType;
  XenoGun* mOwner;
  float mColliderCooldown;
  PortalDirection mDirection;

  float mWidth;
  float mHeight;

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