//
// Created by henrique on 11/5/25.
//

#include "UIScreen.h"

#include "../Game.h"
#include "Font.h"

UIScreen::UIScreen(class Game *game)
: mGame(game) {
  mGame->PushUI(this);
}

UIScreen::~UIScreen() {
  mGame->RemoveUI(this);
}

void UIScreen::Update(float deltaTime) {
}

void UIScreen::Draw() {
}

void UIScreen::ProcessInput(const uint8_t *keys) {
  if (!keys) return;
  // Forward any other pressed scancodes to the handler
  for (int sc = 0; sc < SDL_NUM_SCANCODES; ++sc) {
    if (keys[sc]) {
      HandleKeyPress(sc);
    }
  }
}

void UIScreen::HandleKeyPress(int key) {
}

void UIScreen::SetTitle(const std::string &text, const Vector3 &color, int pointSize) {
}

void UIScreen::DrawTexture(class Shader *shader, class Texture *texture, const Vector2 &offset, float scale) {
}
