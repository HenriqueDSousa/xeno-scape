#include "LevelSelectMenu.h"
#include "../../Game.h"

LevelSelectMenu::LevelSelectMenu(Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
  SetSize(Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT));

  AddText("Select Level",
      Vector2(GetSize().x / 2, GetSize().y / 6),
      Vector2::Zero,
      36,
      Color::White);

  // Set how many levels are available
  const int levelCount = 4;   // IMPORTANT: Only 4 scenes exist right now
  CreateLevelButtons(levelCount);

  AddButton("Back",
      Vector2(GetSize().x / 2, GetSize().y / 1.15f),
      Vector2(GetSize().x / 4, 50.f),
      24,
      UIButton::TextPos::Center,
      [this]() {
          mGame->SetScene(GameScene::MainMenu);
      },
      Vector2::Zero,
      Color::White);
}

void LevelSelectMenu::CreateLevelButtons(int levelCount)
{
  float startY = GetSize().y / 3;
  float spacing = 60.0f;
  float buttonWidth = GetSize().x / 4;

  for (int i = 0; i < levelCount; i++)
  {
    AddButton(
        "Level " + std::to_string(i + 1),
        Vector2(GetSize().x / 2, startY + i * spacing),
        Vector2(buttonWidth, 50.f),
        24,
        UIButton::TextPos::Center,
        [this, i]() {
            // Map i to the matching scene
            GameScene target = static_cast<GameScene>(static_cast<int>(GameScene::Level1) + i);
            mGame->SetScene(target);   // ← triggers fade & ApplySceneChange
        },
        Vector2::Zero,
        Color::White
    );
  }
}
