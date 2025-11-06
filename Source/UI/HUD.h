//
// Created by henrique on 11/5/25.
//

#pragma once
#include "UIScreen.h"

class HUD : public UIScreen {
public:
    HUD(class Game* game);
    virtual ~HUD();

    virtual void Update(float deltaTime);
    virtual void Draw(class Shader* shader);
    virtual void ProcessInput(const uint8_t* keys);

};

