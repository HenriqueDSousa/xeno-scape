//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"

const float MAX_SPEED_X = 750.0f;
const float MAX_SPEED_Y = 750.0f;
const float GRAVITY = 2000.0f;

class RigidBodyComponent : public Component
{
public:
    // Lower update order to update first
    RigidBodyComponent(class Actor* owner, float mass = 1.0f, float friction = 0.0f,
                       bool applyGravity = true, int updateOrder = 10);

    void Update(float deltaTime) override;

    const Vector2& GetVelocity() const { return mVelocity; }
    void SetVelocity(const Vector2& velocity) { mVelocity = velocity; }

    const Vector2& GetAcceleration() const { return mAcceleration; }
    void SetAcceleration(const Vector2& acceleration) { mAcceleration = acceleration; }

    void SetApplyGravity(const bool applyGravity) { mApplyGravity = applyGravity; }

    void ApplyForce(const Vector2 &force);

    void SetMaxSpeedX(const float maxSpeedX) { mMaxSpeedX = maxSpeedX; }
    void SetMaxSpeedY(const float maxSpeedY) { mMaxSpeedY = maxSpeedY; }
    float GetMaxSpeedX() const { return mMaxSpeedX; }
    float GetMaxSpeedY() const { return mMaxSpeedY; }

private:
    bool mApplyGravity;

    // Physical properties
    float mFrictionCoefficient;
    float mMass;
    float mMaxSpeedX = MAX_SPEED_X;
    float mMaxSpeedY = MAX_SPEED_Y;
    float mGravity = GRAVITY;

    Vector2 mVelocity;
    Vector2 mAcceleration;
};
