

#include "MeleeRobot.h"

#include "../Components/Physics/AABBColliderComponent.h"
#include "../Game.h"
#include "Xeno.h"
MeleeRobot::MeleeRobot(Game* game,  float width, float height)
  :Actor(game)
  ,mWidth(width)
  ,mHeight(height)
  ,mHealth(3)
  ,mHitCooldown(HIT_COOLDOWN)
  ,mIntangibilityTimer(INTANGIBILITY_COOLDOWN)
  ,mIsMoving(false)
  ,mPatrolDirection(1.0f)
  ,mHitDurationTimer(0.0f)
{

  mRigidBody = new RigidBodyComponent(this, 1.0f, 2.00f, true);
  mRigidBody->SetMaxSpeedX(700.0f);

  mCollider = new AABBColliderComponent(this,
        0.0f, 0.0f,
        mWidth - 1, mHeight - 1, ColliderLayer::Enemy);

  mAnimator = new AnimatorComponent(
  this, "../Assets/Sprites/MeleeRobot/MeleeRobot.png","../Assets/Sprites/MeleeRobot/MeleeRobot.json",
    mWidth, mHeight, 100);
  mAnimator->AddAnimation("idle", std::vector{0,1,2,3});
  mAnimator->AddAnimation("walk", std::vector{4,5,6,7,8,9,10,11});
  mAnimator->AddAnimation("hit", std::vector{12});
  mAnimator->SetAnimation("idle");
  mAnimator->SetAnimFPS(10.0f);

}

void MeleeRobot::OnHorizontalCollision(const float minOverlap,
                                       AABBColliderComponent* other) {

  switch (other->GetLayer()) {
    case ColliderLayer::Player:
      mHitCooldown = HIT_COOLDOWN;
      other->GetOwner()->Kill();
      break;
    case ColliderLayer::Bullet:
      Kill();
      other->GetOwner()->Kill();
      break;
    case ColliderLayer::Blocks:
      mPatrolDirection *= -1.0f;
      break;
    default:
      break;
  }
}
void MeleeRobot::OnVerticalCollision(const float minOverlap,
                                     AABBColliderComponent* other) {
  Actor::OnVerticalCollision(minOverlap, other);
}

void MeleeRobot::Kill() {
  mHealth--;
  if (mHealth == 0) {
    mGame->GetAudio()->PlaySound("Explosion.wav", false);
    SetState(ActorState::Destroy);
    return;
  } else {
    mAnimator->SetAnimation("hit");
    mHitDurationTimer = 0.2f;
    mGame->GetAudio()->PlaySound("Hit.wav", false);
  }
  mIntangibilityTimer = INTANGIBILITY_COOLDOWN;
}

void MeleeRobot::OnUpdate(float deltaTime) {

  if (mGame->IsPositionOutOfBounds(mPosition)) {
    SetState(ActorState::Destroy);
    return;
  }

  mHitCooldown -= deltaTime;
  if (mHitCooldown <= 0) {
    mHitCooldown = 0.0f;
  }

  mIntangibilityTimer -= deltaTime;
  if (mIntangibilityTimer <= 0) {
  mIntangibilityTimer = 0.0f;
  }
  mHitDurationTimer -= deltaTime;
  if (mHitDurationTimer <= 0.0f) {
    mHitDurationTimer = 0.0f;
    if (CanSeePlayer()) {
      MoveTowardsPlayer(deltaTime);
    } else {
      if (IsOnGround()) {
        // Check for edge ahead
        if (!IsGroundAhead(mPatrolDirection)) {
          mPatrolDirection *= -1.0f;
        }

        // Update facing direction
        if ((mPatrolDirection > 0.0f && mScale.x < 0.0f) ||
          (mPatrolDirection < 0.0f && mScale.x > 0.0f)) {
          mScale.x *= -1.0f;
        }

        // Apply patrol movement
        mRigidBody->ApplyForce(Vector2(mPatrolDirection * MOVE_SPEED * 0.5f, 0.0f));
        mIsMoving = true;
      } else {
        mIsMoving = false;
      }
    }
  } else {
    // Stop moving when hit
    mRigidBody->SetVelocity(Vector2(0.0f, mRigidBody->GetVelocity().y));
    mIsMoving = false;
  }
  
  ManageAnimations();
}

void MeleeRobot::ManageAnimations() {
  if (mHitDurationTimer > 0.0f) {
    return;
  }

  if (mIsMoving && IsOnGround()) {
    mAnimator->SetAnimation("walk");
    mAnimator->SetAnimFPS(10.0f);
  } else {
    mAnimator->SetAnimation("idle");
    mAnimator->SetAnimFPS(10.0f);
  }
}

bool MeleeRobot::CanSeePlayer() {
  Xeno* player = mGame->GetPlayer();
  if (player == nullptr) {
    return false;
  }

  if (player->IsDead()) {
    return false;
  }

  Vector2 toPlayer = player->GetPosition() - mPosition;
  float distance = toPlayer.Length();

  return distance <= mGame->GetTileSize() * 8;
}

bool MeleeRobot::IsGroundAhead(float direction) {
  if (!IsOnGround()) {
    return true;
  }

  // Use patrol direction for consistent edge checking
  float checkDirection = direction;

  Vector2 checkPos = mPosition;
  checkPos.x += checkDirection * (mWidth / 2.0f + EDGE_CHECK_DISTANCE);
  checkPos.y += mHeight / 2.0f + 10.0f;

  auto& colliders = mGame->GetColliders();
  for (auto collider : colliders) {
    if (collider->GetLayer() == ColliderLayer::Blocks) {
      Vector2 colliderMin = collider->GetMin();
      Vector2 colliderMax = collider->GetMax();

      if (checkPos.x >= colliderMin.x && checkPos.x <= colliderMax.x &&
          checkPos.y >= colliderMin.y && checkPos.y <= colliderMax.y) {
        return true;
      }
    }
  }
  
  return false;
}

void MeleeRobot::MoveTowardsPlayer(float deltaTime) {
  auto actors = mGame->GetActors();
  Xeno* player = nullptr;
  
  for (auto actor : actors) {
    player = dynamic_cast<Xeno*>(actor);
    if (player != nullptr) {
      break;
    }
  }
  
  if (player == nullptr) {
    return;
  }

  Vector2 toPlayer = player->GetPosition() - mPosition;
  float directionX = (toPlayer.x > 0.0f) ? 1.0f : -1.0f;

  if (directionX > 0.0f && mScale.x < 0.0f) {
    mScale.x *= -1.0f;
  } else if (directionX < 0.0f && mScale.x > 0.0f) {
    mScale.x *= -1.0f;
  }

  if (IsGroundAhead(directionX)) {
    mRigidBody->ApplyForce(Vector2(directionX * MOVE_SPEED, 0.0f));
    mIsMoving = true;
  } else {
    mRigidBody->SetVelocity(Vector2(0.0f, mRigidBody->GetVelocity().y));
    mIsMoving = false;
  }
}