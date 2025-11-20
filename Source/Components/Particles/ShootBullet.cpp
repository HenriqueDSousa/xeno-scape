#include "ShootBullet.h"

#include "../../Game.h"
#include "../Drawing/AnimatorComponent.h"

ShootBullet::ShootBullet(class Game* game)
  :Bullet(game) {
  mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/ShootBullet.png", "" ,
    5 * mGame->GetGameScale(), 5 * mGame->GetGameScale());
  mAnimatorComponent->SetVisible(false);
}

void ShootBullet::OnHorizontalCollision(const float minOverlap,
                                        AABBColliderComponent* other) {
  Bullet::OnHorizontalCollision(minOverlap, other);
}
void ShootBullet::OnVerticalCollision(const float minOverlap,
                                      AABBColliderComponent* other) {
  Bullet::OnVerticalCollision(minOverlap, other);
}

void ShootBullet::OnUpdate(float deltaTime) {
  Bullet::OnUpdate(deltaTime);

}