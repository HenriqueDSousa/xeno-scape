//
// Created by henrique on 11/20/25.
//

#include "PortalBullet.h"
#include "../../Actors/XenoArm.h"

#include <cfloat>

#include "../../Actors/Portal.h"
#include "../../Actors/XenoArm.h"
#include "../../Actors/XenoGun.h"
#include "../../Game.h"
#include "../Drawing/AnimatorComponent.h"
#include "../Physics/AABBColliderComponent.h"
#include "../Physics/RigidBodyComponent.h"

PortalBullet::PortalBullet(class Game* game, PortalType portalType)
  :Bullet(game)
  ,mPortalType(portalType)
  ,mGun(nullptr) {

  if (mPortalType == PortalType::BLUE) {
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/BluePortalShoot.png", "" ,
      7 * mGame->GetGameScale(), 7 * mGame->GetGameScale(), 0);
    mAnimatorComponent->SetVisible(false);
  } else if (mPortalType == PortalType::ORANGE) {
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/OrangePortalShoot.png", "" ,
      7 * mGame->GetGameScale(), 7 * mGame->GetGameScale(), 0);
    mAnimatorComponent->SetVisible(false);
  }
  mCollisionComponent->SetLayer(ColliderLayer::PortalBullet);
}

void PortalBullet::OnHorizontalCollision(const float minOverlap,
                                         AABBColliderComponent* other) {
  if (other->GetLayer() == ColliderLayer::Blocks) {
    float rotation = 0.0f;
    PortalDirection direction;
    if (mRigidBody->GetVelocity().x > 0.0f) {
      rotation = 0.0f;
      direction = PortalDirection::LEFT;
    } else {
      rotation = Math::Pi;
      direction = PortalDirection::RIGHT;
    }
    Kill();
    SpawnPortal(rotation, direction, minOverlap);
  } else if (other->GetLayer() != ColliderLayer::Portal && (!InGraceTime() || other->GetLayer() != ColliderLayer::Player)) {
    Kill();
  }
}

void PortalBullet::OnVerticalCollision(const float minOverlap,
                                       AABBColliderComponent* other) {
  if (other->GetLayer() == ColliderLayer::Blocks) {
    float rotation = 0.0f;
    PortalDirection direction;
    if (mRigidBody->GetVelocity().y > 0.0f) {
      rotation = Math::PiOver2;
      direction = PortalDirection::UP;
    } else {
      rotation = -Math::PiOver2;
      direction = PortalDirection::DOWN;
    }
    Kill();
    SpawnPortal(rotation, direction, minOverlap);
  } else if (other->GetLayer() != ColliderLayer::Portal && (!InGraceTime() || other->GetLayer() != ColliderLayer::Player)) {
    Kill();
  }
}

void PortalBullet::OnUpdate(float deltaTime) {
  Bullet::OnUpdate(deltaTime);
}

void PortalBullet::SpawnPortal(float rotation, PortalDirection direction, float minOverlap) const {
  if (!mGun) return;

  Vector2 portalPosition = GetPosition();
  if (direction == PortalDirection::LEFT || direction == PortalDirection::RIGHT) {
    portalPosition.x += minOverlap;
  } else {
    portalPosition.y += minOverlap;
  }

  if (mPortalType == PortalType::BLUE) {
    auto* portal = mGun->GetActiveBluePortal();
    if (portal) {
      portal->SetPosition(portalPosition);
      portal->SetRotation(rotation);
      portal->SetDirection(direction);
      portal->SetActive(true);
      mGun->SetBluePortalActive(true);
    }
  } else if (mPortalType == PortalType::ORANGE) {
    auto* portal = mGun->GetActiveOrangePortal();
    if (portal) {
      portal->SetPosition(portalPosition);
      portal->SetRotation(rotation);
      portal->SetDirection(direction);
      portal->SetActive(true);
      mGun->SetOrangePortalActive(true);
    }
  }
}

void PortalBullet::FixInitialOverlap()
{
    bool resolvedSomething = false;
    for (int iter = 0; iter < 5; ++iter)
    {
        resolvedSomething = false;

        Vector2 bulletPos = GetPosition();
        Vector2 gunPos = mGame->GetPlayer()->GetAimArm()->ComputeShoulderPos();

        auto colliders = GetGame()->GetColliders();

        for (auto* collider : colliders)
        {
            if (collider->GetLayer() != ColliderLayer::Blocks)
                continue;
            if (!mCollisionComponent->Intersect(*collider))
                continue;

            // BLOCK
            Vector2 bMin = collider->GetMin();
            Vector2 bMax = collider->GetMax();

            // BULLET
            Vector2 cMin = mCollisionComponent->GetMin();
            Vector2 cMax = mCollisionComponent->GetMax();

            // Overlaps
            float overlapLeft   = cMax.x - bMin.x;
            float overlapRight  = bMax.x - cMin.x;
            float overlapUp     = cMax.y - bMin.y;
            float overlapDown   = bMax.y - cMin.y;

            struct Cand {
                Vector2 corr;
                float distSq;
                bool valid;
            };

            Cand cand[4];

            cand[0] = { Vector2(-overlapLeft, 0.0f), (bulletPos + Vector2(-overlapLeft,0.0f) - gunPos).LengthSq(), overlapLeft  > 0 };
            cand[1] = { Vector2(overlapRight, 0.0f), (bulletPos + Vector2(overlapRight,0.0f) - gunPos).LengthSq(), overlapRight > 0 };
            cand[2] = { Vector2(0.0f, -overlapUp),   (bulletPos + Vector2(0.0f,-overlapUp) - gunPos).LengthSq(),   overlapUp    > 0 };
            cand[3] = { Vector2(0.0f, overlapDown),  (bulletPos + Vector2(0.0f,overlapDown) - gunPos).LengthSq(),  overlapDown  > 0 };

            // Pick closest-to-gun correction
            float bestDist = FLT_MAX;
            Vector2 bestCorr = Vector2::Zero;

            for (auto& c : cand)
            {
                if (!c.valid) continue;
                if (c.distSq < bestDist)
                {
                    bestDist = c.distSq;
                    bestCorr = c.corr;
                }
            }

            bulletPos += bestCorr;
            SetPosition(bulletPos);

            resolvedSomething = true;
        }

        // If we went through all colliders and didn’t resolve anything, stop!
        if (!resolvedSomething)
            break;
    }
}