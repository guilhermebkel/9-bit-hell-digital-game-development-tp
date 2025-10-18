// File Path: ./Source/Renderer/Texture.cpp

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
    SDL_Surface* originalSurface = IMG_Load(filePath.c_str());
    if (!originalSurface)
    {
        SDL_Log("Falha ao carregar o arquivo de textura %s - Erro: %s", filePath.c_str(), IMG_GetError());
        return false;
    }

    SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(originalSurface, SDL_PIXELFORMAT_RGBA32, 0);

    SDL_FreeSurface(originalSurface);

    if (!convertedSurface)
    {
        SDL_Log("Failed to convert surface to RGBA32 for the file: %s", filePath.c_str());
        return false;
    }

    mWidth = convertedSurface->w;
    mHeight = convertedSurface->h;

    GLenum format = GL_RGBA;

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, convertedSurface->pixels);

    SDL_FreeSurface(convertedSurface);

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