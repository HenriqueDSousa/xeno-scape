//
// Created by Lucas N. Ferreira on 05/11/25.
//

#pragma once


#include "UIElement.h"
#include "../Math.h"

class UIRect : public UIElement
{
public:
    UIRect(const Vector2 &position, const Vector2 &size, Vector4 color, float angle = 0.0f,
      RendererMode rendererMode = RendererMode::TRIANGLES);
    ~UIRect();

    void Draw(Renderer* renderer, const Vector2 &screenPos) override;
    void SetColor(const Vector4 &color) { mColor = color; }

protected:
    RendererMode mRenderMode;
    float mAngle;
};

