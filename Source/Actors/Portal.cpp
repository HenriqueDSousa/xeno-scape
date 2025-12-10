//
// Created by henrique on 11/20/25.
//

#include "Portal.h"

#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "Blocks/Block.h"
#include "XenoGun.h"

const std::unordered_set<ColliderLayer> IGNORE_COLLISION_LAYERS = {
  ColliderLayer::Blocks,
  ColliderLayer::Portal,
  ColliderLayer::PortalBullet
};

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
    3 * mGame->GetGameScale(), 32 * mGame->GetGameScale(), ColliderLayer::Portal, false, 69);
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
  OnHorizontalCollision(minOverlap,other);
}

void Portal::SetDirection(PortalDirection direction) {
  mDirection = direction;

  if (IsVerticalDirection(direction)) {
    // Horizontal portal → wide, short
    mCollider->SetWidth(32 * mGame->GetGameScale());
    mCollider->SetHeight(3 * mGame->GetGameScale());
  } else {
    // Vertical portal → tall, thin
    mCollider->SetWidth(3 * mGame->GetGameScale());
    mCollider->SetHeight(32 * mGame->GetGameScale());
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
  return  IGNORE_COLLISION_LAYERS.find(other->GetLayer()) != IGNORE_COLLISION_LAYERS.end() ||
    mColliderCooldown > 0.0f;
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
  mColliderCooldown = M_PI/10;
  Vector2 newPosition = exitPortal->GetPosition();
  if (!IsHorizontalDirection(exitPortal->GetDirection())) {
    newPosition.y=exitPortal->GetPosition().y + (actor->GetComponent<AABBColliderComponent>()->mHeight/2 + OFFSET_AMOUNT) * (exitPortal->GetDirection()==PortalDirection::UP ? -1 : 1);
  }
  else {
    newPosition.x=exitPortal->GetPosition().x + (actor->GetComponent<AABBColliderComponent>()->mWidth/2 + OFFSET_AMOUNT) * (exitPortal->GetDirection()==PortalDirection::LEFT ? -1 : 1);
  }
  actor->SetPosition(newPosition);
  SetCooldown(COLLIDER_COOLDOWN_TIME);
  exitPortal->SetCooldown(COLLIDER_COOLDOWN_TIME);
  mGame->GetAudio()->PlaySound("PortalTeleport.wav", false);

  auto* rigidBody = actor->GetComponent<RigidBodyComponent>();
  if (rigidBody) {
    Vector2 newVelocity = ConvertVelocity(rigidBody->GetVelocity(), exitPortal->GetDirection(),actor);
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

Vector2 Portal::ConvertVelocity(const Vector2& v, PortalDirection exitDir, Actor *actor) const
{
    PortalDirection in = mDirection;
    PortalDirection out = exitDir;

    bool entryIsVertical = IsVerticalDirection(in);
    bool exitIsVertical  = IsVerticalDirection(out);

    // --- 1. Compute rotated/redirected velocity ---
    Vector2 outVel;

    if (IsHorizontalDirection(mDirection) == IsHorizontalDirection(exitDir)) {
        // First case: Parallel (same or opposite)
        outVel = v;
    }
    else {
      if (entryIsVertical && !exitIsVertical) {
        // entry: UP/DOWN (vertical) -> exit: LEFT/RIGHT (horizontal)
        // Incoming vertical speed (v.y) becomes horizontal output.
        float mappedX = (out == PortalDirection::RIGHT) ? Math::Abs(v.y) : -Math::Abs(v.y);
        // The remaining component on y should come from the incoming x.
        outVel = Vector2(mappedX, v.x);
      } else if (!entryIsVertical && exitIsVertical) {
        // entry: LEFT/RIGHT → exit: UP/DOWN

        // Preserve facing direction for the new horizontal component:
        float facing = actor->GetScale().x;               // +right, −left
        float mappedX = Math::Abs(v.y) * (facing >= 0 ? 1.0f : -1.0f);

        // Vertical component still based on incoming horizontal velocity:
        float mappedY =
            (out == PortalDirection::DOWN) ? Math::Abs(v.x) : -Math::Abs(v.x);

        outVel = Vector2(mappedX, mappedY);
      }
    }

    // --- 2. Scale resulting velocity ---
    outVel *= VELOCITY_SCALE_FACTOR;

    // --- 3. Enforce MIN_OUT_VELOCITY depending on exit direction ---
  auto clampMin = [&](float vel, PortalDirection dir) -> float {
    switch (dir) {
      case PortalDirection::RIGHT: return (vel <  MIN_OUT_VELOCITY) ?  MIN_OUT_VELOCITY : vel;
      case PortalDirection::LEFT:  return (vel > -MIN_OUT_VELOCITY) ? -MIN_OUT_VELOCITY : vel;
      case PortalDirection::DOWN:  return (vel <  MIN_OUT_VELOCITY) ?  MIN_OUT_VELOCITY : vel;
      case PortalDirection::UP:    return (vel > -MIN_OUT_VELOCITY) ? -MIN_OUT_VELOCITY : vel;
    }
    return vel;
  };

  if (IsHorizontalDirection(out))
    outVel.x = clampMin(outVel.x, out);
  else
    outVel.y = clampMin(outVel.y, out);

    return outVel;
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

inline Vector2 Portal::DirectionToUnitVector(PortalDirection dir) const {
  switch (dir) {
    case PortalDirection::UP:    return Vector2(0.0f, -1.0f);
    case PortalDirection::DOWN:  return Vector2(0.0f,  1.0f);
    case PortalDirection::LEFT:  return Vector2(-1.0f, 0.0f);
    case PortalDirection::RIGHT: return Vector2( 1.0f, 0.0f);
  }
  return Vector2::Zero;
}

Vector2 Portal::ApplyExitTransform(const Vector2& v, PortalDirection exitDir) const {
  PortalDirection in = mDirection;
  PortalDirection out = exitDir;

  // --- PARALLEL (same or opposite): velocity unchanged ---
  if ((in == out) ||
      (in == PortalDirection::RIGHT && out == PortalDirection::LEFT) ||
      (in == PortalDirection::LEFT && out == PortalDirection::RIGHT) ||
      (in == PortalDirection::UP && out == PortalDirection::DOWN) ||
      (in == PortalDirection::DOWN && out == PortalDirection::UP)) {
    return v;  // <--- FIXED
  }

  // --- PERPENDICULAR: compute ±90° rotation ---
  bool rotateCCW = false;

  switch (in) {
    case PortalDirection::RIGHT:
      rotateCCW = (out == PortalDirection::UP);
      break;
    case PortalDirection::UP:
      rotateCCW = (out == PortalDirection::LEFT);
      break;
    case PortalDirection::LEFT:
      rotateCCW = (out == PortalDirection::DOWN);
      break;
    case PortalDirection::DOWN:
      rotateCCW = (out == PortalDirection::RIGHT);
      break;
  }

  if (rotateCCW)
    return Vector2(-v.y, v.x);  // +90°
  else
    return Vector2(v.y, -v.x);  // -90°
}
