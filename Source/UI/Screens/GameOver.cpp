//
// Created by henrique on 11/20/25.
//

#include "GameOver.h"

#include "../../Game.h"

GameOver::GameOver(class Game* game, const std::string& fontName)
  :UIScreen(game, fontName) {
  mGame->SetState(GameState::GameOver);
  mSize = Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

  // Game Over title
  AddText("GAME OVER", Vector2(2 * GetSize().x / 4, GetSize().y / 4),
          Vector2::Zero,
          40,
          Color::White);

  // Restart button
  AddButton("Restart", Vector2(2 * GetSize().x / 4, GetSize().y / 2),
          Vector2(GetSize().x / 4, 50.f),
          24,
          UIButton::TextPos::Center,
          [this]() {
            mGame->SetState(GameState::Gameplay);
            mGame->SetScene(mGame->GetCurrentScene());
            Close();
          },
          Vector2::Zero,
          Color::White);

  // Main Menu button
  AddButton("Main Menu", Vector2(2 * GetSize().x / 4, GetSize().y / 2 + 80),
          Vector2(GetSize().x / 4, 50.f),
          24,
          UIButton::TextPos::Center,
          [this]() {
            mGame->SetScene(GameScene::MainMenu);
            Close();
          },
          Vector2::Zero,
          Color::White);

  // Quit button
  AddButton("Quit", Vector2(2 * GetSize().x / 4, GetSize().y / 2 + 160),
          Vector2(GetSize().x / 4, 50.f),
          24,
          UIButton::TextPos::Center,
          [this]() {
            mGame->Quit();
          },
          Vector2::Zero,
          Color::White);
}