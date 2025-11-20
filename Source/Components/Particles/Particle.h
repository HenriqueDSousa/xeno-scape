#pragma once

#include "../../Actors/Actor.h"

class Particle : public Actor
{
public:
  Particle(class Game* game);

  void OnUpdate(float deltaTime) override;

  virtual void Awake(const Vector2 &position, const float &rotation, float lifetime = 1.0f);
  virtual void Emit(const Vector2 &direction, float speed);
  virtual void Kill();

  bool IsDead() const { return mIsDead; }

private:
  float mLifeTime;
  bool mIsDead;
};