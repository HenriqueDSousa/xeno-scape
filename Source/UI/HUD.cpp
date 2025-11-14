//
// Created by henrique on 11/5/25.
//

#include "HUD.h"

#include "../Game.h"

HUD::HUD(class Game* game, const std::string& fontName)
: UIScreen(game, fontName),
  mIsPaused(true) {
  SetPosition(Vector2(Vector2::Zero));
  SetSize(Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT));
  mTimer = new Timer(60.0f,
    Vector2((GetSize().x / 2.0f), GetSize().y / 30.0f),
    Vector2::Zero,
    Color::White, mFont);
}

void HUD::Update(float deltaTime) {
  if (mIsPaused) {
    return;
  }
  mTimer->Update(deltaTime);
}

void HUD::Draw() {
  if (!mIsVisible) {
    return;
  }
  mTimer->Draw(mRenderer, mPos);
}

void HUD::ChangeResolution(float oldScale, float newScale) {
  mTimer->ChangeResolution(oldScale, newScale);
}
