//
// Created by henrique on 11/5/25.
//

#include "HUD.h"

#include "../Actors/XenoGun.h"
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
  AddText("Ammo: ", Vector2(20.0f * mGame->GetGameScale(), 10.0f * mGame->GetGameScale()),
    Vector2(20.0f * mGame->GetGameScale(), 30.0f * mGame->GetGameScale()) ,
    16, Color::White);
  mShootingModeText = AddText("Pistol", Vector2((60.0f) * mGame->GetGameScale(), 11.0f * mGame->GetGameScale()),
    Vector2(20.0f * mGame->GetGameScale(), 30.0f * mGame->GetGameScale()) ,
    12, Color::Yellow);
}

void HUD::Update(float deltaTime) {
  if (mIsPaused) {
    return;
  }
  mTimer->Update(deltaTime);
  switch (GetCurrentShootingMode()) {
    case ShootingMode::SHOOT:
      mShootingModeText->SetText("Pistol");
      mShootingModeText->SetColor(Color::Yellow);
      break;
    case ShootingMode::PORTAL_BLUE:
      mShootingModeText->SetText("Blue Portal ");
      mShootingModeText->SetColor(Color::Blue);
      break;
    case ShootingMode::PORTAL_ORANGE:
      mShootingModeText->SetText("Orange Portal ");
      mShootingModeText->SetColor(Color::Orange);
      break;
    default:
      break;
  }
}

void HUD::Draw() {
  if (!mIsVisible) {
    return;
  }
  UIScreen::Draw();
  mTimer->Draw(mRenderer, mPos);
}

void HUD::ChangeResolution(float oldScale, float newScale) {
  UIScreen::ChangeResolution(oldScale, newScale);
  mTimer->ChangeResolution(oldScale, newScale);
}

ShootingMode HUD::GetCurrentShootingMode() {
    XenoGun* xenoGun = mGame->GetPlayer()->GetXenoGun();
    return xenoGun->GetShootingMode();
}

