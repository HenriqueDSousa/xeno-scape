//
// Created by Lucas N. Ferreira on 18/09/25.
//

#include "../../Game.h"
#include "ParticleSystemComponent.h"

template <typename T>
ParticleSystemComponent<T>::ParticleSystemComponent(class Actor* owner, int poolSize, int updateOrder)
    : Component(owner, updateOrder)
{
  // Create a pool of particles
  for (int i = 0; i < poolSize; i++)
  {
    auto* p = new T(owner->GetGame());
    mParticles.push_back(p);
  }
}

template <typename T>
ParticleSystemComponent<T>::~ParticleSystemComponent()
{
  mParticles.clear();
}

template <typename T>
T *ParticleSystemComponent<T>::EmitParticle(float lifetime, float speed, const Vector2& offsetPosition)
{
  for (auto p : mParticles)
  {
    if (p->IsDead())
    {
      // Wake up the particle
      Vector2 spawnPos = mOwner->GetPosition() + offsetPosition;
      p->Awake(spawnPos, mOwner->GetRotation(), lifetime);

      // Forward velocity
      Vector2 direction = mOwner->GetForward();
      p->Emit(direction, speed);

      // Set the particle's layer to match the owner's layer
      p->SetLayer(mOwner->GetLayer());

      return p;
    }
  }
  return nullptr;
}