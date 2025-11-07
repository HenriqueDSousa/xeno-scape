//
// Created by henrique on 11/5/25.
//

#include "UIScreen.h"

#include "../Game.h"
#include "Font.h"

UIScreen::UIScreen(class Game *game, const std::string& fontName)
: mGame(game),
  mRenderer(game->GetRenderer()),
  mPos(Vector2::Zero),
  mSize(Vector2(Vector2::Zero)),
  mState(EActive),
  mIsVisible(true) {
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
  //TODO: implement button rendering

  for (UIText* text : mTexts) {
    text->Draw(mRenderer, mPos);
  }
}

void UIScreen::Draw() {

  for (UIText* text : mTexts) {
    text->Draw(mRenderer, mPos);
  }
}

void UIScreen::ProcessInput(const uint8_t *keys) {
  if (!keys) return;
  // Forward any other pressed scancodes to the handler
  for (int sc = 0; sc < SDL_NUM_SCANCODES; ++sc) {
    if (keys[sc]) {
      HandleKeyPress(sc);
    }
  }
}

void UIScreen::HandleKeyPress(int key) {
}

UIText *UIScreen::AddText(const std::string &name, const Vector2 &pos,
                          const Vector2 &dims, const int pointSize,
                          Vector3 color, const int unsigned wrapLength) {
  UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, color);
  mTexts.emplace_back(t);

  return t;
}

void UIScreen::ChangeResolution(float oldScale, float newScale) {
  mPos.x = mPos.x / oldScale * newScale;
  mPos.y = mPos.y / oldScale * newScale;
  mSize.x = mSize.x / oldScale * newScale;
  mSize.y = mSize.y / oldScale * newScale;

  //TODO: Change resolution for buttons

  for (UIText* text: mTexts) {
    text->ChangeResolution(oldScale, newScale);
  }
}
