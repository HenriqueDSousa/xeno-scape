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
  ,mWidth(3)
  ,mHeight(32)
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
    3 * mGame->GetGameScale(), 32 * mGame->GetGameScale(), ColliderLayer::Portal, false);
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

void Portal::SetDirection(PortalDirection direction) {
  mDirection = direction;
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
  Vector2 newPosition = AddOffset(exitPortal->GetPosition(), exitPortal->GetDirection());
  actor->SetPosition(newPosition);
  SetCooldown(COLLIDER_COOLDOWN_TIME);
  exitPortal->SetCooldown(COLLIDER_COOLDOWN_TIME);

  auto* rigidBody = actor->GetComponent<RigidBodyComponent>();
  if (rigidBody) {
    Vector2 newVelocity = ConvertVelocity(rigidBody->GetVelocity(), exitPortal->GetDirection());
    rigidBody->SetVelocity(newVelocity);

    if (ShouldFlipScale(exitPortal)) {
      Vector2 actorScale = actor->GetScale();
      actor->SetScale(Vector2(actorScale.x * -1.0f, actorScale.y));
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
  return IsHorizontalDirection(mDirection) && IsHorizontalDirection(exitPortal->GetDirection())
  && mDirection == exitPortal->GetDirection();
}

Vector2 Portal::ConvertVelocity(const Vector2& velocity, PortalDirection exitDirection) const {
  bool entryIsVertical = IsVerticalDirection(mDirection);
  bool exitIsVertical = IsVerticalDirection(exitDirection);

  // Vertical entry to horizontal exit
  if (entryIsVertical && !exitIsVertical) {
    float xVel = (exitDirection == PortalDirection::RIGHT) ? Math::Abs(velocity.y) : -Math::Abs(velocity.y);
    if (xVel == 0) xVel = (exitDirection == PortalDirection::RIGHT) ? MIN_OUT_VELOCITY : -MIN_OUT_VELOCITY;
    return Vector2(xVel * VELOCITY_SCALE_FACTOR, velocity.x);
  }

  // Horizontal entry to vertical exit
  if (!entryIsVertical && exitIsVertical) {
    float yVel = (exitDirection == PortalDirection::DOWN) ? Math::Abs(velocity.x) : -Math::Abs(velocity.x);
    if (yVel == 0) yVel = (exitDirection == PortalDirection::DOWN) ? MIN_OUT_VELOCITY : -MIN_OUT_VELOCITY;
    return Vector2(velocity.x, yVel * VELOCITY_SCALE_FACTOR);
  }

  // Both vertical: flip Y if same direction
  if (entryIsVertical) {
    if (mDirection == exitDirection) {
      float yVel = -velocity.y * VELOCITY_SCALE_FACTOR;
      if (yVel == 0) yVel = (exitDirection == PortalDirection::DOWN) ? MIN_OUT_VELOCITY : -MIN_OUT_VELOCITY;
      return Vector2(0.0f, yVel);
    }
  }

  // Both horizontal: flip X if same direction
  if (!entryIsVertical) {
    if (mDirection == exitDirection) {
      float xVel = -velocity.x * VELOCITY_SCALE_FACTOR;
      if (xVel == 0) xVel = (exitDirection == PortalDirection::RIGHT) ? MIN_OUT_VELOCITY : -MIN_OUT_VELOCITY;
      return Vector2(xVel, 0.0f);
    }
  }

  return velocity;
}

Vector2 Portal::AddOffset(const Vector2& position, PortalDirection direction) const {
  switch (direction) {
    case PortalDirection::RIGHT:
      return Vector2(position.x + OFFSET_AMOUNT * mGame->GetGameScale(), position.y);
    case PortalDirection::LEFT:
      return Vector2(position.x - OFFSET_AMOUNT * mGame->GetGameScale(), position.y);
    case PortalDirection::UP:
      return Vector2(position.x, position.y - OFFSET_AMOUNT * mGame->GetGameScale());
    case PortalDirection::DOWN:
      return Vector2(position.x, position.y + OFFSET_AMOUNT * mGame->GetGameScale());
    default:
      return position;
  }
}