#pragma once

#include "Particle.h"

class Bullet : public Particle
{
public:
  Bullet(class Game* game);
  ~Bullet();

  void OnUpdate(float deltaTime) override;

  void Kill() override;
  void Emit(const Vector2 &direction, float speed) override;
  void Awake(const Vector2 &position, const float &rotation, float lifetime) override;

protected:

  class AnimatorComponent* mAnimatorComponent;
  class AABBColliderComponent* mCollisionComponent;
  class RigidBodyComponent *mRigidBody;
};