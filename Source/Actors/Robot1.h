//
// Created by henrique on 11/13/25.
//

#pragma once
#include "Actor.h"

class Robot1 : public Actor {
 public:
  Robot1(Game* game);
  void OnHorizontalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;
  void OnVerticalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;
  void Kill() override;

 protected:
  void OnUpdate(float deltaTime) override;
} ;

