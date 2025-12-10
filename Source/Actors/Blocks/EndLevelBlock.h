//
// Created by henrique on 11/23/25.
//

#pragma once
#include "Block.h"

class EndLevelBlock : public Block {
 public:
  EndLevelBlock(Game* game, const std::string& texturePath);
  void OnUpdate(float deltaTime) override;
private:
  AABBColliderComponent *endZone;
};

