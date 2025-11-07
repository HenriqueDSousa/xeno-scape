#include "Texture.h"

Texture::Texture()
: mTextureID(0)
, mWidth(0)
, mHeight(0)
{
}

Texture::~Texture()
{
}

bool Texture::LoadFromSurface(SDL_Surface *surface) {
    mHeight = surface->h;
    mWidth = surface->w;
    // Ensure surface is in a known RGBA32 format
    // Convert to SDL_PIXELFORMAT_RGBA32 so we can reliably use GL_RGBA.
    SDL_Surface* formatted = surface;
    if (surface->format->format != SDL_PIXELFORMAT_RGBA32) {
        formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(surface);
        if (!formatted) {
            SDL_Log("Failed to convert surface to RGBA32 format");
            return false;
        }
    }

    mHeight = formatted->h;
    mWidth = formatted->w;

    // GPU alloc with OpenGL
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        mWidth,
        mHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        formatted->pixels
    );
    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (formatted != surface) {
        SDL_FreeSurface(formatted);
    } else {
        SDL_FreeSurface(surface);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

bool Texture::Load(const std::string &filePath)
{
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface)
    {
        SDL_Log("Falha ao carregar a textura: %s", filePath.c_str());
        return false;
    }

    return LoadFromSurface(surface);
}

void Texture::Unload()
{
    glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive(int index) const
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}

bool Texture::CreateFromSurface(SDL_Surface *surface) {
    if (!surface)
    {
        SDL_Log("Failure on creating texture from surface");
        return false;
    }
    return LoadFromSurface(surface);
}
