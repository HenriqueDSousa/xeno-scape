#pragma once

#include "Bullet.h"
#include "Particle.h"

class ShootBullet : public Bullet {
public:
  ShootBullet(class Game* game);

  void OnHorizontalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;
  void OnVerticalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;
  void OnUpdate(float deltaTime) override;

};