//
// Created by henrique on 11/23/25.
//

#pragma once
#include "Block.h"

class EndLevelBlock : public Block {
 public:
  EndLevelBlock(Game* game, const std::string& texturePath);
  void OnHorizontalCollision(const float minOverlap,
                             AABBColliderComponent* other) override;
  void OnVerticalCollision(const float minOverlap,
                           AABBColliderComponent* other) override;

};

