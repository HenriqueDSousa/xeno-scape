#include "ShootBullet.h"

#include "../../Game.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/AABBColliderComponent.h"

ShootBullet::ShootBullet(class Game* game)
  :Bullet(game) {
  mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/ShootBullet.png", "" ,
    5 * mGame->GetGameScale(), 5 * mGame->GetGameScale());
  mAnimatorComponent->SetVisible(false);
}

void ShootBullet::OnHorizontalCollision(const float minOverlap,
                                        AABBColliderComponent* other) {
  if (other->GetLayer() == ColliderLayer::Blocks ||
    other->GetLayer() == ColliderLayer::Bullet ||
    other->GetLayer() == ColliderLayer::PortalDisabledBlock) {
    Kill();
  }
}
void ShootBullet::OnVerticalCollision(const float minOverlap,
                                      AABBColliderComponent* other) {
  OnHorizontalCollision(minOverlap, other);
}

void ShootBullet::OnUpdate(float deltaTime) {
  mGraceTime = std::max(0.0f,mGraceTime-deltaTime);
}

void ShootBullet::Kill() {
  mGraceTime=0;
  Bullet::Kill();
}