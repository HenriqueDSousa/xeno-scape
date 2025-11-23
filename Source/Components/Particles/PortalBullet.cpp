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
    PortalDirection direction;
    if (mRigidBody->GetVelocity().x > 0.0f) {
      rotation = 0.0f;
      direction = PortalDirection::RIGHT;
    } else {
      rotation = Math::Pi;
      direction = PortalDirection::LEFT;
    }
    SpawnPortal(rotation, direction);
    Kill();
  } else {
    Kill();
  }
}

void PortalBullet::OnVerticalCollision(const float minOverlap,
                                       AABBColliderComponent* other) {
  if (other->GetLayer() == ColliderLayer::Blocks) {
    float rotation = 0.0f;
    PortalDirection direction;
    if (mRigidBody->GetVelocity().y > 0.0f) {
      rotation = Math::PiOver2;
      direction = PortalDirection::DOWN;
    } else {
      rotation = -Math::PiOver2;
      direction = PortalDirection::UP;
    }
    SpawnPortal(rotation, direction);
    Kill();
  } else {
    Kill();
  }
}

void PortalBullet::OnUpdate(float deltaTime) {
  Bullet::OnUpdate(deltaTime);
}

void PortalBullet::SpawnPortal(float rotation, PortalDirection direction) const {
  if (!mGun) return;
  
  Vector2 portalPosition = GetPosition();
  
  if (mPortalType == PortalType::BLUE) {
    auto* portal = mGun->GetActiveBluePortal();
    if (portal) {
      portal->SetPosition(portalPosition);
      portal->SetRotation(rotation);
      portal->SetDirection(direction);
      portal->SetActive(true);
      mGun->SetBluePortalActive(true);
    }
  } else if (mPortalType == PortalType::ORANGE) {
    auto* portal = mGun->GetActiveOrangePortal();
    if (portal) {
      portal->SetPosition(portalPosition);
      portal->SetRotation(rotation);
      portal->SetDirection(direction);
      portal->SetActive(true);
      mGun->SetOrangePortalActive(true);
    }
  }
}