//
// Created by henrique on 11/20/25.
//

#include "Portal.h"

#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "Blocks/Block.h"
#include "XenoGun.h"
Portal::Portal(Game* game, XenoGun* owner, Vector2 position, PortalType portalType)
  :Actor(game)
  ,mPortalType(portalType)
  ,mOwner(owner)
  ,mColliderCooldown(0.0f)
{
  SetPosition(position);
  if (mPortalType == PortalType::ORANGE) {
    mAnimator = new AnimatorComponent(this, "../Assets/Sprites/OrangePortal.png", "",
      3 * mGame->GetGameScale(), 32 * mGame->GetGameScale());
    mAnimator->SetVisible(false);
  } else if (mPortalType == PortalType::BLUE) {
    mAnimator = new AnimatorComponent(this, "../Assets/Sprites/BluePortal.png", "",
      3 * mGame->GetGameScale(), 32 * mGame->GetGameScale());
    mAnimator->SetVisible(false);
  }
  mCollider = new AABBColliderComponent(this,
    0.0f, 0.0f,
    16 * mGame->GetGameScale(), 32 * mGame->GetGameScale(), ColliderLayer::Portal, false);
}

void Portal::Kill() {

}


void Portal::OnHorizontalCollision(const float minOverlap,
                                   AABBColliderComponent* other) {

  if (other->GetLayer() == ColliderLayer::Blocks || mColliderCooldown > 0.0f) {
    return;
  }

  auto* otherActor = other->GetOwner();
  auto* rigidBody = otherActor->GetComponent<RigidBodyComponent>();
  mColliderCooldown = 1.0f;
  if (mPortalType == PortalType::ORANGE) {
    auto* bluePortal = mOwner->GetActiveBluePortal();
    if (bluePortal && mOwner->IsBluePortalActive()) {
      otherActor->SetPosition(bluePortal->GetPosition());
      otherActor->SetScale(otherActor->GetScale() * -1.0f);
      bluePortal->SetCooldown(COLLIDER_COOLDOWN_TIME);
      if (rigidBody) {
        Vector2 velocity = rigidBody->GetVelocity();
        rigidBody->SetVelocity(Vector2(-velocity.x, velocity.y));
      }
    }
  } else if (mPortalType == PortalType::BLUE) {
    auto* orangePortal = mOwner->GetActiveOrangePortal();
    if (orangePortal && mOwner->IsOrangePortalActive()) {
      otherActor->SetPosition(orangePortal->GetPosition());
      otherActor->SetScale(otherActor->GetScale() * -1.0f);
      orangePortal->SetCooldown(COLLIDER_COOLDOWN_TIME);
      if (rigidBody) {
        Vector2 velocity = rigidBody->GetVelocity();
        rigidBody->SetVelocity(Vector2(-velocity.x, velocity.y));
      }
    }
  }
}

void Portal::OnVerticalCollision(const float minOverlap,
                                 AABBColliderComponent* other) {

  if (other->GetLayer() == ColliderLayer::Blocks || mColliderCooldown > 0.0f) {
    return;
  }

  auto* otherActor = other->GetOwner();
  auto* rigidBody = otherActor->GetComponent<RigidBodyComponent>();
  mColliderCooldown = 1.0f;
  if (mPortalType == PortalType::ORANGE) {
    auto* bluePortal = mOwner->GetActiveBluePortal();
    if (bluePortal && mOwner->IsBluePortalActive()) {
      otherActor->SetPosition(bluePortal->GetPosition());
      otherActor->SetScale(otherActor->GetScale() * -1.0f);
      bluePortal->SetCooldown(COLLIDER_COOLDOWN_TIME);
      if (rigidBody) {
        Vector2 velocity = rigidBody->GetVelocity();
        rigidBody->SetVelocity(Vector2(-velocity.x, velocity.y));
      }
    }
  } else if (mPortalType == PortalType::BLUE) {
    auto* orangePortal = mOwner->GetActiveOrangePortal();
    if (orangePortal && mOwner->IsOrangePortalActive()) {
      otherActor->SetPosition(orangePortal->GetPosition());
      otherActor->SetScale(otherActor->GetScale() * -1.0f);
      orangePortal->SetCooldown(COLLIDER_COOLDOWN_TIME);
      if (rigidBody) {
        Vector2 velocity = rigidBody->GetVelocity();
        rigidBody->SetVelocity(Vector2(-velocity.x, velocity.y));
      }
    }
  }
}

void Portal::SetActive(bool active) {
  SetState(active == true ? ActorState::Active : ActorState::Paused);
  mAnimator->SetVisible(active);
  mCollider->SetEnabled(active);
}

void Portal::OnUpdate(float deltaTime) {

  mColliderCooldown -= deltaTime;
  if (mColliderCooldown <= 0.0f) {
    mColliderCooldown = 0.0f;
    mCollider->SetEnabled(true);
  } else {
    mCollider->SetEnabled(false);
  }

}