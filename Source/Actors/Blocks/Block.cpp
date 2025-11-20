//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Block.h"
#include "../../Game.h"
#include "../../Components/Drawing/AnimatorComponent.h"
#include "../../Components/Physics/AABBColliderComponent.h"

Block::Block(Game* game, const std::string &texturePath)
        : Actor(game)
{
    mDrawComponent = new AnimatorComponent(
        this, texturePath,"",
        game->GetTileSize(), game->GetTileSize()
        );
    mColliderComponent = new AABBColliderComponent(this,
        0.0f, 0.0f,
        game->GetTileSize(), game->GetTileSize(), ColliderLayer::Blocks, true);
    mGame->AddBlock(this);
}
