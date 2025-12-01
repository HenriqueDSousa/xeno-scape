//
// Created by henrique on 11/20/25.
//

#pragma once
#include "Bullet.h"

enum class PortalType {
  BLUE,
  ORANGE
};

enum class PortalDirection {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

class XenoGun;

class PortalBullet : public Bullet {
 public:
  PortalBullet(class Game* game, PortalType portalType);

  void OnHorizontalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;
  void OnVerticalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;
  void OnUpdate(float deltaTime) override;

  void SetGun(XenoGun* gun) { mGun = gun; }

  void FixInitialOverlap();

private:
  void SpawnPortal(float rotation, PortalDirection direction, float minOverlap) const;

 PortalType mPortalType;
  XenoGun* mGun;
};

// Wrapper classes for ParticleSystemComponent compatibility
class BluePortalBullet : public PortalBullet {
public:
  BluePortalBullet(class Game* game) : PortalBullet(game, PortalType::BLUE) {}
};

class OrangePortalBullet : public PortalBullet {
public:
  OrangePortalBullet(class Game* game) : PortalBullet(game, PortalType::ORANGE) {}
};

