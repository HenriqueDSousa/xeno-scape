//
// Created by henrique on 11/5/25.
//

#pragma once
#include <cstdint>
#include <string>

#include "../Math.h"

class Vector3;

class UIScreen
{
public:
    UIScreen(class Game* game);
    virtual ~UIScreen();
    // UIScreen subclasses can override these
    virtual void Update(float deltaTime);
    virtual void Draw();
    virtual void ProcessInput(const uint8_t* keys);
    virtual void HandleKeyPress(int key);
     // Tracks if the UI is active or closing
     enum UIState { EActive, EClosing };
    // Set state to closing
    void Close() { mState = EClosing; }
    // Get state of UI screen
    UIState GetState() const { return mState; }
    // Change the title text
    void SetTitle(const std::string& text,
    const Vector3& color = Color::White,
    int pointSize = 40);
protected:
    // Helper to draw a texture
    void DrawTexture(class Shader* shader, class Texture* texture,
    const Vector2& offset = Vector2::Zero,
    float scale = 1.0f);
    class Game* mGame;
    class Renderer* mRenderer;
    // For the UI screen's title text
    class Font* mFont;
    class Texture* mTitle;
    Vector2 mTitlePos;
    // State
    UIState mState;


};

