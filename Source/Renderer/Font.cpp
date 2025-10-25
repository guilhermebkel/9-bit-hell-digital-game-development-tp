#include "../Renderer/Font.h"
#include "../Game.h"
#include <vector>

Font::Font(class Game* game)
    : mGame(game)
{
}

Font::~Font()
{
}

bool Font::Load(const std::string& fileName)
{
    std::vector<int> pointSizes = { 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 48, 72 };

    for (auto& size : pointSizes)
    {
        TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
        if (font == nullptr)
        {
            SDL_Log("Failed to load font %s in size %d", fileName.c_str(), size);
            return false;
        }
        mFontData.emplace(size, font);
    }
    return true;
}

void Font::Unload()
{
    for (auto& font : mFontData)
    {
        TTF_CloseFont(font.second);
    }
    mFontData.clear();
}

Texture* Font::RenderText(const std::string& text, const Vector3& color, int pointSize)
{
    Texture* texture = nullptr;

    SDL_Color sdlColor;
    sdlColor.r = static_cast<Uint8>(color.x * 255);
    sdlColor.g = static_cast<Uint8>(color.y * 255);
    sdlColor.b = static_cast<Uint8>(color.z * 255);
    sdlColor.a = 255;

    auto iter = mFontData.find(pointSize);
    if (iter != mFontData.end())
    {
        TTF_Font* font = iter->second;
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), sdlColor);
        if (surf != nullptr)
        {
            texture = new Texture();
            texture->CreateFromSurface(surf);
            SDL_FreeSurface(surf);
        }
    }
    else
    {
        SDL_Log("Point size %d is not supported", pointSize);
    }

    return texture;
}