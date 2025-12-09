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
  mTimer = new Timer(game, 60.0f,
    Vector2((GetSize().x / 2.0f), GetSize().y / 30.0f),
    Vector2::Zero,
    Color::White, mFont);
  AddText("Ammo: ", Vector2(40.0f * mGame->GetGameScale(), 10.0f * mGame->GetGameScale()),
    Vector2(20.0f * mGame->GetGameScale(), 30.0f * mGame->GetGameScale()) ,
    16, Color::White);
  mShootingModeText = AddText("Pistol", Vector2(80.0f * mGame->GetGameScale(), 11.0f * mGame->GetGameScale()),
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

  // Update tutorial fade
  if (mShowTutorial) {
    mTutorialTimer -= deltaTime;
    if (mTutorialTimer <= 0.0f) {
      mShowTutorial = false;
      // Hide tutorial texts
      for (auto* text : mTutorialTexts) {
        text->SetVisible(false);
      }
    } else if (mTutorialTimer < 2.0f) {
      // Fade out in last 2 seconds
      float alpha = mTutorialTimer / 2.0f;
      for (auto* text : mTutorialTexts) {
        Vector4 color = text->GetColor();
        color.w = alpha;
        text->SetColor(color);
      }
    }
  }
}

void HUD::ShowTutorial(float duration) {
  mShowTutorial = true;
  mTutorialTimer = duration;

  float scale = static_cast<float>(mGame->GetGameScale());
  float centerX = GetSize().x / 2.0f;
  float startY = GetSize().y * 0.15f;  // Top area of screen
  float lineHeight = 28.0f * scale;

  // Create tutorial texts
  UIText* t1 = AddText("[A] [D] - Move", 
    Vector2(centerX, startY),
    Vector2(300.0f * scale, 30.0f * scale),
    14, Color::White);
  
  UIText* t2 = AddText("[W] - Jump", 
    Vector2(centerX, startY + lineHeight),
    Vector2(300.0f * scale, 30.0f * scale),
    14, Color::White);

  UIText* t3 = AddText("[Right Click] - Aim", 
    Vector2(centerX, startY + lineHeight * 2),
    Vector2(300.0f * scale, 30.0f * scale),
    14, Color::White);

  UIText* t4 = AddText("[Left Click] - Shoot", 
    Vector2(centerX, startY + lineHeight * 3),
    Vector2(300.0f * scale, 30.0f * scale),
    14, Color::White);

  UIText* t5 = AddText("[1] Pistol  [2] Blue Portal  [3] Orange Portal", 
    Vector2(centerX, startY + lineHeight * 4),
    Vector2(400.0f * scale, 30.0f * scale),
    14, Color::Yellow);

  mTutorialTexts.push_back(t1);
  mTutorialTexts.push_back(t2);
  mTutorialTexts.push_back(t3);
  mTutorialTexts.push_back(t4);
  mTutorialTexts.push_back(t5);
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

void HUD::SetTimerTime(float timeLeft) {
  mTimer->SetTimeLeft(timeLeft);
}

ShootingMode HUD::GetCurrentShootingMode() {
    XenoGun* xenoGun = mGame->GetPlayer()->GetXenoGun();
    return xenoGun->GetShootingMode();
}

