//
// Created by henrique on 11/20/25.
//

#include "PortalBullet.h"

#include "../../Game.h"
#include "../Drawing/AnimatorComponent.h"
PortalBullet::PortalBullet(class Game* game, PortalType portalType)
  :Bullet(game)
  ,mPortalType(portalType) {

  if (mPortalType == PortalType::BLUE) {
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/BluePortalShoot.png", "" ,
      7 * mGame->GetGameScale(), 7 * mGame->GetGameScale());
    mAnimatorComponent->SetVisible(false);
  } else if (mPortalType == PortalType::ORANGE) {
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/OrangePortalShoot.png", "" ,
      7 * mGame->GetGameScale(), 7 * mGame->GetGameScale());
    mAnimatorComponent->SetVisible(false);
  }

}

void PortalBullet::OnHorizontalCollision(const float minOverlap,
                                         AABBColliderComponent* other) {
}
void PortalBullet::OnVerticalCollision(const float minOverlap,
                                       AABBColliderComponent* other) {

}
void PortalBullet::OnUpdate(float deltaTime) {
  Bullet::OnUpdate(deltaTime);
}