//
// Created by henrique on 11/8/25.
//

#include "UIButton.h"

UIButton::UIButton(const std::string& text, class Font* font, std::function<void()> onClick,
                    const Vector2& pos, const Vector2& size, const Vector3& color,
                    int pointSize , unsigned wrapLength,
                    const Vector2 &textPos, TextPos textAlign, const Vector3& textColor, const Vector2 &textSize)
        :UIElement(pos, size, color)
        ,mOnClick(onClick)
        ,mHighlighted(false)
        ,mTextAlign(textAlign)
{
  mText = new UIText(text, font, pointSize, wrapLength, textPos, textSize, textColor);
}

UIButton::~UIButton()
{
  delete mText;
  mText = nullptr;
}


void UIButton::Draw(Renderer *renderer, const Vector2 &screenPos)
{
  Vector2 rectCenter = screenPos + mPosition + mSize * 0.5f;

  if (mHighlighted) {
    renderer->DrawRect(rectCenter, mSize, 0.0f, Color::Yellow,
      Vector2::Zero, RendererMode::TRIANGLES);
  } else {
    renderer->DrawRect(rectCenter, mSize, 0.0f, mColor,
    Vector2::Zero, RendererMode::TRIANGLES);
  }

  if (mTextAlign == TextPos::AlignLeft) {
    mText->Draw(renderer, screenPos + mPosition + Vector2(8.0f, 0.0f));
  }
  else if (mTextAlign == TextPos::Center) {
    mText->Draw(renderer, screenPos + mPosition + mSize * 0.5f - mText->GetSize() * 0.5f);
  }
}

bool UIButton::ContainsPoint(const Vector2 &pt) const {
  if (pt.x < mPosition.x || pt.x > mPosition.x + mSize.x ||
      pt.y < mPosition.y || pt.y > mPosition.y + mSize.y)
  {
    return false;
  }
  return true;
}


void UIButton::OnClick()
{
  if (mOnClick) {
    mOnClick();
  }
}

void UIButton::ChangeResolution(float oldScale, float newScale) {
  mPosition.x = mPosition.x / oldScale * newScale;
  mPosition.y = mPosition.y / oldScale * newScale;
  mSize.x = mSize.x / oldScale * newScale;
  mSize.y = mSize.y / oldScale * newScale;

  mText->ChangeResolution(oldScale, newScale);
}