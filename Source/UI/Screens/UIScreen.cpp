//
// Created by henrique on 11/5/25.
//

#include "UIScreen.h"

#include <utility>

#include "../../Game.h"
#include "../Font.h"

UIScreen::UIScreen(class Game *game, const std::string& fontName)
: mGame(game),
  mRenderer(game->GetRenderer()),
  mPos(Vector2::Zero),
  mSize(Vector2(Vector2::Zero)),
  mState(EActive),
  mIsVisible(true),
  mSelectedButtonIndex(0) {
  mGame->PushUI(this);
  mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen() {
  mGame->RemoveUI(this);

  for (UIText* text : mTexts) {
    delete text;
  }
  mTexts.clear();
}

void UIScreen::Update(float deltaTime) {
  for (size_t i = 0; i < mButtons.size(); ++i) {
    mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
  }
}

void UIScreen::Draw() {

  for (UIText* text : mTexts) {
    text->Draw(mRenderer, mPos);
  }

  for (UIButton* button : mButtons) {
    button->Draw(mRenderer, mPos);
  }
}

void UIScreen::ProcessInput(const uint8_t *keys) {}

void UIScreen::HandleKeyPress(int key) {
  if (key == SDLK_UP ) {
    // Move para o botão anterior
    mSelectedButtonIndex--;
    if (mSelectedButtonIndex < 0) {
      mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;
    }
        }
  else if (key == SDLK_DOWN) {
    // Move para o próximo botão
    mSelectedButtonIndex++;
    if (mSelectedButtonIndex >= static_cast<int>(mButtons.size())) {
      mSelectedButtonIndex = 0;
    }
      }
  else if (key == SDLK_RETURN)  {
    // Ativa o botão selecionado
    if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
      mButtons[mSelectedButtonIndex]->OnClick();
    }
  }

}

void UIScreen::HandleMouse(const SDL_Event& event) {
  if (event.button.button == SDL_BUTTON_LEFT) {
    int x = event.button.x;
    int y = event.button.y;
    for (UIButton* button : mButtons) {
      if (button->ContainsPoint(Vector2(x, y) - GetPosition())) {
        button->OnClick();
      }
    }
  }

  if (event.type == SDL_MOUSEMOTION) {
    int x = event.motion.x;
    int y = event.motion.y;

    int index = -1;

    for (size_t i = 0; i < mButtons.size(); ++i) {
      if (mButtons[i]->ContainsPoint(Vector2(x, y) - GetPosition())) {
        index = i;
        mSelectedButtonIndex = i;
      }
    }

    // Update button highlighting
    if (index != -1) {
      for (size_t i = 0; i < mButtons.size(); ++i) {
        mButtons[i]->SetHighlighted(static_cast<int>(i) == index);
      }
    }
  }
}

UIText *UIScreen::AddText(const std::string &name, const Vector2 &pos,
                          const Vector2 &dims, const int pointSize,
                          Vector4 color, const int unsigned wrapLength) {
  UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, color);
  mTexts.emplace_back(t);

  return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2& pos,
                              const Vector2& dims, int pointSize,
                              UIButton::TextPos alignText,
                              std::function<void()> onClick, Vector2 textPos,
                              Vector4 textColor) {
  auto* b = new UIButton(name, mFont, std::move(onClick), pos, dims,
    Color::Gray,
    pointSize, 1024,
    textPos, alignText, textColor);
  mButtons.emplace_back(b);

  if (mButtons.size() == 1) {
    mSelectedButtonIndex = 0;
  }

  return b;
}

void UIScreen::ChangeResolution(float oldScale, float newScale) {
  mPos.x = mPos.x / oldScale * newScale;
  mPos.y = mPos.y / oldScale * newScale;
  mSize.x = mSize.x / oldScale * newScale;
  mSize.y = mSize.y / oldScale * newScale;

  for (UIButton* button : mButtons) {
    button->ChangeResolution(oldScale, newScale);
  }

  for (UIText* text: mTexts) {
    text->ChangeResolution(oldScale, newScale);
  }
}
