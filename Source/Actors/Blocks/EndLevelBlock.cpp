//
// Created by henrique on 11/23/25.
//

#include "EndLevelBlock.h"

#include "../../Components/Physics/AABBColliderComponent.h"
#include "../../Game.h"
EndLevelBlock::EndLevelBlock(Game* game, const std::string& texturePath)
  :Block(game, texturePath) {
  mColliderComponent->SetHeight(game->GetTileSize() + 1);
  mColliderComponent->SetWidth(game->GetTileSize() + 1);
}

void EndLevelBlock::OnHorizontalCollision(const float minOverlap,
                                          AABBColliderComponent* other) {
  if (other->GetLayer() == ColliderLayer::Player) {
    mGame->SetNextScene();
  }
}
void EndLevelBlock::OnVerticalCollision(const float minOverlap,
                                        AABBColliderComponent* other) {
  if (other->GetLayer() == ColliderLayer::Player) {
    mGame->SetNextScene();
  }
}