//
// Created by henrique on 11/9/25.
//

#include "Timer.h"
Timer::Timer(float timeLeft, const Vector2& pos, const Vector2& size, const Vector4& color, Font* font)
: UIElement(pos, size, color),
  mTimeLeft(timeLeft) {
  mText = new UIText(std::to_string(timeLeft), font, 15, 1024, Vector2::Zero, size, color);
}

void Timer::Draw(Renderer* renderer, const Vector2& screenPos) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%.2f", mTimeLeft);
  mText->SetText(std::string(buf));
  mText->Draw(renderer, screenPos + mPosition + mSize * 0.5f - mText->GetSize() * 0.5f);
}

void Timer::Update(float deltaTime) {
  mTimeLeft -= deltaTime;
  if (mTimeLeft < 0.0f) {
    mTimeLeft = 0.0f;
  }
}

void Timer::ChangeResolution(float oldScale, float newScale) {
  mPosition.x = mPosition.x / oldScale * newScale;
  mPosition.y = mPosition.y / oldScale * newScale;
  mSize.x = mSize.x / oldScale * newScale;
  mSize.y = mSize.y / oldScale * newScale;

  mText->ChangeResolution(oldScale, newScale);
}