//
// Created by henrique on 11/5/25.
//

#pragma once
#include "Timer.h"
#include "UIScreen.h"

class HUD : public UIScreen {
 public:

  HUD(class Game* game, const std::string& fontName);

  virtual void Update(float deltaTime);
  void Draw() override;
  void ChangeResolution(float oldScale, float newScale) override;
  void SetVisible(bool isVisible) { mIsVisible = isVisible; }
  void SetPaused(bool isPaused) { mIsPaused = isPaused; }

private:
  Timer* mTimer;
  bool mIsPaused;
};

