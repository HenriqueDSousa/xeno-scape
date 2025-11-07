#pragma once
#include <string>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);
	void Unload();

	void SetActive(int index = 0) const;

	bool LoadFromSurface(SDL_Surface *surface);

	static GLenum SDLFormatToGL(SDL_PixelFormat* fmt);

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

	unsigned int GetTextureID() const { return mTextureID; }
	bool CreateFromSurface(SDL_Surface* surface);

private:
	unsigned int mTextureID;
	int mWidth;
	int mHeight;
};
