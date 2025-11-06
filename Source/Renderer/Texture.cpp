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
        surface->pixels
    );
    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Freeing memory and unbinding variable
    SDL_FreeSurface(surface);
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
