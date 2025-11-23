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
  ,mDirection(PortalDirection::RIGHT)
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
  if (ShouldIgnoreCollision(other)) {
    return;
  }

  Portal* exitPortal = GetLinkedPortal();
  if (exitPortal) {
    TeleportActor(other->GetOwner(), exitPortal);
  }
}

void Portal::OnVerticalCollision(const float minOverlap,
                                 AABBColliderComponent* other) {
  if (ShouldIgnoreCollision(other)) {
    return;
  }

  Portal* exitPortal = GetLinkedPortal();
  if (exitPortal) {
    TeleportActor(other->GetOwner(), exitPortal);
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
  }

  mCollider->SetEnabled(mColliderCooldown <= 0.0f);
}

bool Portal::ShouldIgnoreCollision(AABBColliderComponent* other) const {
  return other->GetLayer() == ColliderLayer::Blocks || mColliderCooldown > 0.0f;
}

Portal* Portal::GetLinkedPortal() const {
  if (mPortalType == PortalType::ORANGE) {
    auto* bluePortal = mOwner->GetActiveBluePortal();
    return (bluePortal && mOwner->IsBluePortalActive()) ? bluePortal : nullptr;
  } else {
    auto* orangePortal = mOwner->GetActiveOrangePortal();
    return (orangePortal && mOwner->IsOrangePortalActive()) ? orangePortal : nullptr;
  }
}

void Portal::TeleportActor(Actor* actor, Portal* exitPortal) {
  mColliderCooldown = 1.0f;
  actor->SetPosition(exitPortal->GetPosition());
  exitPortal->SetCooldown(COLLIDER_COOLDOWN_TIME);

  auto* rigidBody = actor->GetComponent<RigidBodyComponent>();
  if (rigidBody) {
    Vector2 newVelocity = ConvertVelocity(rigidBody->GetVelocity(), exitPortal->GetDirection());
    rigidBody->SetVelocity(newVelocity);

    if (ShouldFlipScale(exitPortal)) {
      actor->SetScale(actor->GetScale() * -1.0f);
    }
  }
}

bool Portal::IsHorizontalDirection(PortalDirection direction) const {
  return direction == PortalDirection::LEFT || direction == PortalDirection::RIGHT;
}

bool Portal::IsVerticalDirection(PortalDirection direction) const {
  return direction == PortalDirection::UP || direction == PortalDirection::DOWN;
}

bool Portal::ShouldFlipScale(Portal* exitPortal) const {
  return IsHorizontalDirection(mDirection) && IsHorizontalDirection(exitPortal->GetDirection());
}

Vector2 Portal::ConvertVelocity(const Vector2& velocity, PortalDirection exitDirection) const {
  bool entryIsVertical = IsVerticalDirection(mDirection);
  bool exitIsVertical = IsVerticalDirection(exitDirection);

  // Vertical entry to horizontal exit
  if (entryIsVertical && !exitIsVertical) {
    float xVel = (exitDirection == PortalDirection::RIGHT) ? Math::Abs(velocity.y) : -Math::Abs(velocity.y);
    return Vector2(xVel, velocity.x);
  }

  // Horizontal entry to vertical exit
  if (!entryIsVertical && exitIsVertical) {
    float yVel = (exitDirection == PortalDirection::DOWN) ? Math::Abs(velocity.x) : -Math::Abs(velocity.x);
    float xVel = (mDirection == PortalDirection::RIGHT) ? velocity.x : -velocity.x;
    return Vector2(xVel, yVel);
  }

  // Both vertical: flip Y if same direction
  if (entryIsVertical && exitIsVertical) {
    if (mDirection == exitDirection) {
      return Vector2(velocity.x, -velocity.y);
    }
  }

  // Both horizontal: flip X if same direction
  if (!entryIsVertical && !exitIsVertical) {
    if (mDirection == exitDirection) {
      return Vector2(-velocity.x, velocity.y);
    }
  }

  return velocity;
}