//
// Created by henrique on 11/20/25.
//

#include "PortalBullet.h"

#include "../../Actors/Portal.h"
#include "../../Actors/XenoGun.h"
#include "../../Game.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/AABBColliderComponent.h"
#include "../Physics/RigidBodyComponent.h"

PortalBullet::PortalBullet(class Game* game, PortalType portalType)
  :Bullet(game)
  ,mPortalType(portalType)
  ,mGun(nullptr) {

  if (mPortalType == PortalType::BLUE) {
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/BluePortalShoot.png", "" ,
      7 * mGame->GetGameScale(), 7 * mGame->GetGameScale(), 0);
    mAnimatorComponent->SetVisible(false);
  } else if (mPortalType == PortalType::ORANGE) {
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/OrangePortalShoot.png", "" ,
      7 * mGame->GetGameScale(), 7 * mGame->GetGameScale(), 0);
    mAnimatorComponent->SetVisible(false);
  }

}

void PortalBullet::OnHorizontalCollision(const float minOverlap,
                                         AABBColliderComponent* other) {
  if (other->GetLayer() == ColliderLayer::Blocks) {
    float rotation = 0.0f;
    if (mRigidBody->GetVelocity().x > 0.0f) {
      rotation = 0.0f;
    } else {
      rotation = Math::Pi;
    }
    SpawnPortal(rotation);
    Kill();
  } else {
    Kill();
  }
}

void PortalBullet::OnVerticalCollision(const float minOverlap,
                                       AABBColliderComponent* other) {
  if (other->GetLayer() == ColliderLayer::Blocks) {
    float rotation = 0.0f;
    if (mRigidBody->GetVelocity().y > 0.0f) {
      rotation = Math::PiOver2;
    } else {
      rotation = -Math::PiOver2;
    }
    SpawnPortal(rotation);
    Kill();
  } else {
    Kill();
  }
}

void PortalBullet::OnUpdate(float deltaTime) {
  Bullet::OnUpdate(deltaTime);
}

void PortalBullet::SpawnPortal(float rotation) const {
  if (!mGun) return;
  
  Vector2 portalPosition = GetPosition();
  
  if (mPortalType == PortalType::BLUE) {
    auto* portal = mGun->GetActiveBluePortal();
    if (portal) {
      portal->SetPosition(portalPosition);
      portal->SetRotation(rotation);
      portal->SetActive(true);
      mGun->SetBluePortalActive(true);
    }
  } else if (mPortalType == PortalType::ORANGE) {
    auto* portal = mGun->GetActiveOrangePortal();
    if (portal) {
      portal->SetPosition(portalPosition);
      portal->SetRotation(rotation);
      portal->SetActive(true);
      mGun->SetOrangePortalActive(true);
    }
  }
}