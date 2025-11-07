//
// Created by henrique on 11/5/25.
//

#include "PauseMenu.h"

#include "../../Game.h"
PauseMenu::PauseMenu(class Game* game, const std::string& fontName)
: UIScreen(game, fontName) {
  SetSize(Vector2(Game::WINDOW_WIDTH/3, Game::WINDOW_HEIGHT/3));
  SetPosition(Vector2(Game::WINDOW_WIDTH/3, 5 * Game::WINDOW_HEIGHT/12));
  std::string_view text = "Game Paused";
        AddText(std::string(text),Vector2::Zero, Vector2::Zero,
                30,
                Color::White);
}

void PauseMenu::Update(float deltaTime) { UIScreen::Update(deltaTime); }

void PauseMenu::Draw() { UIScreen::Draw(); }

void PauseMenu::ProcessInput(const uint8_t* keys) {
}

void PauseMenu::HandleKeyPress(int key) {
}