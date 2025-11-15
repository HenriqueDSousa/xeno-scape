//
// Created by Lucas N. Ferreira on 05/11/25.
//

#include "UIRect.h"

UIRect::UIRect(const Vector2 &pos, const Vector2 &size, Vector4 color, float angle,
      RendererMode rendererMode)
        :UIElement(pos, size, color)
        ,mRenderMode(rendererMode)
        ,mAngle(angle)
{

}

UIRect::~UIRect()
{

}

void UIRect::Draw(Renderer* renderer, const Vector2 &screenPos)
{
  renderer->DrawRect(mPosition, mSize, mAngle, mColor,
                    screenPos, mRenderMode);
}