//
// Created by henrique on 11/28/25.
//

#include "PortalDisabledBlock.h"

#include "../../Components/Physics/AABBColliderComponent.h"
PortalDisabledBlock::PortalDisabledBlock(Game* game,
                                         const std::string& texturePath)
  :Block(game, texturePath) {
  mColliderComponent->SetLayer(ColliderLayer::PortalDisabled);
}