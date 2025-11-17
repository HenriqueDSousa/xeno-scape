//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"

#include <iostream>
#include <ostream>
#include <unordered_set>

#include "../../Actors/Actor.h"
#include "../../Game.h"

std::unordered_set ResolvableLayers = {ColliderLayer::Enemy, ColliderLayer::Player};

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
        ColliderLayer layer, bool isStatic, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mIsStatic(isStatic)
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    Vector2 center = mOwner->GetPosition() + mOffset;
    float halfW = (float)mWidth * 0.5f;
    float halfH = (float)mHeight * 0.5f;
    return Vector2(center.x - halfW, center.y - halfH);
}

Vector2 AABBColliderComponent::GetMax() const
{
    Vector2 center = mOwner->GetPosition() + mOffset;
    float halfW = (float)mWidth * 0.5f;
    float halfH = (float)mHeight * 0.5f;
    return Vector2(center.x + halfW, center.y + halfH);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
    Vector2 minA = GetMin();
    Vector2 maxA = GetMax();
    Vector2 minB = b.GetMin();
    Vector2 maxB = b.GetMax();

    return !(maxA.x <= minB.x || minA.x >= maxB.x ||
             maxA.y <= minB.y || minA.y >= maxB.y);
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b) const
{
    Vector2 minA = GetMin();
    Vector2 maxA = GetMax();
    Vector2 minB = b->GetMin();
    Vector2 maxB = b->GetMax();

    // They must overlap horizontally
    if (maxA.x <= minB.x || minA.x >= maxB.x)
        return 0.0f;

    float overlapDown = maxA.y - minB.y;
    float overlapUp   = maxB.y - minA.y;

    if (overlapDown < overlapUp)
        return -overlapDown;
    else
        return overlapUp;
}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b) const
{
    Vector2 minA = GetMin();
    Vector2 maxA = GetMax();
    Vector2 minB = b->GetMin();
    Vector2 maxB = b->GetMax();

    // They must overlap vertically
    if (maxA.y <= minB.y || minA.y >= maxB.y)
        return 0.0f;

    // Compute overlap distances
    float overlapLeft  = maxA.x - minB.x;  // A right of B → push left
    float overlapRight = maxB.x - minA.x;  // A left of B → push right

    // Positive means "move A right", negative means "move A left"
    if (overlapLeft < overlapRight)
        return -overlapLeft;
    else
        return overlapRight;
}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody)
{
    auto colliders = GetGame()->GetColliders();

    for (auto collider : colliders) {
        if (collider == this || !collider->mIsEnabled) {
            continue;
        }
        if (Intersect(*collider)) {
            float overlap = GetMinHorizontalOverlap(collider);
            bool isStatic = mIsStatic || collider->mIsStatic;
            if ((GetLayer() != ColliderLayer::Collectables &&
                collider->GetLayer() != ColliderLayer::Collectables) || isStatic ) {
                ResolveHorizontalCollisions(rigidBody, overlap);
            }
            rigidBody->GetOwner()->OnHorizontalCollision(overlap, collider);
            if (collider->GetOwner()->GetComponent<RigidBodyComponent>() == nullptr ||
                collider->GetOwner()->GetComponent<RigidBodyComponent>()->IsEnabled() == false) {
                collider->GetOwner()->OnHorizontalCollision(overlap, this);
            }
            return overlap;
        }
    }
    return 0.0f;
}

float AABBColliderComponent::DetectVerticalCollision(RigidBodyComponent *rigidBody)
{
    auto colliders = GetGame()->GetColliders();

    for (auto collider : colliders) {
        if (collider == this || !collider->mIsEnabled) {
            continue;
        }
        bool isStatic = mIsStatic || collider->mIsStatic;
        if (Intersect(*collider)) {
            float overlap = GetMinVerticalOverlap(collider);
            if ((GetLayer() != ColliderLayer::Collectables &&
                collider->GetLayer() != ColliderLayer::Collectables) || isStatic )
                ResolveVerticalCollisions(rigidBody, overlap);
            rigidBody->GetOwner()->OnVerticalCollision(overlap, collider);
            collider->GetOwner()->OnVerticalCollision(overlap, this);
            return overlap;
        }
    }
    return 0.0f;
}


void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap)
{
    if (minXOverlap == 0.0f) return;

    Vector2 pos = rigidBody->GetOwner()->GetPosition();
    pos.x += minXOverlap;
    rigidBody->GetOwner()->SetPosition(pos);

    Vector2 vel = rigidBody->GetVelocity();
    vel.x = 0.0f;
    rigidBody->SetVelocity(vel);
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap)
{
    if (minYOverlap == 0.0f) return;

    Vector2 pos = rigidBody->GetOwner()->GetPosition();
    pos.y += minYOverlap;
    rigidBody->GetOwner()->SetPosition(pos);

    Vector2 vel = rigidBody->GetVelocity();
    vel.y = 0.0f;
    rigidBody->SetVelocity(vel);

    if (minYOverlap < 0.0f) {
      rigidBody->GetOwner()->SetOnGround();
    }
}

void AABBColliderComponent::DebugDraw(class Renderer *renderer)
{
    renderer->DrawRect(mOwner->GetPosition() + mOffset,Vector2((float)mWidth, (float)mHeight), mOwner->GetRotation(),
                       Color::Green, mOwner->GetGame()->GetCameraPos(), RendererMode::LINES);
}