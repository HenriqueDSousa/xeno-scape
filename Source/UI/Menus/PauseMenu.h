//
// Created by henrique on 11/5/25.
//

#pragma once
#include "../UIScreen.h"

class PauseMenu : public UIScreen {
 public:
  PauseMenu(class Game *game, const std::string& fontName);

  void Update(float deltaTime) override;
  void Draw() override;
  void ProcessInput(const uint8_t* keys) override;
  void HandleKeyPress(int key) override;

};
