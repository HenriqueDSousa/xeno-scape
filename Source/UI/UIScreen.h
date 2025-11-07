//
// Created by henrique on 11/5/25.
//

#pragma once
#include <string>

#include "../Math.h"
#include "UIText.h"

class Vector3;

class UIScreen
{
public:
  // Tracks if the UI is active or closing
  enum UIState { EActive, EClosing };

  UIScreen(class Game* game, const std::string& fontName);
  virtual ~UIScreen();
  // UIScreen subclasses can override these
  virtual void Update(float deltaTime);
  virtual void Draw();
  virtual void ProcessInput(const uint8_t* keys);
  virtual void HandleKeyPress(int key);

  void SetPosition(Vector2 position) { mPos = position; }
  void SetSize(Vector2 size) { mSize = size; }
  Vector2 GetPosition() const { return mPos; }
  Vector2 GetSize() const { return mSize; }

  // Set state to closing
  void Close() { mState = EClosing; }
  // Get state of UI screen
  UIState GetState() const { return mState; }

  std::vector<UIText *> GetTexts() { return mTexts; }
  UIText* AddText(const std::string& name, const Vector2& pos = Vector2::Zero,
    const Vector2& dims = Vector2::Zero, const int pointSize = 40, Vector3 color = Color::White,
    const int unsigned wrapLength = 1024);

  virtual void ChangeResolution(float oldScale, float newScale);
protected:
  class Game* mGame;
  class Renderer* mRenderer;
  class Font* mFont;

  Vector2 mTitlePos;;
  // Configure positions
  Vector2 mPos;
  Vector2 mSize;

  // State
  UIState mState;

  bool mIsVisible;

  std::vector<UIText *> mTexts;
};

