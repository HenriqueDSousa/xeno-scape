//
// Created by henrique on 11/5/25.
//

#include "HUD.h"

HUD::HUD(class Game* game)
: UIScreen(game){}

HUD::~HUD() {

}

void HUD::Update(float deltaTime) { UIScreen::Update(deltaTime); }

void HUD::Draw(class Shader* shader) {}

void HUD::ProcessInput(const uint8_t* keys) { UIScreen::ProcessInput(keys); }