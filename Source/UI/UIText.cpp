//
// Created by henrique on 11/6/25.
//

#include "UIText.h"

#include "../Game.h"
#include "Font.h"

UIText::UIText(const std::string& text, class Font* font, int pointSize,
               const unsigned wrapLength, const Vector2& pos,
               const Vector2& size, const Vector3& color)
: UIElement(pos, size, color),
  mWrapLength(wrapLength),
  mPointSize(pointSize),
  mFont(font),
  mTextTexture(nullptr) {
  SetText(text);
}

UIText::~UIText() {
  if (mTextTexture) {
    mTextTexture->Unload();
    mTextTexture = nullptr;
  }
}

void UIText::SetText(const std::string& name) {
  if (mTextTexture != nullptr) {
    mTextTexture->Unload();
    mTextTexture = nullptr;
  }
  mText = name;
  mTextTexture = mFont->RenderText(mText, mColor, mPointSize, mWrapLength);
  SetSize(Vector2(mTextTexture->GetWidth(), mTextTexture->GetHeight()));
}

void UIText::Draw(Renderer* renderer, const Vector2& cameraPos) {
  Vector2 centerPos = Vector2(mPosition.x + cameraPos.x + mSize.x * 0.5f,
                              mPosition.y + cameraPos.y + mSize.y * 0.5f);

  renderer->DrawTexture(centerPos,
                        Vector2(mSize.x, mSize.y),
                        0.0f, mColor,
                        mTextTexture);
}

void UIText::ChangeResolution(float oldScale, float newScale) {
  mPosition.x = mPosition.x / oldScale * newScale;
  mPosition.y = mPosition.y / oldScale * newScale;
  mSize.x = mSize.x / oldScale * newScale;
  mSize.y = mSize.y / oldScale * newScale;

  mPointSize = mPointSize / oldScale * newScale;
  SetText(mText);
}