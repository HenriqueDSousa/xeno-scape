//
// Created by henrique on 11/28/25.
//

#include "RangedRobot.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Particles/Bullet.h"
#include "../Game.h"
#include "Xeno.h"

RangedRobot::RangedRobot(Game* game, float width, float height)
  :Actor(game)
  ,mWidth(width)
  ,mHeight(height)
  ,mHealth(3)
  ,mHitCooldown(HIT_COOLDOWN)
  ,mIntangibilityTimer(INTANGIBILITY_COOLDOWN)
  ,mHitDurationTimer(0.0f)
  ,mShootCooldown(0.0f)
  ,mBurstTimer(0.0f)
  ,mBurstShotsRemaining(0)
  ,mAimAngle(0.0f)
{
  mLayer = Layer::Enemy;

  mRigidBody = new RigidBodyComponent(this, 1.0f, 2.00f, true);
  mRigidBody->SetMaxSpeedX(0.0f);

  mCollider = new AABBColliderComponent(this,
        0.0f, 0.0f,
        mWidth - 1, mHeight - 1, ColliderLayer::Enemy);

  mAnimator = new AnimatorComponent(
    this, "../Assets/Sprites/RangedRobot/RangedRobot.png",
    "../Assets/Sprites/RangedRobot/RangedRobot.json",
    mWidth, mHeight, 100);
  mAnimator->AddAnimation("idle", std::vector{0,1,2});
  mAnimator->AddAnimation("hit", std::vector{8});
  mAnimator->SetAnimation("idle");
  mAnimator->SetAnimFPS(10.0f);

  mGun = new ParticleSystemComponent<ShootBullet>(this, 10);

  // Initialize aim angle based on scale direction
  mAimAngle = (mScale.x > 0.0f) ? 0.0f : Math::Pi;
}

void RangedRobot::OnHorizontalCollision(const float minOverlap,
                                        AABBColliderComponent* other) {
  switch (other->GetLayer()) {
    case ColliderLayer::Player:
      // Ranged robot doesn't damage on contact, only by shooting
      break;
    case ColliderLayer::Bullet: {
      auto bullet = dynamic_cast<Bullet*>(other->GetOwner());
      if (bullet != nullptr && bullet->InGraceTime()) {
        return;  // Ignore bullets in grace period (just spawned)
      }
      if (other->GetOwner()->GetLayer() == Layer::Enemy) return;
      Kill();
      other->GetOwner()->Kill();
      break;
    }
    case ColliderLayer::Blocks:
      // Stationary robot, no need to handle wall collision
      break;
    default:
      break;
  }
}

void RangedRobot::OnVerticalCollision(const float minOverlap,
                                      AABBColliderComponent* other) {
  Actor::OnVerticalCollision(minOverlap, other);
}

void RangedRobot::Kill() {
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

void RangedRobot::OnUpdate(float deltaTime) {
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

  mShootCooldown -= deltaTime;
  if (mShootCooldown <= 0.0f) {
    mShootCooldown = 0.0f;
  }

  mHitDurationTimer -= deltaTime;
  if (mHitDurationTimer <= 0.0f) {
    mHitDurationTimer = 0.0f;
    
    // Aim at player if visible, otherwise reset to default direction
    if (CanSeePlayer()) {
      AimAtPlayer();
    } else {
      // Reset to default shooting direction based on scale
      mAimAngle = (mScale.x > 0.0f) ? 0.0f : Math::Pi;
    }

    // Handle burst shooting
    if (mBurstShotsRemaining > 0) {
      mBurstTimer -= deltaTime;
      if (mBurstTimer <= 0.0f) {
        Shoot();
        mBurstShotsRemaining--;
        mBurstTimer = BURST_DELAY;
      }
    } else if (mShootCooldown <= 0.0f) {
      // Start new burst
      mBurstShotsRemaining = BURST_COUNT;
      mBurstTimer = 0.0f;
      mShootCooldown = SHOOT_COOLDOWN;
    }
  }
  
  ManageAnimations();
}

void RangedRobot::ManageAnimations() {
  if (mHitDurationTimer > 0.0f) {
    return;
  }

  mAnimator->SetAnimation("idle");
  mAnimator->SetAnimFPS(10.0f);
}

bool RangedRobot::CanSeePlayer() {
  Xeno* player = mGame->GetPlayer();
  if (player == nullptr) {
    return false;
  }

  if (player->IsDead()) {
    return false;
  }

  Vector2 toPlayer = player->GetPosition() - mPosition;
  float distance = toPlayer.Length();

  return distance <= mGame->GetTileSize() * 10; // Longer sight range than melee
}

void RangedRobot::AimAtPlayer() {
  Xeno* player = mGame->GetPlayer();
  if (player == nullptr) {
    return;
  }

  // Face the player
  Vector2 toPlayer = player->GetPosition() - mPosition;
  float directionX = (toPlayer.x > 0.0f) ? 1.0f : -1.0f;

  if (directionX > 0.0f && mScale.x < 0.0f) {
    mScale.x *= -1.0f;
  } else if (directionX < 0.0f && mScale.x > 0.0f) {
    mScale.x *= 1.0f;
  }

  // Calculate aim angle to face player
  mAimAngle = Math::Atan2(toPlayer.y, toPlayer.x);
}

void RangedRobot::Shoot() {
  // Calculate direction from aim angle
  Vector2 offset(0.0f, 0.0f);

  // Temporarily set rotation for emission, then restore
  float originalRotation = mRotation;
  mRotation = mAimAngle;
  auto *particle = mGun->EmitParticle(5.0f, 300.0f, offset);
  particle->StartGraceTime();
  mRotation = originalRotation;
  
  // Play shoot sound
  mGame->GetAudio()->PlaySound("LaserShoot.wav", false);
}