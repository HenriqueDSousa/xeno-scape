//
// Created by henrique on 11/23/25.
//

#include "EndLevelBlock.h"

#include "../../Components/Physics/AABBColliderComponent.h"
#include "../../Game.h"
EndLevelBlock::EndLevelBlock(Game* game, const std::string& texturePath)
  :Block(game, texturePath) {
  endZone = new AABBColliderComponent(this,
        0.0f, 0.0f,
        game->GetTileSize()+1, game->GetTileSize()+1, ColliderLayer::Blocks, true);
  endZone->SetEnabled(false);
}

void EndLevelBlock::OnUpdate(float deltaTime) {
  auto comp=mGame->GetPlayer()->GetComponent<AABBColliderComponent>();
  if (comp && comp->Intersect(*endZone)) {
    mGame->SetNextScene();
  }
}
