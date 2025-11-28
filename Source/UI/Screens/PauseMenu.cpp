//
// Created by henrique on 11/5/25.
//

#include "PauseMenu.h"
#include "../../Game.h"

PauseMenu::PauseMenu(class Game* game, const std::string& fontName)
: UIScreen(game, fontName) {
  SetSize(Vector2(Game::WINDOW_WIDTH,Game::WINDOW_HEIGHT));
  AddText("Game Paused",Vector2(GetSize().x / 2, GetSize().y / 2 - 120.0f),
    Vector2::Zero,
    30,
    Color::White);

  AddButton("Resume", Vector2(GetSize().x / 2, GetSize().y / 2),
          Vector2(GetSize().x / 4, 50.f),
          24,
          UIButton::TextPos::Center,
          [this]() {
            mGame->OnResume();
          },
          Vector2::Zero,
          Color::White);
  AddButton("Quit", Vector2(GetSize().x / 2, GetSize().y / 2 + 60.0f),
      Vector2(GetSize().x / 4, 50.f),
      24,
      UIButton::TextPos::Center,
      [this]() {
        mGame->Quit();
      },
      Vector2::Zero,
      Color::White);
}