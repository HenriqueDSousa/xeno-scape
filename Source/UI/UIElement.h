//
// Created by henrique on 11/6/25.
//

#pragma once

#include <SDL.h>

#include "../Math.h"
#include "../Renderer/Renderer.h"

class UIElement {
public:
  virtual ~UIElement() = default;
  UIElement(const Vector2 &pos, const Vector2 &size, const Vector4 &color);

  // Getters/setters
  const Vector2& GetPosition() const { return mPosition; }
  void SetPosition(const Vector2 &pos) { mPosition = pos; }

  const Vector2& GetSize() const { return mSize; }
  void SetSize(const Vector2 &size) { mSize = size; }

  const Vector4& GetColor() const { return mColor; }
  void SetColor(const Vector4 &color) { mColor = color; }

  bool IsVisible() const { return mIsVisible; }
  void SetVisible(bool visible) { mIsVisible = visible; }

  virtual void Draw(Renderer* renderer, const Vector2 &screenPos) {};

  virtual void Update(float deltaTime) {};

  virtual void ChangeResolution(float oldScale, float newScale) {};

protected:
  Vector2 mPosition;
  Vector2 mSize;
  Vector4 mColor;
  bool mIsVisible = true;
};
