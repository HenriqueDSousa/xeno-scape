//
// Created by henrique on 11/5/25.
//

#pragma once
#include <SDL_ttf.h>

#include <string>
#include <unordered_map>

#include "../Math.h"

class Renderer;
class Vector3;

class Font
{
 public:
  Font(Renderer* renderer);
  ~Font();
  // Load/unload from a file
  bool Load(const std::string& fileName);
  void Unload();
  // Given string and this font, draw to a texture
  class Texture* RenderText(const std::string& text,
  const Vector3& color = Color::White, int pointSize = 24,
  int wrapLength = 900);
 private:
  // Map of point sizes to font data
  std::unordered_map<int, TTF_Font*> mFontData;

  Renderer* mRenderer;
};
