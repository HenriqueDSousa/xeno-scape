//
// Created by henrique on 11/5/25.
//

#pragma once
#include "../Actors/XenoGun.h"
#include "Screens/UIScreen.h"
#include "Timer.h"

enum class ShootingMode;

class HUD : public UIScreen {
 public:

  HUD(class Game* game, const std::string& fontName);

  virtual void Update(float deltaTime);
  void Draw() override;
  void ChangeResolution(float oldScale, float newScale) override;
  void SetVisible(bool isVisible) { mIsVisible = isVisible; }
  void SetPaused(bool isPaused) { mIsPaused = isPaused; }
  void SetTimerTime(float timeLeft);

private:
  ShootingMode GetCurrentShootingMode();

  Timer* mTimer;
  UIText* mShootingModeText;
  bool mIsPaused;
};

