#include "Texture.h"
#include <GL/glew.h>
#include <SDL2/SDL_image.h>

Texture::Texture()
: mTextureID(0)
, mWidth(0)
, mHeight(0)
{
}

Texture::~Texture()
{
}

bool Texture::Load(const std::string &filePath)
{
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface)
    {
        SDL_Log("Failed to load texture file %s", filePath.c_str());
        return false;
    }

    mWidth = surface->w;
    mHeight = surface->h;

    int format = GL_RGB;
    if (surface->format->BytesPerPixel == 4)
    {
        format = GL_RGBA;
    }

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return true;
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