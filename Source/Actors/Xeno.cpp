//
// Created by henrique on 11/13/25.
//

#include "Xeno.h"
Xeno::Xeno(Game* game)
  :Actor(game)
{
}

void Xeno::OnHorizontalCollision(const float minOverlap,
                                 AABBColliderComponent* other) {
  Actor::OnHorizontalCollision(minOverlap, other);
}

void Xeno::OnVerticalCollision(const float minOverlap,
                               AABBColliderComponent* other) {
  Actor::OnVerticalCollision(minOverlap, other);
}

void Xeno::Kill() { Actor::Kill(); }

void Xeno::OnUpdate(float deltaTime) { Actor::OnUpdate(deltaTime); }

void Xeno::OnProcessInput(const Uint8* keyState) {
  Actor::OnProcessInput(keyState);
}