//
// Created by henrique on 11/6/25.
//

#pragma once

#include <string>

#include "../Math.h"
#include "../Renderer/Texture.h"
#include "UIElement.h"

class UIText : public UIElement {
public:
  UIText(const std::string& text, class Font* font, int pointSize = 40, const unsigned wrapLength = 1024,
         const Vector2 &pos = Vector2::Zero, const Vector2 &size = Vector2(100.f, 20.0f), const Vector4& color = Color::White);

  ~UIText();

  void SetText(const std::string& name);
  void Draw(Renderer* renderer, const Vector2 &screenPos) override;

  void ChangeResolution(float oldScale, float newScale) override;

protected:
  std::string mText;
  class Font* mFont;
  Texture* mTextTexture;

  float mPointSize;
  unsigned int mWrapLength;
};