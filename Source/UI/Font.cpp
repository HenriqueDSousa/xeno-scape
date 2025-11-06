//
// Created by henrique on 11/5/25.
//

#include "Font.h"

#include <vector>

#include "../Renderer/Texture.h"

bool Font::Load(const std::string &fileName) {
    // Support these font sizes
    std::vector<int> fontSizes = {
        8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28,
        30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 52, 56,
        60, 64, 68, 72
        };
    // Call TTF_OpenFont once per every font size
    for (auto& size : fontSizes)
    {
        TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
        if (font == nullptr)
        {
            SDL_Log("Failed to load font %s in size %d", fileName.c_str(),
           size);
            return false;
        }
        mFontData.emplace(size, font);
    }
    return true;
}

Texture* Font::RenderText(const std::string &text,
                        const Vector3 &color,
                        int pointSize) {
    Texture* texture = nullptr;
    // Convert to SDL_Color
    SDL_Color sdlColor;
    sdlColor.r = static_cast<Uint8>(color.x * 255);
    sdlColor.g = static_cast<Uint8>(color.y * 255);
    sdlColor.b = static_cast<Uint8>(color.z * 255);
    sdlColor.a = 255;
    // Find the font data for this point size
    auto iter = mFontData.find(pointSize);
    if (iter != mFontData.end())
    {
        TTF_Font* font = iter->second;
        // Draw this to a surface (blended for alpha)
        SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(),
        sdlColor);
        if (surf != nullptr)
        {
            // Convert from surface to texture
            texture = new Texture();
            texture->CreateFromSurface(surf);
            SDL_FreeSurface(surf);
        }
    }
    else
    {
        SDL_Log("Point size %d is unsupported", pointSize);
    }
    return texture;
}