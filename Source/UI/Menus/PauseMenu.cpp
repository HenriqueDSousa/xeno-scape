//
// Created by henrique on 11/5/25.
//

#include "PauseMenu.h"

#include "../../Game.h"
PauseMenu::PauseMenu(class Game* game)
: UIScreen(game) {
  SetTitle("Paused", Color::White);
}

void PauseMenu::Update(float deltaTime) { UIScreen::Update(deltaTime); }

void PauseMenu::Draw() { UIScreen::Draw(); }

void PauseMenu::ProcessInput(const uint8_t* keys) {
}

void PauseMenu::HandleKeyPress(int key) {
}