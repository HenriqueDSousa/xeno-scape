//
// Created by henrique on 11/9/25.
//

#include "MainMenu.h"

#include "../../Game.h"
MainMenu::MainMenu(Game* game, const std::string& fontName)
: UIScreen(game, fontName) {
  mSize = Vector2(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
  AddButton("Start Game", Vector2(GetSize().x / 4, GetSize().y / 3),
          Vector2(GetSize().x / 2, 50.f),
          24,
          UIButton::TextPos::Center,
          [this]() {
            mGame->SetScene(GameScene::TestLevel);
          },
          Vector2::Zero,
          Color::White);
  AddButton("Quit", Vector2(GetSize().x / 4, 2 * GetSize().y / 3),
        Vector2(GetSize().x / 2, 50.f),
        24,
        UIButton::TextPos::Center,
        [this]() {
          mGame->Quit();
        },
        Vector2::Zero,
        Color::White);
}