//
// Created by henrique on 11/20/25.
//

#include "XenoGun.h"
#include "XenoArm.h"
#include "../Game.h"
#include <SDL.h>
#include <cmath>

XenoGun::XenoGun(Game* game, Xeno* owner)
  :Actor(game)
  ,mOwner(owner)
  ,mBluePortalActive(false)
  ,mOrangePortalActive(false)
  ,mShootingCooldown(0.0f)
  ,mIsShooting(false)
  ,mShootingAnimTimer(0.0f)
  ,mCurrentMode(ShootingMode::SHOOT)
  ,mActiveBluePortal(nullptr)
  ,mActiveOrangePortal(nullptr)
{

  mBluePortalGun = new ParticleSystemComponent<class BluePortalBullet>(this, 20);
  mOrangePortalGun = new ParticleSystemComponent<class OrangePortalBullet>(this, 20);
  mShootingGun = new ParticleSystemComponent<class ShootBullet>(this, 20);

  mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/Xeno/XenoGun.png",
  "../Assets/Sprites/Xeno/XenoGun.json",16 * mGame->GetGameScale(), 10 * mGame->GetGameScale(), 98);
  mAnimatorComponent->AddAnimation("aim",std::vector{0});
  mAnimatorComponent->AddAnimation("bluePortalShooting",std::vector{5});
  mAnimatorComponent->AddAnimation("orangePortalShooting",std::vector{6});
  mAnimatorComponent->AddAnimation("shooting",std::vector{1,2,3,4});
  mAnimatorComponent->SetVisible(false);
  
  // Create the portal instances once
  mActiveBluePortal = new BluePortal(game, this, Vector2::Zero);
  mActiveBluePortal->SetState(ActorState::Paused);
  
  mActiveOrangePortal = new OrangePortal(game, this, Vector2::Zero);
  mActiveOrangePortal->SetState(ActorState::Paused);
  
  // Set gun reference for portal bullets
  SetupPortalBulletReferences();
}

void XenoGun::OnUpdate(float deltaTime) {


  // Update gun position/rotation
  {
    XenoArm* arm = mOwner->GetAimArm();
    if (arm) {
      Vector2 handPos = arm->GetHandPosition();
      float angle = arm->GetRotation();
      float offsetDistance = 5.0f * mGame->GetGameScale();
      Vector2 offset(cosf(angle) * offsetDistance, sinf(angle) * offsetDistance);

      SetPosition(handPos + offset);
      SetRotation(angle);
    }
  }

  if (mOwner->IsAiming()) {
    // Handle shooting animation timer
    if (mShootingAnimTimer > 0.0f) {
      mShootingAnimTimer -= deltaTime;
      if (mShootingAnimTimer <= 0.0f) {
        mShootingAnimTimer = 0.0f;
        mAnimatorComponent->SetAnimation("aim");
      }
    } else if (mAnimatorComponent->GetCurrentAnimation() != "aim") {
      // Fallback
      mAnimatorComponent->SetAnimation("aim");
    }
    mAnimatorComponent->SetVisible(true);
  } else {
    mAnimatorComponent->SetVisible(false);
  }
  
  mShootingCooldown -= deltaTime;
  if (mShootingCooldown < 0.0f) {
    mShootingCooldown = 0.0f;
  }
}

void XenoGun::OnProcessInput(const Uint8* keyState) {
  // Mode switching with number keys
  if (keyState[SDL_SCANCODE_1]) {
    mCurrentMode = ShootingMode::PORTAL_BLUE;
  }
  else if (keyState[SDL_SCANCODE_2]) {
    mCurrentMode = ShootingMode::PORTAL_ORANGE;
  }
  else if (keyState[SDL_SCANCODE_3]) {
    mCurrentMode = ShootingMode::SHOOT;
  }

  // Shoot with left mouse button when aiming and cooldown allows
  int mouseX = 0, mouseY = 0;
  Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
  
  if (mOwner->IsAiming() && (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) && mShootingCooldown <= 0.0f) {
    Shoot();
    mShootingCooldown = 0.5f;
  }
}

Vector2 XenoGun::GetShootingDirection() const {
  // Use the arm's rotation to determine shooting direction
  XenoArm* arm = mOwner->GetAimArm();
  if (arm) {
    float angle = arm->GetRotation();
    return Vector2(cosf(angle), sinf(angle));
  }
  
  // Fallback: shoot to the right
  return Vector2(1.0f, 0.0f);
}

void XenoGun::Shoot() {
  Vector2 direction = GetShootingDirection();
  float bulletSpeed = 1000.0f;
  float bulletLifetime = 2.0f;
  
  switch (mCurrentMode) {
    case ShootingMode::PORTAL_BLUE:
      mBluePortalGun->EmitParticle(bulletLifetime, bulletSpeed, direction * 10.0f);
      mAnimatorComponent->SetAnimation("bluePortalShooting");
      mAnimatorComponent->SetAnimFPS(5.0f);
      mShootingAnimTimer = 0.2f;
      mGame->GetAudio()->PlaySound("PortalShoot.wav", false);
      break;
      
    case ShootingMode::PORTAL_ORANGE:
      mOrangePortalGun->EmitParticle(bulletLifetime, bulletSpeed, direction * 10.0f);
      mAnimatorComponent->SetAnimation("orangePortalShooting");
      mAnimatorComponent->SetAnimFPS(5.0f);
      mShootingAnimTimer = 0.2f;
      mGame->GetAudio()->PlaySound("PortalShoot.wav", false);
      break;
      
    case ShootingMode::SHOOT:
      mShootingGun->EmitParticle(bulletLifetime, bulletSpeed, direction);
      mAnimatorComponent->SetAnimation("shooting");
      mAnimatorComponent->SetAnimFPS(10.0f);
      mShootingAnimTimer = 0.4f; // Duration for 4-frame animation at 10fps (0.4s)
      mGame->GetAudio()->PlaySound("LaserShoot.wav", false);
      break;
  }
}

void XenoGun::SetupPortalBulletReferences() {
  // Set gun reference for all blue portal bullets
  for (auto* bullet : mBluePortalGun->GetParticles()) {
    bullet->SetGun(this);
  }
  
  // Set gun reference for all orange portal bullets
  for (auto* bullet : mOrangePortalGun->GetParticles()) {
    bullet->SetGun(this);
  }
}