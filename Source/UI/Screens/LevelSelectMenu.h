#pragma once

#include "UIScreen.h"

class LevelSelectMenu : public UIScreen {
public:
  LevelSelectMenu(class Game* game, const std::string& fontName);

private:
  void CreateLevelButtons(int levelCount);
};