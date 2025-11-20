//
// Created by henrique on 11/20/25.
//

#pragma once
#include "UIScreen.h"

class GameOver : public UIScreen {
 public:
  void Update(float deltaTime) override;
  void Draw() override;
};

