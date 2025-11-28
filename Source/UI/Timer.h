//
// Created by henrique on 11/9/25.
//

#pragma once
#include "UIElement.h"
#include "UIText.h"

class Timer : public UIElement {
 public:
  Timer(Game* game, float timeLeft, const Vector2 &pos, const Vector2 &size, const Vector4 &color, Font* font);

  void Draw(Renderer* renderer, const Vector2& screenPos) override;
  void Update(float deltaTime) override;
  void SetTimeLeft(float timeLeft) { mTimeLeft = timeLeft; }
  void ChangeResolution(float oldScale, float newScale) override;

private:
  float mTimeLeft;
  UIText* mText;
  Game* mGame;
};


