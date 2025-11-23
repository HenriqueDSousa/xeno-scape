//
// Created by henrique on 11/20/25.
//

#pragma once

#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Particles/ParticleSystemComponent.h"
#include "../Components/Particles/PortalBullet.h"
#include "../Components/Particles/ShootBullet.h"
#include "Actor.h"
#include "Portal.h"

enum class ShootingMode {
  PORTAL_BLUE,
  PORTAL_ORANGE,
  SHOOT
};

class XenoGun : public Actor {
 public:

  XenoGun(class Game* game, class Xeno* owner);

  BluePortal* GetActiveBluePortal() const { return mActiveBluePortal; }
  void SetActiveBluePortal(BluePortal* portal) { mActiveBluePortal = portal; }
  void SetBluePortalActive(bool active) { mBluePortalActive = active; }
  bool IsBluePortalActive() { return mBluePortalActive; }
  
  OrangePortal* GetActiveOrangePortal() const { return mActiveOrangePortal; }
  void SetActiveOrangePortal(OrangePortal* portal) { mActiveOrangePortal = portal; }
  void SetOrangePortalActive(bool active) { mOrangePortalActive = active; }
  bool IsOrangePortalActive() { return mOrangePortalActive; }

  ShootingMode GetShootingMode() {return mCurrentMode; }

 protected:
  void OnUpdate(float deltaTime) override;
  void OnProcessInput(const Uint8* keyState) override;

private:
  Vector2 GetShootingDirection() const;
  void Shoot();
  void SetupPortalBulletReferences();

  class ParticleSystemComponent<class BluePortalBullet> *mBluePortalGun;
  bool mBluePortalActive;
  BluePortal* mActiveBluePortal;

  class ParticleSystemComponent<class OrangePortalBullet> *mOrangePortalGun;
  bool mOrangePortalActive;
  OrangePortal* mActiveOrangePortal;

  class ParticleSystemComponent<class ShootBullet> *mShootingGun;
  float mShootingCooldown;
  float mIsShooting;
  float mShootingAnimTimer;
  ShootingMode mCurrentMode;
  class AnimatorComponent* mAnimatorComponent;
  Xeno* mOwner;
};

