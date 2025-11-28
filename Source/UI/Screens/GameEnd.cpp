//
// Created by henrique on 11/27/25.
//

#include "GameEnd.h"

#include "../../Game.h"

GameEnd::GameEnd(Game* game, const std::string& fontName)
  :UIScreen(game, fontName) {
  mGame->SetState(GameState::GameOver);
  mSize = Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

  // Victory title
  AddText("XENO ESCAPED!", Vector2(2 * GetSize().x / 4, GetSize().y / 4),
          Vector2::Zero,
          40,
          Color::White);

  // Congratulations message
  AddText("Congratulations!", Vector2(2 * GetSize().x / 4, GetSize().y / 4 + 60),
          Vector2::Zero,
          24,
          Color::White);

  // Restart button
  AddButton("Play Again", Vector2(2 * GetSize().x / 4, GetSize().y / 2),
          Vector2(GetSize().x / 4, 50.f),
          24,
          UIButton::TextPos::Center,
          [this]() {
            mGame->SetScene(GameScene::Level1);
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