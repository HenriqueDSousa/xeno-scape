//
// Created by henrique on 11/9/25.
//

#include "MainMenu.h"

#include "../../Game.h"
MainMenu::MainMenu(Game* game, const std::string& fontName)
: UIScreen(game, fontName) {
  mSize = Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

  AddText("Xeno Scape", Vector2(GetSize().x / 2, GetSize().y / 4),
          Vector2::Zero,
          40,
          Color::White);

  AddButton("Start Game", Vector2( GetSize().x / 2, GetSize().y / 2),
          Vector2(GetSize().x / 4, 50.f),
          24,
          UIButton::TextPos::Center,
          [this]() {
            mGame->SetNextScene();
          },
          Vector2::Zero,
          Color::White);

  AddButton("Level Select",
          Vector2(GetSize().x / 2, GetSize().y / 2 + 60.0f),
          Vector2(GetSize().x / 4, 50.f),
          24,
          UIButton::TextPos::Center,
          [this]() {
              mGame->SetScene(GameScene::LevelSelect);
          },
          Vector2::Zero,
          Color::White);

  AddButton("Quit", Vector2(GetSize().x / 2, GetSize().y / 2 + 120.0f),
        Vector2(GetSize().x / 4, 50.f),
        24,
        UIButton::TextPos::Center,
        [this]() {
          mGame->Quit();
        },
        Vector2::Zero,
        Color::White);
}