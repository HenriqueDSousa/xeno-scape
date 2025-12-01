//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "RigidBodyComponent.h"
#include <vector>
#include <set>
#include <unordered_set>

enum class ColliderLayer
{
  Player,
  Enemy,
  Blocks,
  Bullet,
  Portal,
  PortalBullet,
  PortalDisabledBlock,
  Collectables
};

class AABBColliderComponent : public Component
{
public:

    int mWidth;
    int mHeight;

    AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
                                ColliderLayer layer, bool isStatic = false, int updateOrder = 10);
    ~AABBColliderComponent() override;

    bool Intersect(const AABBColliderComponent& b) const;

    float DetectHorizontalCollision(RigidBodyComponent *rigidBody);
    float DetectVerticalCollision(RigidBodyComponent *rigidBody);

    float GetMinVerticalOverlap(AABBColliderComponent* b) const;
    float GetMinHorizontalOverlap(AABBColliderComponent* b) const;

    Vector2 GetMin() const;
    Vector2 GetMax() const;
    ColliderLayer GetLayer() const { return mLayer; }

    // Drawing for debug purposes
    void DebugDraw(class Renderer* renderer) override;
    void SetOffset(Vector2 offset);

    void SetHeight(int height) { mHeight = height; }
    void SetWidth(int width) { mWidth = width; }

    void SetStatic(bool isStatic) { mIsStatic = isStatic; }

    void SetLayer(ColliderLayer layer) { mLayer = layer; }
private:
    void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);
    void ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);


    Vector2 mOffset;
    bool mIsStatic;

    ColliderLayer mLayer;
};