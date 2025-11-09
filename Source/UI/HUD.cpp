//
// Created by henrique on 11/5/25.
//

#include "HUD.h"

#include "../Game.h"

HUD::HUD(class Game* game, const std::string& fontName)
: UIScreen(game, fontName),
  mIsPaused(true) {
  mTimer = new Timer(60.0f,
    Vector2(Game::LEVEL_WIDTH / 2.0f, 10.0f),
    Vector2(200.0f, 50.0f),
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
  mTimer->Draw(mRenderer, mTimer->GetPosition());
}

void HUD::ChangeResolution(float oldScale, float newScale) {
  mTimer->ChangeResolution(oldScale, newScale);
}
