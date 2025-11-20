//
// Created by henrique on 11/20/25.
//

#pragma once

#include "../Components/Particles/ParticleSystemComponent.h"
#include "../Components/Particles/PortalBullet.h"
#include "../Components/Particles/ShootBullet.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "Actor.h"

enum ShootingMode {
  PORTAL_BLUE,
  PORTAL_ORANGE,
  SHOOT
};

class XenoGun : public Actor {
 public:
  XenoGun(Game* game, Xeno* owner);

  bool IsBluePortalActive() { return mBluePortalActive; }
  bool IsOrangePortalActive() { return mOrangePortalActive; }

 protected:
  void OnUpdate(float deltaTime) override;
  void OnProcessInput(const Uint8* keyState) override;

private:
  Vector2 GetShootingDirection() const;
  void Shoot();

  class ParticleSystemComponent<class BluePortalBullet> *mBluePortalGun;
  bool mBluePortalActive;
  class ParticleSystemComponent<class OrangePortalBullet> *mOrangePortalGun;
  bool mOrangePortalActive;
  class ParticleSystemComponent<class ShootBullet> *mShootingGun;
  float mShootingCooldown;
  float mIsShooting;
  float mShootingAnimTimer;
  ShootingMode mCurrentMode;
  class AnimatorComponent* mAnimatorComponent;
  Xeno* mOwner;
};

